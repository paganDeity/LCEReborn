#!/usr/bin/env python3
"""Generate the mod API from api/lce.api.

    hookgen.py generate    write the generated sources, the SDK header and docs/modding-api.md
    hookgen.py check       fail if a generated file is out of date or a hook is not called in the game

The description file is the single source. A `fn` is something a mod can call, a
`hook` is a point in the game where mods are asked before the game decides. Each hook
becomes a typed function in `ModEvents` (ModEvents_generated.hpp); the game calls it at
the `target` function and `check` fails if the call is missing.
"""
import argparse
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
API = os.path.join(ROOT, "api", "lce.api")
# the functions and hooks API version 1 promises, in the order that numbers them for WebAssembly
STABLE = os.path.join(ROOT, "api", "stable.txt")
SCALARS = ("bool", "int", "float", "string", "bytes")
# game objects a hook turns into a value for the mod: the C++ type the game passes, and what the mod sees
CONVERTED = {
    "block": ("Block*", "string", "AddBlock"),
    "item": ("Item*", "string", "AddItem"),
    "sound": ("const SoundEvent*", "string", "AddSound"),
    "entity_type": ("const boost::shared_ptr<Entity>&", "string", "AddEntityType"),
    "damage": ("const DamageSource*", "int", "AddDamage"),
    "light_layer": ("LightLayer::variety", "int", "AddLightLayer"),
}
NATIVE = {"bool": "bool", "int": "int", "float": "float", "string": "const std::wstring&", "bytes": "const std::string&"}
NATIVE_MUT = {"bool": "bool&", "int": "int&", "float": "float&", "string": "std::wstring&", "bytes": "std::string&"}
GETTERS = {"bool": "GetBool", "int": "GetInt", "float": "GetFloat", "string": "GetString", "bytes": "GetBytes"}


class ApiError(Exception):
    pass


def split_arguments(text, line):
    arguments = []
    for part in [p.strip() for p in text.split(",") if p.strip()]:
        match = re.match(r"^(\w+)\s*:\s*(\w+)(\s+mut)?(\s+from\s+(\w+))?$", part)
        if not match:
            raise ApiError("line %d: bad argument %r" % (line, part))
        arguments.append({"name": match.group(1), "type": match.group(2), "mut": bool(match.group(3)), "from": match.group(5)})
    return arguments


def strip_debug(text):
    """The header as every build sees it: what sits under #if LCE_DEBUG_BUILD or #if LCE_PLATFORM_PC is left out (its #else part
    stays), so the API is the same in every build and on every platform."""
    out, stack = [], []
    for line in text.split("\n"):
        stripped = line.strip()
        if stripped.startswith(("#if", "#ifdef", "#ifndef")):
            stack.append("debug" if re.match(r"#if\s+(LCE_DEBUG_BUILD|LCE_PLATFORM_PC)\s*$", stripped) else "other")
            if stack[-1] == "debug":
                continue
        elif stripped.startswith("#else") and stack:
            if stack[-1] == "debug":
                stack[-1] = "else"
                continue
        elif stripped.startswith("#endif") and stack:
            if stack.pop() in ("debug", "else"):
                continue
        if "debug" not in stack:
            out.append(line)
    return "\n".join(out)


def parse(path):
    api = {"version": 0, "handles": [], "functions": [], "hooks": [], "classes": [], "exposed": [], "woven": []}
    current = None
    with open(path, encoding="utf-8") as handle:
        lines = handle.read().split("\n")
    for number, raw in enumerate(lines, 1):
        text = raw.strip()
        if not text or text.startswith("#"):
            continue
        if text.startswith("api "):
            api["version"] = int(text.split()[1])
        elif text.startswith("handle "):
            match = re.match(r'^handle (\w+)\s+"(.*)"\s+type\s+(.+)$', text)
            if not match:
                raise ApiError("line %d: bad handle" % number)
            if re.match(r"^[\w:]+$", match.group(3).strip()) and match.group(1) not in ("entity",):
                VALUE_HANDLES[match.group(3).strip().split("::")[-1]] = match.group(1)
            owned = re.match(r"^boost::shared_ptr<(\w+)>$", match.group(3).strip())
            if owned and owned.group(1) not in ENTITY_FAMILY:
                SHARED_HANDLES[owned.group(1)] = match.group(1)
            api["handles"].append({"name": match.group(1), "doc": match.group(2), "cpp": match.group(3).strip(), "native": ("const %s&" % match.group(3).strip()) if ("shared_ptr" in match.group(3) or "GuiInfo" in match.group(3)) else match.group(3).strip()})
        elif text.startswith("family "):
            match = re.match(r"^family (\w+) of (\w+)$", text)
            if not match:
                raise ApiError("line %d: bad family" % number)
            api.setdefault("raw_families", {})[match.group(1)] = match.group(2)
        elif text.startswith("shared_cast "):
            match = re.match(r"^shared_cast (\w+) to (\w+)$", text)
            if not match:
                raise ApiError("line %d: bad shared_cast" % number)
            api.setdefault("shared_casts", []).append((match.group(1), match.group(2)))
        elif text.startswith("persistent "):
            api.setdefault("persistent", set()).add(text.split()[1])
        elif text.startswith("owned "):
            api.setdefault("owned", set()).add(text.split()[1])
        elif text.startswith("alias "):
            match = re.match(r"^alias (\w+)\.(\w+) (\w+)\.(\w+)$", text)
            if not match:
                raise ApiError("line %d: bad alias" % number)
            api.setdefault("aliases", {})[(match.group(1), match.group(2))] = (match.group(3), match.group(4))
        elif text.startswith("expose "):
            match = re.match(r"^expose (\w+) from (\S+)$", text)
            if not match:
                raise ApiError("line %d: bad expose" % number)
            api["exposed"].append({"class": match.group(1), "headers": match.group(2).split(",")})
            current = None
        elif text.startswith("intercept "):
            match = re.match(r"^intercept (\w+) from (\S+)(?: only (\S+))?$", text)
            if not match:
                raise ApiError("line %d: bad intercept" % number)
            api["woven"].append({"name": match.group(1), "self": match.group(1), "files": match.group(2).split(","), "shared": match.group(1) in ENTITY_FAMILY, "skipped": [],
                                 "woven": True, "only": match.group(3).split(",") if match.group(3) else None})
            current = None
        elif text.startswith("singleton "):
            match = re.match(r"^singleton (\w+)(\*?) (\w+) from (\S+)$", text)
            if not match:
                raise ApiError("line %d: bad singleton" % number)
            api.setdefault("singletons", []).append({"class": match.group(1), "pointer": bool(match.group(2)), "variable": match.group(3), "header": match.group(4)})
            current = None
        elif text.startswith("mobbase "):
            match = re.match(r"^mobbase (\w+) (\w+) (\w+) from (\S+)$", text)
            if not match:
                raise ApiError("line %d: bad mobbase" % number)
            api.setdefault("mobbases", []).append({"name": match.group(1), "type": match.group(2), "class": match.group(3), "header": match.group(4)})
            current = None
        elif text.startswith("override "):
            match = re.match(r"^override (\w+) from (\S+) to (\S+) self (\w+)( shared)?$", text)
            if not match:
                raise ApiError("line %d: bad override" % number)
            api["classes"].append({"name": match.group(1), "headers": match.group(2).split(","), "output": match.group(3), "self": match.group(4),
                                   "shared": bool(match.group(5))})
            current = None
        elif text.startswith("fn "):
            match = re.match(r"^fn (static )?(\w+)\.(\w+)\((.*)\)(\s*->\s*(\w+))?$", text)
            if not match:
                raise ApiError("line %d: bad fn" % number)
            current = {"static": bool(match.group(1)), "module": match.group(2), "name": match.group(3),
                       "args": split_arguments(match.group(4), number), "result": match.group(6), "doc": ""}
            api["functions"].append(current)
        elif text.startswith("hook "):
            match = re.match(r"^hook (\w+)\((.*)\)(\s+cancel)?(\s+provider)?$", text)
            if not match:
                raise ApiError("line %d: bad hook" % number)
            current = {"name": match.group(1), "args": split_arguments(match.group(2), number), "cancel": bool(match.group(3)),
                       "provider": bool(match.group(4)),
                       "doc": "", "targets": []}
            api["hooks"].append(current)
        elif text.startswith('"') and current is not None:
            current["doc"] = text.strip('"')
        elif text.startswith("target ") and current is not None:
            current["targets"].append({"target": text.split(None, 1)[1]})
        else:
            raise ApiError("line %d: unexpected %r" % (number, text))
    for cls in api["classes"]:
        cls["skipped"] = []
        api["hooks"] += class_hooks(cls, api)
    for cls in api["woven"]:
        api["hooks"] += weave_hooks(cls, api)
    expose_classes(api)
    validate(api)
    stabilise(api)
    return api


def fn_call(fn):
    arguments = ", ".join("%s: %s" % (a["name"], a["type"]) for a in (fn["args"] if fn["static"] else fn["args"][1:]))
    return "%s%s%s(%s)" % (fn["module"], "." if fn["static"] else ":", fn["name"], arguments)


def fn_signature(fn):
    return fn_call(fn) + ((" -> " + fn["result"]) if fn["result"] else "")


def hook_call(hook):
    return "%s(%s)" % (hook["name"], ", ".join("%s: %s%s" % (a["name"], mod_type(a["type"]), " (mut)" if a["mut"] else "") for a in hook["args"]))


def hook_signature(hook):
    return hook_call(hook) + (" - can be cancelled" if hook["cancel"] else "")


def read_stable():
    entries = {"fn": [], "hook": []}
    if os.path.isfile(STABLE):
        for line in open(STABLE, encoding="utf-8").read().split("\n"):
            kind, _, signature = line.partition(" ")
            if kind in entries and signature:
                entries[kind].append(signature)
    return entries


def stabilise(api):
    """Numbers the promised functions and hooks by api/stable.txt and puts what is new after them, so a WebAssembly mod built
    against an earlier lce.h calls the same function by the same number; a promised one that is gone or changed is an error."""
    stable = read_stable()
    for kind, items, signature in (("fn", "functions", fn_signature), ("hook", "hooks", hook_signature)):
        by_signature = {}
        for item in api[items]:
            by_signature.setdefault(signature(item), item)
        missing = [text for text in stable[kind] if text not in by_signature]
        if missing:
            raise ApiError("API version %d promises %s %s, which is gone or changed; restore it, or start a new API version"
                           % (api["version"], kind, missing[0]) + (" (and %d more)" % (len(missing) - 1) if len(missing) > 1 else ""))
        promised = [by_signature[text] for text in stable[kind]]
        chosen = set(id(item) for item in promised)
        api[items] = promised + [item for item in api[items] if id(item) not in chosen]


def freeze(api):
    """api/stable.txt with what it already promises, in its order, and everything new after it."""
    stable = read_stable()
    lines = ["# The functions and hooks API version %d promises, in the order that numbers them for WebAssembly (LCE_FN_*, LCE_HOOK_*)." % api["version"],
             "# Written by `hookgen.py freeze`; a line is never removed or changed within an API version."]
    for kind, items, signature in (("fn", "functions", fn_signature), ("hook", "hooks", hook_signature)):
        known = set(stable[kind])
        lines += ["%s %s" % (kind, text) for text in stable[kind]]
        lines += ["%s %s" % (kind, signature(item)) for item in api[items] if signature(item) not in known]
    return "\n".join(lines) + "\n"


# what a virtual method's parameter becomes for the mod: (hook arguments as (name, type, expression), or None to drop it)
DRAWING_CONTEXTS = {"ModHandles::GuiInfo": "gui", "ModHandles::WorldInfo": "canvas"}


def convert_parameter(api, ctype, name):
    ctype = re.sub(r"\s+", " ", ctype.strip())
    base = ctype.replace("const ", "").replace("&", "").strip()
    if base in ("bool", "int", "float"):
        return [(name, base, name)], None
    if base == "std::wstring":
        return [(name, "string", name)], None
    if base == "Level *" or base == "Level*":
        return [(name, "level", name)], None
    if base in ("LevelSource *", "LevelSource*"):
        # a source that is not a level reaches the mod as an empty handle, so the hook still fires and a woven body can carry it
        return [(name, "level", "dynamic_cast<Level*>(%s)" % name)], None
    if base in ("BlockPos", "BlockPos &"):
        prefix = "" if name == "pos" else name + "_"
        return [(prefix + axis, "int", "%s.%s" % (name, axis)) for axis in "xyz"], None
    if base in ("BlockState *", "BlockState*"):
        return [(name, "int", "(%s != NULL ? const_cast<Block*>(%s->getBlock())->convertBlockStateToLegacyData(%s) : 0)" % (name, name, name))], None
    m = re.match(r"^boost::shared_ptr<(\w+)>$", base)
    if m and m.group(1) in ("Entity", "Player", "LivingEntity", "Mob", "ServerPlayer"):
        return [(name, "entity", name)], None
    m = re.match(r"^(\w+)\s*\*$", base)
    if m and m.group(1) in ENTITY_FAMILY:
        return [(name, "entity", "ModHooks::SharedEntity(%s)" % name)], None  # a plain pointer, given to the mod as the level's handle
    if base in ("Block *", "Block*"):
        return [(name, "block", name)], None
    if base in ("Direction *", "Direction*"):
        return [(name, "int", "(%s != NULL ? %s->get3DDataValue() : -1)" % (name, name))], None
    if base == "InteractionHand::EInteractionHand":
        return [(name, "int", "static_cast<int>(%s)" % name)], None
    if base in DRAWING_CONTEXTS:
        return [(name, DRAWING_CONTEXTS[base], name)], None  # a context the game builds to draw in, handed to the mod as its drawing handle
    if base in ("Random *", "Random*", "void *", "void*"):
        return [], None  # opaque to a mod
    kind = wire_type(ctype)
    if kind is not None and kind[0] == "smart":
        handle = ensure_handle(api, kind[1])
        if "::" in kind[1]:
            return None, None
        return [(name, handle, "const_cast<%s*>(%s.get())" % (kind[1], name))], None
    if kind is not None and kind[0] == "scalarptr":
        mod = {"double": "float"}.get(kind[1], kind[1])
        return [(name, mod, "(%s != NULL ? *%s : %s)" % (name, name, {"bool": "false", "int": "0", "float": "0.0f", "double": "0.0"}[kind[1]]))], None
    if kind is not None and kind[0] == "bytes":
        return [(name, "bytes", "std::string(reinterpret_cast<const char*>(%s.data), %s.length * sizeof(%s))" % (name, name, kind[1]))], None
    if kind is not None and kind[0] == "scalar":
        return [(name, kind[1], "static_cast<%s>(%s)" % ({"int": "int", "float": "float", "bool": "bool"}[kind[1]], name))], None
    if kind is not None and kind[0] == "enum":
        return [(name, "int", "static_cast<int>(%s)" % name)], None
    if kind is not None and kind[0] == "handle" and "::" in kind[1]:
        return None, None  # a nested class is not forward declarable, so hooks leave it to the calls
    if kind is not None and kind[0] == "handle":
        handle = ensure_handle(api, kind[1])
        return [(name, handle, "const_cast<%s*>(%s%s)" % (kind[1], "&" if kind[2] == "&" else "", name))], None
    return None, None


def ensure_handle(api, cls):
    """The handle for a game class, made when no signature has needed it before (a nested class is named Owner_Name for the mod)."""
    for h in api["handles"]:
        if h["cpp"] == cls + "*":
            return h["name"]
    name = cls.replace("::", "_")
    api["handles"].append({"name": name, "doc": "A %s in the game." % cls, "cpp": cls + "*", "native": cls + "*", "auto": True})
    return name


RESULTS = {"void": None, "bool": "bool", "int": "int", "float": "float", "std::wstring": "string",
           "const BlockState*": "state", "const BlockState *": "state", "Item*": "item", "Item *": "item"}


def param_types(params):
    # the types of a parameter list without the names or default values, to tell overloads apart
    types = []
    for param in split_top(params):
        param = param.split("=")[0].strip()
        if param in ("", "void"):
            continue
        m = re.match(r"^(.*?)(\w+)$", param)
        if m and m.group(2) in TYPE_WORDS:
            m = None  # an unnamed parameter ends in its type, not a name
        text = m.group(1) if m and m.group(1).strip() else param
        types.append(re.sub(r"\s+", "", text.replace("const", "")))
    return tuple(types)


def split_top(text):
    parts, depth, current = [], 0, ""
    for ch in text:
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(current)
            current = ""
        else:
            current += ch
    if current.strip():
        parts.append(current)
    return [p.strip() for p in parts]


def class_methods(cls):
    # the most derived header that declares a signature wins, and remembers which class declared it
    found = {}
    for header in cls["headers"]:
        owner = os.path.splitext(os.path.basename(header))[0]
        with open(os.path.join(ROOT, "source", header), encoding="utf-8") as handle:
            text = re.sub(r"//[^\n]*", "", strip_debug(handle.read()))
        pattern = re.compile(r"virtual\s+([^;{}()~]+?)\s+(\w+)\s*\(([^;{}]*?)\)\s*(const)?\s*;", re.S)
        for match in pattern.finditer(text):
            params = split_top(match.group(3))
            types = tuple(re.sub(r"\s+", " ", (re.match(r"^(.*?)(\w+)$", p.split("=")[0].strip()) or re.match(r"^(.*)()$", p)).group(1)).strip() for p in params)
            nested = set(re.findall(r"^[ \t]+enum\s+(?:class\s+)?(\w+)", text, re.M))
            found[(match.group(2), types, bool(match.group(4)))] = {
                "ret": re.sub(r"\s+", " ", match.group(1)).strip(), "name": match.group(2), "params": params,
                "const": bool(match.group(4)), "owner": owner, "nested_enums": nested}
    return list(found.values())


def hook_doc(cls, method, answers):
    if cls.get("woven"):
        return ("Called as %s::%s begins, in the game's own code; set result and cancel to return that instead of running it." if answers else
                "Called as %s::%s begins, in the game's own code; cancel to skip it.") % (cls["name"], method["name"])
    return ("Overrides %s::%s on a mod object; set result and cancel to answer instead of the game." if answers else
            "Overrides %s::%s on a mod object; cancel to skip the game's own work.") % (cls["name"], method["name"])


def class_hooks(cls, api, methods=None):
    hooks = []
    counts = {}
    cls["methods"] = []
    for method in (methods if methods is not None else class_methods(cls)):
        if method["name"] == "onShared":
            continue
        result = RESULTS.get(method["ret"], "?")
        returned = wire_type(method["ret"]) if result == "?" else None
        if returned is not None and returned[0] == "bytes" and cls.get("woven"):
            result = "bytes:" + returned[1]  # an array of numbers, packed
        elif returned is not None and returned[0] == "enum":
            result = "enum"  # the mod answers with the number, the game gets the enum back
        elif returned is not None and returned[0] == "handle" and returned[2] == "*":
            # the mod answers with a handle it was given (or one of its own making); the game gets that object's pointer
            result = "handle:" + returned[1]
            ensure_handle(api, returned[1])
        self_type = "entity" if cls.get("shared") else (cls["self"] if cls["self"] in CONVERTED or cls["self"] == "entity" else ensure_handle(api, cls["self"]))
        arguments = [] if method.get("static") else [("self", self_type, "@SELFPTR@")]
        preamble = []
        ok = result != "?"
        for index, param in enumerate(method["params"]):
            param = param.split("=")[0].strip()
            if param == "void" or not param:
                continue
            m = re.match(r"^(.*?)(\w+)$", param)
            if m and m.group(2) in TYPE_WORDS:
                m = None  # an unnamed parameter ends in its type, not a name
            if not m or not m.group(1).strip():
                ctype, name = param, "a%d" % index
            else:
                ctype, name = m.group(1), m.group(2)
            converted, pre = convert_parameter(api, ctype, name)
            if converted is None:
                if cls.get("woven"):
                    continue  # the weave runs inside the function, which keeps all its own parameters; the hook just does not see this one
                ok = False
                break
            # the hook's own names, self and result, are the mod's; a parameter of that name is told apart
            # a plain number the function takes by value (or by non-const reference) a hook may change before the function runs
            changeable = len(converted) == 1 and converted[0][1] in ("bool", "int", "float") and converted[0][2] == name and "const" not in ctype
            arguments += [((a[0] + "_arg") if a[0] in ("self", "result") else a[0], a[1], a[2]) + ((changeable,) if changeable else ()) for a in converted]
            if pre:
                preamble.append(pre)
        if not ok:
            cls["skipped"].append(method["name"])
            continue
        counts[method["name"]] = counts.get(method["name"], 0) + 1
        hook_name = "%s_%s" % (cls["name"], method["name"])
        if counts[method["name"]] > 1:
            hook_name += "_%d" % counts[method["name"]]
        names = [a[0] for a in arguments]
        if len(set(names)) != len(names):
            cls["skipped"].append(method["name"])
            counts[method["name"]] -= 1
            continue
        hook_args = [{"name": a[0], "type": a[1], "mut": len(a) > 3, "from": None} for a in arguments]
        if result:
            hook_args.append({"name": "result", "type": ensure_handle(api, result[7:]) if result.startswith("handle:") else ("bytes" if result.startswith("bytes:") else {"state": "int", "item": "string", "enum": "int"}.get(result, result)), "mut": True, "from": None})
        hooks.append({"name": hook_name, "args": hook_args, "cancel": True, "provider": False, "generated": True,
                      "doc": hook_doc(cls, method, bool(result)),
                      "targets": []})
        cls["methods"].append({"method": method, "hook": hook_name, "arguments": arguments, "preamble": preamble, "result": result})
    return hooks


def gen_class(api, cls):
    template = "Mod" + cls["name"]
    out = ["// Generated from %s by tools/hooks/hookgen.py. Do not edit." % ", ".join(cls["headers"]),
           "#ifndef GAME_MODCLASS_%s_GENERATED_HPP" % cls["name"].upper(), "#define GAME_MODCLASS_%s_GENERATED_HPP" % cls["name"].upper(), "",
           "#include <stddef.h>", "", '#include "game/core/ModEvents_generated.hpp"', '#include "game/core/ModHooks.hpp"', '#include "game/core/ModItemNames.hpp"', '#include "game/world/Level.hpp"', '#include "game/world/block/Block.hpp"', '#include "game/world/block/BlockState.hpp"', '#include "game/world/generation/Player.hpp"', '#include "game/entity/mob/LivingEntity.hpp"',
           ] + ['#include "%s"' % h for h in cls["headers"]] + [""] + [
           "// every virtual method of %s a mod can answer, on top of any game class derived from it" % cls["name"],
           "template <class Base>", "class %s : public Base" % template, "{", "public:", "    using Base::Base;", ""]
    if cls["shared"]:
        # the game wraps an entity in a shared pointer after building it, and a mod can only be handed it from then on
        out += ["    virtual void onShared()", "    {", "        mLive = true;", "        Base::onShared();", "    }", ""]
    for entry in cls["methods"]:
        method, arguments = entry["method"], entry["arguments"]
        params = []
        call = []
        for index, param in enumerate(method["params"]):
            param = param.split("=")[0].strip()
            if param == "void" or not param:
                continue
            m = re.match(r"^(.*?)(\w+)$", param)
            if m and m.group(2) in TYPE_WORDS:
                m = None  # an unnamed parameter ends in its type, not a name
            if not m or not m.group(1).strip():
                ctype, name = param, "a%d" % index
            else:
                ctype, name = m.group(1).strip(), m.group(2)
            for enum in method.get("nested_enums", ()):
                # an enum declared inside the class needs the class's name outside it
                ctype = re.sub(r"(?<![\w:])%s\b" % re.escape(enum), "%s::%s" % (method["owner"], enum), ctype)
            params.append("%s %s" % (ctype, name))
            call.append(name)
        base_call = "base_%s(%s)" % (entry["hook"], ", ".join(call + ["0"]))
        # a derived class can hide the overloads it does not declare, so fall back to the root class's own
        helper_params = ", ".join(params + ["int"])
        helper_call = ", ".join(call)
        out.append("    template <class B>")
        qualifier = " const" if method["const"] else ""
        selfcast = "static_cast<const B*>(this)" if method["const"] else "static_cast<B*>(this)"
        out.append("    auto base_%s(%s)%s -> decltype(%s->B::%s(%s))" % (entry["hook"], helper_params, qualifier, selfcast, method["name"], helper_call))
        out.append("    {")
        out.append("        %s%s->B::%s(%s);" % ("" if method["ret"] == "void" else "return ", selfcast, method["name"], helper_call))
        out.append("    }")
        out.append("    template <class B>")
        out.append("    %s base_%s(%s)%s" % (method["ret"], entry["hook"], ", ".join(params + ["long"]), qualifier))
        out.append("    {")
        out.append("        %sstatic_cast<%s%s*>(this)->%s::%s(%s);" % ("" if method["ret"] == "void" else "return ", "const " if method["const"] else "", method["owner"], method["owner"], method["name"], helper_call))
        out.append("    }")
        out.append("")
        base_call = base_call.replace("base_%s(" % entry["hook"], "base_%s<Base>(" % entry["hook"], 1)
        const = " const" if method["const"] else ""
        selfptr = "this->shared_from_this()" if cls["shared"] else "static_cast<%s*>(const_cast<%s*>(this))" % (cls["name"], template + "<Base>")
        out.append("    virtual %s %s(%s)%s" % (method["ret"], method["name"], ", ".join(params), const))
        out.append("    {")
        out.append("        if (%sModHooks::IsActive(kModHook_%s))" % ("mLive && " if cls["shared"] else "", entry["hook"]))
        out.append("        {")
        for pre in entry["preamble"]:
            out.append("            " + pre.replace("@BASE@", base_call))
        if entry["result"]:
            default = {"bool": "false", "int": "0", "float": "0.0f", "string": "std::wstring()", "state": "0", "item": "std::wstring()", "enum": "0"}.get(entry["result"], "NULL")
            local = ("%s*" % entry["result"][7:]) if entry["result"].startswith("handle:") else {"state": "int", "item": "std::wstring", "enum": "int"}.get(entry["result"], method["ret"])
            out.append("            %s result = %s;" % (local, default))
        expressions = [a[2].replace("@SELFPTR@", selfptr).replace("@SELF@", template + "<Base>") for a in arguments]
        if entry["result"]:
            expressions.append("result")
        event = "ModEvents::" + pascal(entry["hook"])
        if entry["result"]:
            out.append("            if (!%s(%s))" % (event, ", ".join(expressions)))
            out.append("                return %s;" % {"state": "this->getBlockState(result)", "item": "ModItemNames::Find(result)", "enum": "static_cast<%s>(result)" % method["ret"]}.get(entry["result"], "result"))
        else:
            out.append("            if (!%s(%s))" % (event, ", ".join(expressions)))
            out.append("                return;")
        out.append("        }")
        out.append("        %s%s;" % ("" if method["ret"] == "void" else "return ", base_call))
        out.append("    }")
        out.append("")
    if cls["skipped"]:
        out.append("    // not answerable yet, a parameter or result type has no conversion: %s" % ", ".join(sorted(set(cls["skipped"]))))
    if cls["shared"]:
        out += ["", "private:", "    bool mLive = false;"]
    out += ["};", "", "#endif", ""]
    return "\n".join(out)


# ---- the game's own classes as mod-callable objects ------------------------------------------
# `expose Class from header` makes every public method of the class a function on a handle for
# it. A pointer to any other class in a signature becomes a handle of that class, so the set of
# types grows with the signatures and nothing has to be listed.

def class_body(text, name):
    match = re.search(r"\b(class|struct)\s+%s\b[^;{]*\{" % re.escape(name), text)
    if not match:
        return None
    start, depth, i = match.end(), 1, match.end()
    while i < len(text) and depth:
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
    return match.group(1), text[start:i - 1]


def public_statements(kind, body):
    access = "public" if kind == "struct" else "private"
    statements, depth, current = [], 0, ""
    for line in body.split("\n"):
        label = re.match(r"^\s*(public|protected|private)\s*:\s*$", line)
        if label and depth == 0:
            access = label.group(1)
            continue
        for ch in line + "\n":
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    if access == "public" and current.strip():
                        statements.append(current.strip() + " {}")
                    current = ""
                    continue
            if depth == 0:
                if ch == ";":
                    if access == "public" and current.strip():
                        statements.append(current.strip())
                    current = ""
                else:
                    current += ch
    return statements


SCALAR_ARGS = {"bool": "Bool", "int": "Int", "float": "Float", "double": "Float"}
# classes whose objects a mod holds as the one entity handle, which owns what it points at
ENTITY_FAMILY = ("Entity", "Player", "LivingEntity", "Mob", "ServerPlayer", "Animal", "Monster", "PathfinderMob", "AgableMob")
# every other whole number type travels as an int
TYPE_WORDS = ("long", "int", "unsigned", "short", "char", "float", "double", "bool", "void", "const", "size_t")
INTEGERS = ("unsigned", "unsigned int", "short", "unsigned short", "long", "unsigned long", "long long", "unsigned long long",
            "unsigned char", "size_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t", "int8_t", "int16_t", "int32_t", "int64_t")
LUA_KEYWORDS = ("and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto", "if", "in", "local", "nil", "not", "or",
                "repeat", "return", "then", "true", "until", "while")
POINTER = re.compile(r"^(?:const\s+)?(\w+)\s*(\*|&)$")


_GRAPH = None


def class_graph():
    """class name -> (header, [public base class names]) for the classes declared at namespace scope."""
    global _GRAPH
    if _GRAPH is None:
        _GRAPH = {}
        pattern = re.compile(r"^(?:class|struct)\s+(\w+)\s*(?::\s*([^{;]+?))?\s*\{", re.M)
        for base, _, files in source_walk():
            for filename in sorted(files):
                if filename.endswith(".hpp"):
                    path = os.path.join(base, filename)
                    with open(path, encoding="utf-8", errors="replace") as handle:
                        text = handle.read()
                    for match in pattern.finditer(text):
                        bases = []
                        for part in split_top(match.group(2) or ""):
                            words = part.replace("virtual", " ").split()
                            if not words or words[0] in ("private", "protected") or "<" in part:
                                continue
                            bases.append(words[-1])
                        _GRAPH.setdefault(match.group(1), (os.path.relpath(path, os.path.join(ROOT, "source")), bases))
    return _GRAPH


def handle_bases(api):
    """(derived handle, base handle) pairs, nearest base first, for every handle whose class derives from another handle's."""
    graph = class_graph()
    by_class = {h["cpp"].rstrip("*").strip(): h["name"] for h in api["handles"] if h["cpp"].endswith("*")}
    pairs = []
    for cls, name in by_class.items():
        seen, queue = set(), list(graph.get(cls, (None, []))[1])
        while queue:
            base = queue.pop(0)
            if base in seen:
                continue
            seen.add(base)
            if base in by_class and base != cls:
                pairs.append((name, by_class[base], cls, base))
            queue += graph.get(base, (None, []))[1]
    # an entity handle is accepted where a call takes one of the entity classes by raw pointer: the call finds the object behind it
    for cls, name in by_class.items():
        if cls in ENTITY_FAMILY:
            pairs.append(("entity", name, "", cls))
    # an owned object that can be cast to another class is accepted where the other is wanted
    for source, target in api.get("shared_casts", []):
        if source in by_class and target in by_class:
            pairs.append((by_class[source], by_class[target], "", target))
    return pairs


_TOP_LEVEL = None
_TOP_RAW = set()
_ENUMS = None


def enum_names():
    # an enum travels as its number
    global _ENUMS
    if _ENUMS is None:
        _ENUMS = set()
        pattern = re.compile(r"\benum\s+(?:class\s+)?(\w+)")
        for base, _, files in source_walk():
            for filename in files:
                if filename.endswith(".hpp"):
                    with open(os.path.join(base, filename), encoding="utf-8", errors="replace") as handle:
                        _ENUMS.update(pattern.findall(handle.read()))
    return _ENUMS


def top_level_classes():
    # only a class declared at namespace scope can be forward declared for a handle; nested ones cannot
    global _TOP_LEVEL
    if _TOP_LEVEL is None:
        _TOP_LEVEL = set()
        pattern = re.compile(r"^(?:class|struct)\s+(\w+)\s*(?:[:{]|$)", re.M)
        nested_pattern = re.compile(r"^[ \t]+(?:class|struct)\s+(\w+)\s*(?:[:{]|$)", re.M)
        nested = set()
        for base, _, files in source_walk():
            for filename in files:
                if filename.endswith(".hpp"):
                    with open(os.path.join(base, filename), encoding="utf-8", errors="replace") as handle:
                        text = handle.read()
                        _TOP_LEVEL.update(pattern.findall(text))
                        nested.update(nested_pattern.findall(text))
        # a name that is also a nested class somewhere cannot be told apart from it here
        _TOP_RAW.update(_TOP_LEVEL)
        _TOP_LEVEL -= nested
    return _TOP_LEVEL


SHARED_HANDLES = {}
# handles that are small structs held by value (a frame's drawing context): their methods are called on the value
VALUE_HANDLES = {}


def wire_type(ctype, shared=False):
    """What a C++ type is to a mod: ('scalar', mod type), ('string',), ('pos',), ('handle', class) or None."""
    ctype = re.sub(r"\s+", " ", ctype.strip()).replace(" *", "*").replace(" &", "&")
    plain = ctype.replace("const ", "").replace("&", "").strip()
    if plain in ("bool", "int", "float", "double") or plain in INTEGERS:
        if "&" in ctype and "const" not in ctype:
            return None
        return ("scalar", "int" if plain in INTEGERS else {"double": "float"}.get(plain, plain), plain)
    if plain in ("std::wstring",):
        return ("string",)
    if plain in ("std::vector<boost::shared_ptr<Entity> >", "std::vector<boost::shared_ptr<Entity> >*"):
        return ("entityids", "*" in plain)  # a list of entities, which a mod gets as their ids, one on each line; level:getEntity(id) makes a handle of one
    if plain in ("std::set<std::wstring>*", "std::set<std::wstring>", "std::vector<std::wstring>", "std::vector<std::wstring>*"):
        return ("strings", plain.rstrip("*"), "*" in plain)  # a list of text, which a mod gets as the lines of one string
    if plain == "std::string" and "*" not in ctype and ("&" not in ctype or "const" in ctype):
        return ("rawstring",)  # the bytes of a Lua string as they are, a file's contents for one
    m = re.match(r"^arrayWithLength<([\w ]+)>$", plain)
    if m and not ("&" in ctype and "const" not in ctype):
        element = {"char": "unsigned char", "uint8_t": "unsigned char"}.get(m.group(1).strip(), m.group(1).strip())
        # numbers in an array travel packed as the bytes of the array (string.pack and string.unpack in Lua)
        if element in ("unsigned char", "short", "unsigned short", "int", "unsigned int", "float", "double", "long long", "unsigned long long", "int16_t", "uint16_t", "int32_t", "uint32_t", "int64_t", "uint64_t"):
            return ("bytes", element)
    if plain == "BlockPos":
        return ("pos",)
    if "&" not in ctype and "*" not in ctype and plain.split("::")[-1] in enum_names():
        return ("enum", plain)
    m = re.match(r"^boost::shared_ptr<(\w+)>$", plain)
    if shared and m and m.group(1) in SHARED_HANDLES and "*" not in ctype:
        return ("shared", m.group(1), SHARED_HANDLES[m.group(1)])  # an object a mod owns, by its handle
    if m and m.group(1) in ENTITY_FAMILY:
        return ("entity", m.group(1))
    m = re.match(r"^(NonNullList<not_null_ptr<(\w+)> >|std::vector<(?:boost::shared_ptr|not_null_ptr)<(\w+)> >)$", plain)
    if m and "*" not in ctype and (m.group(2) or m.group(3)) in top_level_classes() and (m.group(2) or m.group(3)) not in ENTITY_FAMILY:
        return ("handlelist", m.group(2) or m.group(3), m.group(1))  # objects the game owns, which a mod gets as a list of their handles
    m = re.match(r"^(boost::shared_ptr|not_null_ptr)<(\w+)>$", plain)
    if m and (m.group(2) in top_level_classes() or m.group(2) in nested_classes()):
        if shared and "*" not in ctype and "<const" not in ctype:
            return ("sharedobj", nested_classes().get(m.group(2), m.group(2)), m.group(1))  # an owned object a call takes back from its handle
        return ("smart", nested_classes().get(m.group(2), m.group(2)))
    m = re.match(r"^(bool|int|float|double)\s*\*$", plain) if "const" in ctype or True else None
    if m and "*" in ctype:
        return ("scalarptr", m.group(1))
    m = POINTER.match(ctype)
    if m and m.group(1) in top_level_classes():
        return ("handle", m.group(1), m.group(2))
    if m and m.group(1) in nested_classes():
        return ("handle", nested_classes()[m.group(1)], m.group(2))
    return None


_DEFINED = None


_POD = None


def pod_structs():
    """Structs of plain numbers, nested ones too (MenuRect: four ints; MenuTarget: ints, a MenuRect and a bool): name to the flat list of (type, path),
    such as ("int", "rect.x0"). A call takes one as its numbers in turn and a method that returns one is a function for each number."""
    global _POD
    if _POD is None:
        raw = {}
        pattern = re.compile(r"\bstruct\s+(\w+)\s*\{([^{}]*)\};")
        member = re.compile(r"^\s*(\w+)\s+(\w+(?:\s*,\s*\w+)*)\s*;\s*$")
        for base, _, files in source_walk():
            for filename in files:
                if not filename.endswith(".hpp"):
                    continue
                with open(os.path.join(base, filename), encoding="utf-8", errors="replace") as handle:
                    text = re.sub(r"//[^\n]*", "", handle.read())
                for match in pattern.finditer(text):
                    fields, ok = [], True
                    for line in [l for l in match.group(2).split("\n") if l.strip()]:
                        m = member.match(line)
                        if not m:
                            ok = False
                            break
                        fields += [(m.group(1), name.strip()) for name in m.group(2).split(",")]
                    if ok and fields:
                        raw[match.group(1)] = fields

        def flatten(name, seen):
            flat = []
            for kind, member_name in raw[name]:
                if kind in ("int", "float", "double", "bool"):
                    flat.append((kind, member_name))
                elif kind in raw and kind not in seen:
                    inner = flatten(kind, seen | {kind})
                    if inner is None:
                        return None
                    flat += [(t, member_name + "." + path) for t, path in inner]
                else:
                    return None
            return flat

        _POD = {}
        for name in raw:
            flat = flatten(name, {name})
            if flat:
                _POD[name] = flat
    return _POD


def source_walk():
    """The folders whose classes a mod can reach: the game and the PC platform."""
    return [entry for folder in ("game", os.path.join("platform", "pc")) for entry in os.walk(os.path.join(ROOT, "source", folder))]


def defined_methods():
    # a header can declare what no source file defines; calling that would not link
    global _DEFINED
    if _DEFINED is None:
        _DEFINED = set()
        pattern = re.compile(r"\b(\w+)::(\w+)\s*\(")
        for base, _, files in source_walk():
            for filename in files:
                if filename.endswith(".cpp"):
                    with open(os.path.join(base, filename), encoding="utf-8", errors="replace") as handle:
                        text = handle.read()
                        for match in pattern.finditer(text):
                            depth, end = 1, match.end()
                            while end < len(text) and depth:
                                depth += {"(": 1, ")": -1}.get(text[end], 0)
                                end += 1
                            _DEFINED.add((match.group(1), match.group(2), param_types(text[match.end():end - 1])))
    return _DEFINED


_NESTED = None


def nested_classes():
    """name -> Owner::name for classes declared inside another class, when the name is declared once in the whole game and is no
    top-level class, so that it means one thing."""
    global _NESTED
    if _NESTED is None:
        found, counts = {}, {}
        for base, _, files in os.walk(os.path.join(ROOT, "source", "game")):
            for filename in files:
                if not filename.endswith(".hpp"):
                    continue
                owner = None
                with open(os.path.join(base, filename), encoding="utf-8", errors="replace") as handle:
                    for line in handle:
                        top = re.match(r"^(?:class|struct)\s+(\w+)\b", line)
                        if top:
                            owner = top.group(1)
                        nested = re.match(r"^[ \t]+(?:class|struct)\s+(\w+)\s*(?::[^;{]*)?\{?\s*$", line)
                        if nested and owner:
                            counts[nested.group(1)] = counts.get(nested.group(1), 0) + 1
                            found[nested.group(1)] = "%s::%s" % (owner, nested.group(1))
        _NESTED = {name: qualified for name, qualified in found.items() if counts[name] == 1 and name not in (top_level_classes(), _TOP_RAW)[1]}
    return _NESTED


_ENUM_OWNERS = None


def enum_owners():
    """enum name -> the class that declares it, for enums nested in a class (a base class's enum is used by its subclasses unqualified)."""
    global _ENUM_OWNERS
    if _ENUM_OWNERS is None:
        _ENUM_OWNERS = {}
        for base, _, files in os.walk(os.path.join(ROOT, "source", "game")):
            for filename in files:
                if not filename.endswith(".hpp"):
                    continue
                owner = None
                with open(os.path.join(base, filename), encoding="utf-8", errors="replace") as handle:
                    for line in handle:
                        top = re.match(r"^(?:class|struct)\s+(\w+)\b", line)
                        if top:
                            owner = top.group(1)
                        nested = re.match(r"^\s+enum\s+(?:class\s+)?(\w+)", line)
                        if nested and owner:
                            _ENUM_OWNERS.setdefault(nested.group(1), owner)
    return _ENUM_OWNERS


def qualify_enum(spec, cls, name):
    # an enum declared inside a class needs the class in front of it outside the class
    if "::" in name:
        return name
    owner = enum_owners().get(name)
    return "%s::%s" % (owner, name) if owner else name


def namespace_body(text, name):
    match = re.search(r"\bnamespace\s+%s\s*\{" % re.escape(name), text)
    if not match:
        return None
    start, depth, i = match.end(), 1, match.end()
    while i < len(text) and depth:
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
    return "struct", text[start:i - 1]


def is_namespace(spec):
    """True when the exposed name is a namespace of free functions, not a class."""
    for header in spec["headers"]:
        with open(os.path.join(ROOT, "source", header), encoding="utf-8") as handle:
            text = re.sub(r"//[^\n]*", "", strip_debug(handle.read()))
        if class_body(text, spec["class"]) or class_body(text, os.path.splitext(os.path.basename(header))[0]):
            return False
        if namespace_body(text, spec["class"]):
            return True
    return False


def header_comments(text):
    """The description of each declaration: the `//` lines right above it, joined, keyed by the name the declaration gives."""
    found, pending = {}, []
    for line in text.split("\n"):
        stripped = line.strip()
        if stripped.startswith("//"):
            pending.append(stripped[2:].strip())
            continue
        name = re.search(r"\b(\w+)\s*\(", stripped)
        if pending and name:
            found.setdefault(name.group(1), " ".join(pending))
        if stripped:
            pending = []
    return found


def expose_classes(api):
    handle_names = {h["cpp"].rstrip("*").strip(): h["name"] for h in api["handles"] if h["cpp"].endswith("*")}
    api["generated_fns"] = []
    seen_overloads = {}
    seen_signatures = set()
    for spec in api["exposed"]:
        cls = spec["class"]
        qualified = nested_classes().get(cls, cls)
        family = cls in ENTITY_FAMILY
        raw_base = api.get("raw_families", {}).get(cls)
        namespace = is_namespace(spec)
        spec["namespace"] = namespace
        module = (cls if namespace else SHARED_HANDLES.get(cls) or VALUE_HANDLES.get(cls) or ("entity" if family else handle_names.get(raw_base or qualified) or ensure_handle(api, raw_base or qualified)))
        spec["family"] = family
        if not family and not raw_base and not namespace and cls not in SHARED_HANDLES and cls not in VALUE_HANDLES:
            handle_names[qualified] = module
        spec["qualified"] = qualified
        spec["module"] = module
        spec["skipped"] = []
        statements = []
        for header in spec["headers"]:
            with open(os.path.join(ROOT, "source", header), encoding="utf-8") as handle:
                raw = strip_debug(handle.read())
            spec.setdefault("comments", {}).update(header_comments(raw))
            text = re.sub(r"//[^\n]*", "", raw)
            owner = os.path.splitext(os.path.basename(header))[0]
            found = class_body(text, cls) or class_body(text, owner) or (namespace_body(text, cls) if namespace else None)
            if found:
                statements += public_statements(*found)
                spec.setdefault("body", "")
                spec["body"] += found[1]
        for statement in statements:
            statement = re.sub(r"\s+", " ", statement)
            inline = statement.endswith("{}")
            statement = statement[:-2].strip() if inline else statement
            constant = re.match(r"^const\s+(unsigned long long|unsigned int|unsigned|long long|int|float|double|bool)\s+(\w+)\s*=", statement) if namespace else None
            if constant and ("(" not in statement.split("=")[0]):
                ctype, cname_const = constant.groups()
                kind_const = wire_type(ctype)
                fn_name_const = "get_" + cname_const
                if kind_const and kind_const[0] == "scalar" and (cls, fn_name_const, True) not in [(f["module"], f["name"], f["static"]) for f in api["functions"]]:
                    fn = {"static": True, "module": cls, "name": fn_name_const, "args": [], "result": kind_const[1],
                          "doc": "The game's %s::%s (a constant)." % (cls, cname_const),
                          "generated": {"cls": spec["qualified"], "family": False, "static": True, "ctor": False, "method": cname_const, "native": [],
                                        "ret": kind_const, "void": False, "axis": None, "static_const": True}}
                    api["functions"].append(fn)
                    api["generated_fns"].append(fn)
                continue
            # a public static data member that points at an object of the game (Blocks::stone) is read by a static function get_name
            static_field = re.match(r"^static\s+(?:const\s+)?(\w+)\s*\*\s*(\w+)$", statement) if "(" not in statement else None
            if static_field and static_field.group(1) in top_level_classes():
                field_class, field_name = static_field.groups()
                handle = handle_names.get(field_class) or ensure_handle(api, field_class)
                handle_names[field_class] = handle
                if ("%s" % cls, "get_" + field_name, True) not in [(f["module"], f["name"], f["static"]) for f in api["functions"]]:
                    fn = {"static": True, "module": cls, "name": "get_" + field_name, "args": [], "result": handle,
                          "doc": "The game's %s::%s (a static member)." % (cls, field_name),
                          "generated": {"cls": spec["qualified"], "family": False, "static": True, "ctor": False, "method": field_name, "native": [],
                                        "ret": ("handle", field_class, "*"), "void": False, "axis": None, "static_field": field_class}}
                    api["functions"].append(fn)
                    api["generated_fns"].append(fn)
                continue
            ctor = re.match(r"^(?:explicit\s+)?%s\s*\((.*)\)$" % re.escape(cls), statement)
            is_ctor = bool(ctor) and "~" not in statement
            if is_ctor:
                # a constructor is the static function `new` that makes one; a class with a pure virtual method cannot be made
                if re.search(r"=\s*0\s*;", spec.get("body", "")) or re.match(r"^\s*const\s+%s\s*&\s*\w*\s*$" % re.escape(cls), ctor.group(1)):
                    continue
                flags, ret, name, params, const = "static ", cls + "*", "new", ctor.group(1), None
                match = True
            else:
                match = re.match(r"^((?:(?:virtual|static|inline|explicit)\s+)*)(.+?)\s+(\w+)\s*\((.*)\)\s*(const)?\s*(?:override|= 0|= default|= delete)?$", statement)
                if not match or "operator" in statement or match.group(3) in (cls,) or "template" in statement or "~" in statement:
                    continue
                flags, ret, name, params, const = match.groups()
            static = "static" in flags or namespace
            if ret.strip() in ("", "virtual"):
                continue
            pure = bool(re.search(r"=\s*0\s*$", statement))
            if not namespace and not inline and not pure and (cls, cls if is_ctor else name, param_types(params)) not in defined_methods():
                spec["skipped"].append(name)
                continue
            pod_result = pod_structs().get(re.sub(r"\bconst\b|&|\s", "", ret)) if ret.strip() != "void" else None
            result = None if ret.strip() == "void" else (("podfield",) if pod_result else wire_type(ret))
            if result is not None and result[0] == "enum":
                result = ("enum", qualify_enum(spec, cls, result[1]))
            if result is not None and result[0] == "smart" and re.sub(r"\bconst\s+", "", ret.strip()).startswith(("boost::shared_ptr", "not_null_ptr")):
                result = ("sharedptr", result[1])  # an object the game owns, handed to the mod by its handle for the call
            if ret.strip() != "void" and (result is None or result[0] in ("smart", "scalarptr")):
                spec["skipped"].append(name)
                continue
            arguments, native, ok = ([] if static else [{"name": "self", "type": module, "mut": False, "from": None}]), [], True
            for index, param in enumerate([p.split("=")[0].strip() for p in split_top(params)]):
                if param in ("", "void"):
                    continue
                m = re.match(r"^(.*?)(\w+)$", param)
                if m and m.group(2) in TYPE_WORDS:
                    m = None  # an unnamed parameter ends in its type, not a name
                ctype, pname = (m.group(1), m.group(2)) if m and m.group(1).strip() else (param, "a%d" % index)
                # a parameter may not take the name of one of the generated function's own variables
                cname = pname + "_" if pname in ("result", "arguments", "self", "object") else pname
                pod_name = re.sub(r"\bconst\b|&|\s", "", ctype)
                if pod_name in pod_structs():
                    fields = pod_structs()[pod_name]
                    for field_type, field_name in fields:
                        arguments.append({"name": pname + "_" + field_name.replace(".", "_"), "type": "int" if field_type in ("int", "bool") else "float", "mut": False, "from": None})
                    native.append((("pod", pod_name, fields), cname))
                    continue
                kind = wire_type(ctype, True)
                if kind is not None and kind[0] == "entityids" and kind[1]:
                    # a list to fill that the game takes as NULL and then uses its own: not an argument of the call
                    native.append(("null", cname))
                    continue
                if kind is not None and kind[0] == "entityids":
                    ok = False  # a list a mod cannot make
                    break
                if pname == "owner" and "std::string" in ctype:
                    # the mod that made the call, which the game knows and the caller need not say
                    native.append(("owner", cname))
                    continue
                if kind is None or kind[0] in ("smart", "scalarptr", "strings", "handlelist"):
                    ok = False  # a call cannot make a shared pointer from a raw one, fill a scalar behind a pointer, or take a list of text or handles yet
                    break
                if kind[0] == "sharedobj":
                    handle = handle_names.get(kind[1]) or ensure_handle(api, kind[1])
                    handle_names[kind[1]] = handle
                    arguments.append({"name": pname, "type": handle, "mut": False, "from": None})
                    native.append((kind, cname))
                    continue
                if kind[0] == "shared":
                    arguments.append({"name": pname, "type": kind[2], "mut": False, "from": None})
                    native.append((kind, cname))
                    continue
                if kind[0] == "enum":
                    kind = ("enum", qualify_enum(spec, cls, kind[1]))
                if kind[0] == "pos":
                    for axis in "xyz":
                        arguments.append({"name": ("" if pname == "pos" else pname + "_") + axis, "type": "int", "mut": False, "from": None})
                    native.append(("pos", cname))
                    continue
                if kind[0] == "scalar":
                    mod = kind[1]
                elif kind[0] in ("string", "rawstring"):
                    mod = "string"
                elif kind[0] == "enum":
                    mod = "int"
                elif kind[0] == "bytes":
                    mod = "string"
                elif kind[0] == "entity":
                    mod = "entity"
                else:
                    mod = handle_names.get(kind[1])
                    if mod is None:
                        mod = ensure_handle(api, kind[1])
                        handle_names[kind[1]] = mod
                arguments.append({"name": pname, "type": mod, "mut": False, "from": None})
                native.append((kind, cname))
            if not ok:
                spec["skipped"].append(name)
                continue
            # a Lua keyword cannot be a method name in a call
            lua_name = name + "_" if name in LUA_KEYWORDS else name
            signature = (module, name, param_types(params), static)
            if signature in seen_signatures:
                continue
            seen_signatures.add(signature)
            key = (module, name, static)
            seen_overloads[key] = seen_overloads.get(key, 0) + 1
            fn_name = lua_name if seen_overloads[key] == 1 else "%s_%d" % (lua_name, seen_overloads[key])
            fn_module = cls if static else module
            alias = api.get("aliases", {}).get((cls, name))
            if alias:
                fn_module, fn_name = alias
            if (fn_module, fn_name, static) in [(f["module"], f["name"], f["static"]) for f in api["functions"]]:
                continue
            result_type = None
            if result is not None:
                if result[0] == "pos":
                    result_type = "int"
                elif result[0] == "scalar":
                    result_type = result[1]
                elif result[0] in ("string", "rawstring", "strings", "entityids"):
                    result_type = "string"
                elif result[0] == "enum":
                    result_type = "int"
                elif result[0] == "bytes":
                    result_type = "string"
                elif result[0] == "entity":
                    result_type = "entity"
                elif result[0] == "podfield":
                    result_type = "int"  # set for each number below
                elif result[0] == "sharedptr":
                    result_type = SHARED_HANDLES.get(result[1]) or handle_names.get(result[1]) or ensure_handle(api, result[1])
                    handle_names[result[1]] = result_type
                elif result[0] == "handlelist":
                    element = SHARED_HANDLES.get(result[1]) or handle_names.get(result[1]) or ensure_handle(api, result[1])
                    handle_names[result[1]] = element
                    result_type = "list:" + element
                else:
                    mod = handle_names.get(result[1])
                    if mod is None:
                        mod = ensure_handle(api, result[1])
                        handle_names[result[1]] = mod
                    result_type = mod
            # a position is three numbers, so a method that returns one is three functions, name_x, name_y and name_z
            suffixes = ("_x", "_y", "_z") if result is not None and result[0] == "pos" else ("",)
            if pod_result:
                # a struct of numbers is a function for each number: name_member, name_inner_member
                suffixes = tuple("_" + path.replace(".", "_") for _, path in pod_result)
            for suffix in suffixes:
                if pod_result:
                    field_type, field_path = pod_result[suffixes.index(suffix)]
                    result = ("podfield", field_type)
                    result_type = "int" if field_type in ("int", "bool") else "float"
                fn = {"static": static, "module": fn_module, "name": fn_name + suffix, "args": arguments, "result": result_type,
                      "doc": spec["comments"].get(name) or "The game's %s::%s%s." % (cls, name, (" (the %s of the position)" % suffix[1]) if suffix and not pod_result else (" (its %s)" % suffix[1:].replace("_", ".")) if suffix else ""),
                      "generated": {"cls": spec["qualified"], "family": family, "raw_family": bool(raw_base) and not static, "static": static, "ctor": is_ctor, "method": name, "native": native,
                                    "ret": result, "void": ret.strip() == "void", "axis": (field_path if pod_result else suffix[1:] or None)}}
                api["functions"].append(fn)
                api["generated_fns"].append(fn)
    # a derived class's methods are put on the handle of its base; one the base's handle already reaches through a shared cast (a chest is
    # a Container) is the cast's, so it works for every object of the base and not only for the derived class
    cast_modules = {}
    for source, target in api.get("shared_casts", []):
        from_module = handle_names.get(nested_classes().get(source, source))
        to_module = handle_names.get(nested_classes().get(target, target))
        if from_module and to_module:
            cast_modules.setdefault(from_module, set()).add(to_module)
    reached = {(fn["module"], fn["name"]) for fn in api["generated_fns"]}
    shadowing = [fn for fn in api["generated_fns"]
                 if fn["generated"].get("raw_family") and any((target, fn["name"]) in reached for target in cast_modules.get(fn["module"], ()))]
    dropped = set(id(fn) for fn in shadowing)
    api["functions"] = [fn for fn in api["functions"] if id(fn) not in dropped]
    api["generated_fns"] = [fn for fn in api["generated_fns"] if id(fn) not in dropped]
    # a class whose objects a mod keeps is freed with its static function free
    for cls in sorted(api.get("persistent", set())):
        handle = handle_names.get(cls) or ensure_handle(api, cls)
        fn = {"static": True, "module": cls, "name": "free", "args": [{"name": "object", "type": handle, "mut": False, "from": None}], "result": None,
              "doc": "Free a %s you made now; it is freed anyway when your mod is unloaded." % cls,
              "generated": {"cls": cls, "family": False, "static": True, "ctor": False, "method": "free", "native": [], "ret": None, "void": True, "axis": None,
                            "persistent_free": True}}
        api["functions"].append(fn)
        api["generated_fns"].append(fn)
    # a global object of the game: the class's static function `instance` hands it out
    for single in api.get("singletons", []):
        handle = ensure_handle(api, single["class"])
        fn = {"static": True, "module": single["class"], "name": "instance", "args": [], "result": handle,
              "doc": "The game's one %s (the global %s)." % (single["class"], single["variable"]),
              "generated": {"cls": single["class"], "family": False, "static": True, "method": "instance", "native": [], "ret": None, "void": False,
                            "singleton": single}}
        api["functions"].append(fn)
        api["generated_fns"].append(fn)
    return


def gen_exposed(api):
    """Implementations of the functions made by `expose`."""
    out = ["// Generated from the game's headers by tools/hooks/hookgen.py. Do not edit.", '#include "game/core/ModApiFunctions_generated.hpp"', "",
           '#include <boost/lexical_cast.hpp>', '#include "game/core/ModText.hpp"', '#include "mod/ModApiIds_generated.hpp"', '#include "game/world/block/BlockPos.hpp"']
    headers = [single["header"] for single in api.get("singletons", [])]
    for spec in api["exposed"]:
        headers += spec["headers"]
    out += ['#include "%s"' % h for h in sorted(set(headers))]
    out += ['#include <string.h>', '#include <boost/pointer_cast.hpp>', '#include "game/entity/mob/Entity.hpp"', '#include "game/entity/mob/LivingEntity.hpp"', '#include "game/world/generation/Player.hpp"']
    graph = class_graph()
    needed = set()
    pairs = handle_bases(api)
    for _, _, derived, _ in pairs:
        if derived in graph:
            needed.add(graph[derived][0])
    out += ['#include "%s"' % h for h in sorted(needed)]
    out += [""]
    out += ["namespace", "{", "template <class D, class B> void* Upcast(void* pointer)", "{", "    return static_cast<B*>(static_cast<D*>(pointer));", "}", "",
            "template <class From, class To> boost::shared_ptr<void> CastShared(const boost::shared_ptr<void>& owner)", "{",
            "    return boost::shared_ptr<void>(boost::dynamic_pointer_cast<To>(boost::static_pointer_cast<From>(owner)));", "}", "",
            "struct RegisterUpcasts", "{", "    RegisterUpcasts()", "    {"]
    for source, target in api.get("shared_casts", []):
        out.append("        ModHandles::AddSharedCast(ModHandles::Key<%s>(), ModHandles::Key<%s>(), CastShared<%s, %s>);" % (source, target, source, target))
    for _, _, derived, base in pairs:
        if derived in graph and base in graph:
            out.append("        ModHandles::AddBase(ModHandles::Key<%s>(), ModHandles::Key<%s>(), Upcast<%s, %s>);" % (derived, base, derived, base))
    out += ["    }", "} sRegisterUpcasts;", "}", ""]
    for fn in api["generated_fns"]:
        g = fn["generated"]
        if g["static"]:
            out.append("bool ModApi_%s(const mod::Value* arguments, mod::Value* result)" % ident(fn))
        else:
            out.append("bool ModApi_%s(%s self, const mod::Value* arguments, mod::Value* result)" % (ident(fn), self_type(api, fn)))
        out.append("{")
        out.append("    (void)arguments;")
        out.append("    (void)result;")
        target = "self"
        if g["static"]:
            target = g["cls"]
        elif g.get("raw_family"):
            out.append("    %s* object = dynamic_cast<%s*>(self);" % (g["cls"], g["cls"]))
            out.append("    if (object == NULL)")
            out.append("        return false;")
            target = "object"
        elif g["family"]:
            out.append("    %s* object = dynamic_cast<%s*>(self.get());" % (g["cls"], g["cls"]))
            out.append("    if (object == NULL)")
            out.append("        return false;")
            target = "object"
        cursor, call = 0, []
        for kind, pname in g["native"]:
            if kind == "owner":
                out.append("    const std::string %s = ModHandles::Caller::Mod();" % pname)
                call.append(pname)
            elif kind == "null":
                call.append("NULL")
            elif isinstance(kind, tuple) and kind[0] == "pod":
                out.append("    %s %s;" % (kind[1], pname))
                for number, (field_type, field_name) in enumerate(kind[2]):
                    reader = {"int": "static_cast<int>(arguments[%d].Int())", "bool": "arguments[%d].Int() != 0", "float": "static_cast<float>(arguments[%d].Float())", "double": "arguments[%d].Float()"}[field_type]
                    out.append("    %s.%s = %s;" % (pname, field_name, reader % (cursor + number)))
                call.append(pname)
                cursor += len(kind[2])
            elif kind == "pos":
                out.append("    const BlockPos %s(static_cast<int>(arguments[%d].Int()), static_cast<int>(arguments[%d].Int()), static_cast<int>(arguments[%d].Int()));" % (pname, cursor, cursor + 1, cursor + 2))
                call.append(pname)
                cursor += 3
            elif kind[0] == "scalar":
                out.append("    const %s %s = static_cast<%s>(arguments[%d].%s());" % (kind[2], pname, kind[2], cursor, SCALAR_ARGS.get(kind[2], "Int")))
                call.append(pname)
                cursor += 1
            elif kind[0] == "bytes":
                # the callee may keep the array (setBiomes does), so it gets a copy of its own that is never freed here
                out.append("    const std::string %s_bytes = arguments[%d].String();" % (pname, cursor))
                out.append("    const unsigned int %s_count = static_cast<unsigned int>(%s_bytes.size() / sizeof(%s));" % (pname, pname, kind[1]))
                out.append("    %s* %s_data = new %s[%s_count + 1];" % (kind[1], pname, kind[1], pname))
                out.append("    memcpy(%s_data, %s_bytes.data(), %s_count * sizeof(%s));" % (pname, pname, pname, kind[1]))
                out.append("    arrayWithLength<%s> %s(%s_data, %s_count);" % (kind[1], pname, pname, pname))
                call.append(pname)
                cursor += 1
            elif kind[0] == "enum":
                out.append("    const %s %s = static_cast<%s>(arguments[%d].Int());" % (kind[1], pname, kind[1], cursor))
                call.append(pname)
                cursor += 1
            elif kind[0] == "sharedobj":
                out.append("    boost::shared_ptr<%s> %s_owner;" % (kind[1], pname))
                out.append("    if (!ModHandles::GetShared(arguments[%d].Id(), &%s_owner))" % (cursor, pname))
                out.append("        return false;")
                out.append("    %s<%s> %s(%s_owner);" % (kind[2], kind[1], pname, pname))
                call.append(pname)
                cursor += 1
            elif kind[0] == "shared":
                out.append("    boost::shared_ptr<%s> %s;" % (kind[1], pname))
                out.append("    if (!ModHandles::GetShared(arguments[%d].Id(), &%s))" % (cursor, pname))
                out.append("        return false;")
                call.append(pname)
                cursor += 1
            elif kind[0] == "rawstring":
                out.append("    const std::string %s = arguments[%d].String();" % (pname, cursor))
                call.append(pname)
                cursor += 1
            elif kind[0] == "string":
                out.append("    const std::wstring %s = ModText::FromUtf8(arguments[%d].String());" % (pname, cursor))
                call.append(pname)
                cursor += 1
            elif kind[0] == "entity":
                out.append("    boost::shared_ptr<Entity> %s_entity;" % pname)
                out.append("    if (arguments[%d].Id() != 0 && !ModHandles::Get(arguments[%d].Id(), &%s_entity))" % (cursor, cursor, pname))
                out.append("        return false;")
                out.append("    %s %s = boost::dynamic_pointer_cast<%s>(%s_entity);" % ("boost::shared_ptr<%s>" % kind[1], pname, kind[1], pname))
                call.append(pname)
                cursor += 1
            elif kind[1] in ENTITY_FAMILY and kind[2] == "*":
                # a mob, player or other entity by raw pointer: a handle of the class itself, or the entity handle every mob arrives as
                out.append("    %s* %s = NULL;" % (kind[1], pname))
                out.append("    boost::shared_ptr<Entity> %s_entity;" % pname)
                out.append("    if (arguments[%d].Kind() == mod::api::eHandle_entity)" % cursor)
                out.append("    {")
                out.append("        if (!ModHandles::Get(arguments[%d].Id(), &%s_entity))" % (cursor, pname))
                out.append("            return false;")
                out.append("        %s = dynamic_cast<%s*>(%s_entity.get());" % (pname, kind[1], pname))
                out.append("        if (%s == NULL)" % pname)
                out.append("            return false;")
                out.append("    }")
                out.append("    else if (!ModHandles::Get(arguments[%d].Id(), &%s))" % (cursor, pname))
                out.append("        return false;")
                call.append(pname)
                cursor += 1
            else:
                out.append("    %s* %s = NULL;" % (kind[1], pname))
                out.append("    if (!ModHandles::Get(arguments[%d].Id(), &%s))" % (cursor, pname))
                out.append("        return false;")
                call.append(("*" if kind[2] == "&" else "") + pname)
                cursor += 1
        if g.get("singleton"):
            single = g["singleton"]
            out.append("    *result = mod::Value::FromHandle(mod::api::eHandle_%s, ModHandles::Add(%s%s));" % (handle_names_of(api, single["class"]), "" if single.get("pointer") else "&", single["variable"]))
            out.append("    return true;")
            out.append("}")
            out.append("")
            continue
        if g.get("persistent_free"):
            out.append("    ModHandles::ReleasePersistent(arguments[0].Id());")
            out.append("    return true;")
            out.append("}")
            out.append("")
            continue
        if g.get("static_const"):
            ret = g["ret"]
            out.append("    *result = mod::Value::From%s(static_cast<%s>(%s::%s));" % ({"bool": "Bool", "int": "Int", "float": "Float"}[ret[1]], ret[2], g["cls"], g["method"]))
            out.append("    return true;")
            out.append("}")
            out.append("")
            continue
        if g.get("static_field"):
            out.append("    %s* modResult = const_cast<%s*>(%s::%s);" % (g["static_field"], g["static_field"], g["cls"], g["method"]))
            out.append("    if (modResult != NULL)")
            out.append("        *result = mod::Value::FromHandle(mod::api::eHandle_%s, ModHandles::Add(modResult));" % handle_names_of(api, g["static_field"]))
            out.append("    return true;")
            out.append("}")
            out.append("")
            continue
        if g.get("ctor") and g["cls"] in api.get("persistent", set()):
            # an object a mod keeps: its handle lasts until the mod frees it
            out.append("    const boost::shared_ptr<%s> modOwner(new %s(%s));" % (g["cls"], g["cls"], ", ".join(call)))
            out.append("    const uint32_t modId = ModHandles::AddPersistent(modOwner);")
            out.append("    if (modId != 0)")
            out.append("        *result = mod::Value::FromHandle(mod::api::eHandle_%s, modId);" % handle_names_of(api, g["cls"]))
            out.append("    return true;")
            out.append("}")
            out.append("")
            continue
        if g.get("ctor") and g["cls"] in api.get("owned", set()):
            # a class whose objects are held by shared pointer: what a mod makes is owned by the handle until a call takes it
            out.append("    const boost::shared_ptr<%s> modOwner(new %s(%s));" % (g["cls"], g["cls"], ", ".join(call)))
            out.append("    *result = mod::Value::FromHandle(mod::api::eHandle_%s, ModHandles::AddShared(modOwner));" % handle_names_of(api, g["cls"]))
            out.append("    return true;")
            out.append("}")
            out.append("")
            continue
        by_value = not g["static"] and not g["family"] and g["cls"].split("::")[-1] in VALUE_HANDLES
        invocation = "new %s(%s)" % (g["cls"], ", ".join(call)) if g.get("ctor") else "%s%s%s(%s)" % (target, "::" if g["static"] else ("." if by_value else "->"), g["method"], ", ".join(call))
        ret = g["ret"]
        if g["void"]:
            out.append("    %s;" % invocation)
        elif ret[0] == "scalar":
            out.append("    *result = mod::Value::From%s(%s);" % ({"bool": "Bool", "int": "Int", "float": "Float"}[ret[1]], invocation))
        elif ret[0] == "bytes":
            out.append("    const arrayWithLength<%s> modBytes = %s;" % (ret[1], invocation))
            out.append("    if (modBytes.data != NULL)")
            out.append("        *result = mod::Value::FromString(std::string(reinterpret_cast<const char*>(modBytes.data), modBytes.length * sizeof(%s)));" % ret[1])
        elif ret[0] == "pos":
            out.append("    *result = mod::Value::FromInt(static_cast<int>(%s.%s));" % (invocation, g["axis"]))
        elif ret[0] == "podfield":
            kind_name = "Float" if ret[1] in ("float", "double") else "Int"
            out.append("    *result = mod::Value::From%s(static_cast<%s>(%s.%s));" % (kind_name, "double" if kind_name == "Float" else "int", invocation, g["axis"]))
        elif ret[0] == "enum":
            out.append("    *result = mod::Value::FromInt(static_cast<int>(%s));" % invocation)
        elif ret[0] == "strings":
            out.append("    %s%s modList = %s;" % ("const " if False else "", ret[1] + ("*" if ret[2] else ""), invocation))
            out.append("    std::string modLines;")
            out.append("    for (%s::const_iterator it = modList%sbegin(); it != modList%send(); ++it)" % (ret[1], "->" if ret[2] else ".", "->" if ret[2] else "."))
            out.append("        modLines += (modLines.empty() ? \"\" : \"\\n\") + ModText::ToUtf8(*it);")
            out.append("    *result = mod::Value::FromString(modLines);")
        elif ret[0] == "entityids":
            if ret[1]:
                out.append("    const std::vector<boost::shared_ptr<Entity> >* modList = %s;" % invocation)
                out.append("    std::string modIds;")
                out.append("    for (size_t modAt = 0; modList != NULL && modAt < modList->size(); ++modAt)")
                out.append("        modIds += (modIds.empty() ? \"\" : \"\\n\") + boost::lexical_cast<std::string>((*modList)[modAt]->getId());")
            else:
                out.append("    const std::vector<boost::shared_ptr<Entity> > modList = %s;" % invocation)
                out.append("    std::string modIds;")
                out.append("    for (size_t modAt = 0; modAt < modList.size(); ++modAt)")
                out.append("        modIds += (modIds.empty() ? \"\" : \"\\n\") + boost::lexical_cast<std::string>(modList[modAt]->getId());")
            out.append("    *result = mod::Value::FromString(modIds);")
        elif ret[0] == "rawstring":
            out.append("    *result = mod::Value::FromString(%s);" % invocation)
        elif ret[0] == "string":
            out.append("    *result = mod::Value::FromString(ModText::ToUtf8(%s));" % invocation)
        elif ret[0] == "handlelist":
            adder = "AddPersistent" if ret[1] in SHARED_HANDLES else "AddShared"
            out.append("    const %s& modList = %s;" % (ret[2], invocation))
            out.append("    std::vector<mod::Value> modItems;")
            out.append("    for (unsigned int modAt = 0; modAt < static_cast<unsigned int>(modList.size()); ++modAt)")
            out.append("    {")
            out.append("        const boost::shared_ptr<%s> modOwner = modList[modAt];" % ret[1])
            out.append("        const uint32_t modId = modOwner ? ModHandles::%s(modOwner) : 0;" % adder)
            out.append("        modItems.push_back(modId != 0 ? mod::Value::FromHandle(mod::api::eHandle_%s, modId) : mod::Value());" % handle_names_of(api, ret[1]))
            out.append("    }")
            out.append("    *result = mod::Value::FromList(modItems);")
        elif ret[0] == "sharedptr":
            out.append("    const boost::shared_ptr<%s> modOwner = %s;" % (ret[1], invocation))
            adder = "AddPersistent" if ret[1] in SHARED_HANDLES else "AddShared"
            out.append("    const uint32_t modId = modOwner ? ModHandles::%s(modOwner) : 0;" % adder)
            out.append("    if (modId != 0)")
            out.append("        *result = mod::Value::FromHandle(mod::api::eHandle_%s, modId);" % handle_names_of(api, ret[1]))
        elif ret[0] == "entity":
            out.append("    const boost::shared_ptr<Entity> modResult = %s;" % invocation)
            out.append("    if (modResult)")
            out.append("        *result = mod::Value::FromHandle(mod::api::eHandle_entity, ModHandles::Add(modResult));")
        else:
            out.append("    %s* modResult = const_cast<%s*>(%s%s);" % (ret[1], ret[1], "&" if ret[2] == "&" else "", invocation))
            out.append("    if (modResult != NULL)")
            out.append("        *result = mod::Value::FromHandle(mod::api::eHandle_%s, ModHandles::Add(modResult));" % handle_names_of(api, ret[1]))
        out.append("    return true;")
        out.append("}")
        out.append("")
    return "\n".join(out)


def handle_names_of(api, cls):
    if cls in SHARED_HANDLES:
        return SHARED_HANDLES[cls]
    for h in api["handles"]:
        if h["cpp"] == cls + "*":
            return h["name"]
    return cls



# ---- interception of any function: the build weaves a hook into the class's own source -----------------
# `intercept Class from game/path/Class.cpp` makes every method that file defines callable by a mod before it runs
# (with the arguments, able to cancel it or answer for it) and after it returns. Nothing is placed by hand: the
# woven copy of the file is made at build time from the source, so the game's code stays as it is.

DEFINITION = None


def weave_methods(cls):
    methods = []
    graph = class_graph()
    header_text = ""
    if cls["name"] in graph:
        with open(os.path.join(ROOT, "source", graph[cls["name"]][0]), encoding="utf-8", errors="replace") as handle:
            header_text = handle.read()
    pattern = re.compile(r"^([^\n;{}()#]+?)\s+%s::(\w+)\s*\(([^;{}]*?)\)\s*(const)?\s*\{" % re.escape(cls["name"]), re.M | re.S)
    for path in cls["files"]:
        with open(os.path.join(ROOT, "source", path), encoding="utf-8") as handle:
            text = handle.read()
        for match in pattern.finditer(text):
            ret = re.sub(r"\b(inline|virtual|static|explicit)\b", "", match.group(1)).strip()
            name = match.group(2)
            if name == cls["name"] or not ret or (cls.get("only") and name not in cls["only"]):
                continue
            static = bool(re.search(r"\bstatic\b[^;(]*\b%s\s*\(" % re.escape(name), header_text))
            params = split_top(match.group(3))
            # a parameter left unnamed is given a name in the woven copy, so the body of the hook can pass it on
            listed = []
            for index, param in enumerate(params):
                text = param.split("=")[0].strip()
                if not text or text == "void":
                    continue
                m = re.match(r"^(.*?)(\w+)$", text)
                if m and m.group(2) in TYPE_WORDS:
                    m = None
                listed.append(param if (m and m.group(1).strip()) else "%s lceArg%d" % (text, index))
            original = [q for q in params if q.strip() and q.strip() != "void"]
            renamed = ", ".join(listed) if listed != original else None
            methods.append({"ret": re.sub(r"\s+", " ", ret), "name": name, "params": listed, "const": bool(match.group(4)), "static": static,
                            "renamed": renamed, "params_at": (match.start(3), match.end(3)),
                            "owner": cls["name"], "file": path, "brace": match.end() - 1})
    return methods


def class_fields(cls, api):
    """The data members of a class, private ones too, that a mod can read (and write when they are plain numbers): functions made from the
    header, reaching the member by the access an explicit template instantiation allows, in the woven copy of the class's source."""
    graph = class_graph()
    if cls["name"] not in graph:
        return []
    with open(os.path.join(ROOT, "source", graph[cls["name"]][0]), encoding="utf-8", errors="replace") as handle:
        text = re.sub(r"//[^\n]*", "", strip_debug(handle.read()))
    found = class_body(text, cls["name"])
    if not found:
        return []
    body, depth, flat = found[1], 0, ""
    for ch in body:
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
        elif depth == 0:
            flat += ch
    handle_names = {h["cpp"].rstrip("*").strip(): h["name"] for h in api["handles"] if h["cpp"].endswith("*")}
    module = "entity" if cls.get("shared") else ensure_handle(api, cls["name"])
    fields, seen = [], set()
    for statement in flat.split(";"):
        statement = re.sub(r"\s+", " ", statement.replace("\n", " ")).strip()
        statement = re.sub(r"^(?:public|private|protected)\s*:\s*", "", statement)
        if not statement or "(" in statement or re.match(r"^(static|typedef|using|friend|enum|class|struct|template|virtual)\b", statement):
            continue
        match = re.match(r"^(.*?[\w>*&])\s+(\w+)\s*(?:=.*)?$", statement)
        if not match or match.group(2) in seen:
            continue
        ctype, name = match.group(1).strip(), match.group(2)
        kind = wire_type(ctype)
        # a shared pointer member is read as a handle to the object it holds, kept alive by the member for the call
        if kind is None or kind[0] in ("scalarptr", "handlelist") or (kind[0] == "smart" and "::" in kind[1]) or (kind[0] == "handle" and kind[2] == "&") or "const " in ctype.split("*")[-1]:
            continue
        seen.add(name)
        if kind[0] == "enum":
            # an enum declared inside a class is named with its owner outside the class
            kind = ("enum", qualify_enum({}, cls["name"], kind[1]))
            ctype = kind[1]
        elif kind[0] == "handle" and "::" in kind[1]:
            ctype = re.sub(r"\b%s\b" % re.escape(kind[1].split("::")[-1]), kind[1], ctype)  # a nested class is named with its owner
        fields.append({"name": name, "ctype": ctype, "kind": kind})
    functions = []
    for field in fields:
        kind = field["kind"]
        if kind[0] == "scalar":
            mod_type = kind[1]
        elif kind[0] == "string":
            mod_type = "string"
        elif kind[0] == "enum":
            mod_type = "int"
        elif kind[0] in ("handle", "smart"):
            mod_type = ensure_handle(api, kind[1])
        elif kind[0] == "pos":
            # a position field is three numbers, read as name_x, name_y and name_z
            for axis in "xyz":
                functions.append({"static": False, "module": module, "name": "get_%s_%s" % (field["name"], axis), "args": [{"name": "self", "type": module, "mut": False, "from": None}],
                                  "result": "int", "doc": "The game's %s::%s (a field, its %s)." % (cls["name"], field["name"], axis),
                                  "woven_field": {"cls": cls["name"], "field": field, "setter": False, "axis": axis}})
            continue
        else:
            continue
        spec = {"cls": cls["name"], "field": field}
        functions.append({"static": False, "module": module, "name": "get_" + field["name"], "args": [{"name": "self", "type": module, "mut": False, "from": None}],
                          "result": mod_type, "doc": "The game's %s::%s (a field, read)." % (cls["name"], field["name"]), "woven_field": dict(spec, setter=False)})
        if kind[0] in ("scalar", "enum"):
            functions.append({"static": False, "module": module, "name": "set_" + field["name"],
                              "args": [{"name": "self", "type": module, "mut": False, "from": None}, {"name": "value", "type": mod_type, "mut": False, "from": None}],
                              "result": None, "doc": "The game's %s::%s (a field, written)." % (cls["name"], field["name"]), "woven_field": dict(spec, setter=True)})
    return functions


def gen_field_accessors(api, cls, path):
    """Defined at the end of the woven source, where the class is complete: the accessors for its fields."""
    out = ["", "// the fields of %s, reached by explicit instantiation, which is how a private member can be named" % cls["name"],
           "namespace lce_field", "{", "template <class Tag, typename Tag::type Member> struct Rob", "{", "    friend typename Tag::type Take(Tag) { return Member; }", "};", "}"]
    tagged = set()
    for fn in cls["fields"]:
        spec = fn["woven_field"]
        if spec["setter"]:
            continue
        field = spec["field"]
        tag = "%s_%s" % (cls["name"], field["name"])
        if tag in tagged:
            continue  # a position field has three functions and one tag
        tagged.add(tag)
        out += ["namespace lce_field", "{", "struct %s" % tag, "{", "    typedef %s %s::*type;" % (field["ctype"], cls["name"]), "    friend type Take(%s);" % tag, "};",
                "template struct Rob<%s, &%s::%s>;" % (tag, cls["name"], field["name"]), "}"]
    for fn in cls["fields"]:
        spec = fn["woven_field"]
        field, kind = spec["field"], spec["field"]["kind"]
        tag = "%s_%s" % (cls["name"], field["name"])
        member = "(self->*Take(lce_field::%s()))" % tag
        module = fn["module"]
        if cls.get("shared"):
            # an entity handle owns the object; it is the class asked for only when it is one
            out.append("bool ModApi_%s(boost::shared_ptr<Entity> owner, const mod::Value* arguments, mod::Value* result)" % ident(fn))
            out.append("{")
            out += ["    (void)arguments;", "    (void)result;", "    %s* self = dynamic_cast<%s*>(owner.get());" % (cls["name"], cls["name"]), "    if (self == NULL)", "        return false;"]
        else:
            out.append("bool ModApi_%s(%s* self, const mod::Value* arguments, mod::Value* result)" % (ident(fn), cls["name"]))
            out.append("{")
            out += ["    (void)arguments;", "    (void)result;"]
        if spec["setter"]:
            if kind[0] == "enum":
                out.append("    %s = static_cast<%s>(arguments[0].Int());" % (member, kind[1]))
            else:
                out.append("    %s = static_cast<%s>(arguments[0].%s());" % (member, kind[2], SCALAR_ARGS.get(kind[2], "Int")))
        elif kind[0] == "pos":
            out.append("    *result = mod::Value::FromInt(static_cast<int>(%s.%s));" % (member, spec["axis"]))
        elif kind[0] == "scalar":
            out.append("    *result = mod::Value::From%s(%s);" % ({"bool": "Bool", "int": "Int", "float": "Float"}[kind[1]], member))
        elif kind[0] == "enum":
            out.append("    *result = mod::Value::FromInt(static_cast<int>(%s));" % member)
        elif kind[0] == "string":
            out.append("    *result = mod::Value::FromString(ModText::ToUtf8(%s));" % member)
        elif kind[0] == "smart":
            out += ["    %s* value = const_cast<%s*>(%s.get());" % (kind[1], kind[1], member),
                    "    if (value != NULL)", "        *result = mod::Value::FromHandle(mod::api::eHandle_%s, ModHandles::Add(value));" % handle_names_of(api, kind[1])]
        else:
            out += ["    %s* value = const_cast<%s*>(%s%s);" % (kind[1], kind[1], "&" if "*" not in field["ctype"] else "", member),
                    "    if (value != NULL)", "        *result = mod::Value::FromHandle(mod::api::eHandle_%s, ModHandles::Add(value));" % handle_names_of(api, kind[1])]
        out += ["    return true;", "}", ""]
    return "\n".join(out)



def weave_hooks(cls, api):
    methods = weave_methods(cls)
    hooks = class_hooks(cls, api, methods)
    # what a woven body can answer for: a method needing the object's own state conversion, or an argument preamble, is left alone
    kept, dropped = [], 0
    for entry in cls["methods"]:
        if entry["preamble"] or entry["result"] == "state":
            dropped += 1
            continue
        kept.append(entry)
    keep_names = {e["hook"] for e in kept}
    hooks = [h for h in hooks if h["name"] in keep_names]
    cls["methods"] = kept
    after = []
    for entry in kept:
        hook = next(h for h in hooks if h["name"] == entry["hook"])
        # what the function returned is the after hook's result, which it may change (not for an item or a block state)
        carries = entry["result"] in ("bool", "int", "float", "string", "enum") or (entry["result"] or "").startswith(("handle:", "bytes:"))
        args = [dict(a) for a in hook["args"] if a["name"] != "result" or carries]
        for a in args:
            if a["name"] == "result":
                a["seed"] = True
            else:
                a["mut"] = False
        after.append({"name": entry["hook"] + "_after", "args": args, "cancel": False, "provider": False, "generated": True, "targets": [],
                      "doc": ("Called as %s::%s returns, in the game's own code, with the arguments it had; result is what it returned and can be changed." if carries else
                              "Called as %s::%s returns, in the game's own code, with the arguments it had.") % (cls["name"], entry["method"]["name"])})
        entry["after"] = after[-1]["name"]
        entry["after_result"] = carries
    cls["fields"] = class_fields(cls, api)
    api["functions"] += cls["fields"]
    return hooks + after


def function_end(text, brace):
    """The index of the brace that closes the function body opened at `brace`, skipping strings, characters and comments."""
    depth, i, n = 0, brace, len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and text[i + 1:i + 2] == "/":
            i = text.find("\n", i)
            if i < 0:
                return None
        elif ch == "/" and text[i + 1:i + 2] == "*":
            i = text.find("*/", i) + 1
        elif ch == '"' or ch == "'":
            quote = ch
            i += 1
            while i < n and text[i] != quote:
                i += 2 if text[i] == "\\" else 1
        elif ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return None


LOCK_PRELUDE = ["namespace lce_entity", "{", "template <class Tag, typename Tag::type Member> struct Rob", "{", "    friend typename Tag::type Take(Tag) { return Member; }", "};",
                "struct WeakTag", "{", "    typedef boost::weak_ptr<Entity> boost::enable_shared_from_this<Entity>::*type;", "    friend type Take(WeakTag);", "};",
                "template struct Rob<WeakTag, &boost::enable_shared_from_this<Entity>::weak_this_>;",
                "inline boost::shared_ptr<Entity> Lock(Entity* entity)", "{", "    return (entity->*Take(WeakTag())).lock();", "}", "}"]


def gen_woven(api, cls, path):
    """The text of a source file with its interception woven in."""
    with open(os.path.join(ROOT, "source", path), encoding="utf-8") as handle:
        text = handle.read()
    edits = []
    for entry in cls["methods"]:
        method = entry["method"]
        if method["file"] != path:
            continue
        end = function_end(text, method["brace"])
        if end is None:
            continue
        shared = bool(cls.get("shared")) and not method.get("static")
        self_expr = "lceSelf" if shared else "const_cast<%s*>(this)" % cls["name"]
        expressions = [a[2].replace("@SELFPTR@", self_expr) for a in entry["arguments"]]
        event = "ModEvents::" + pascal(entry["hook"])
        ret, result = method["ret"], entry["result"]
        owned = "lceSelf && " if shared else ""
        lines = [""]
        if shared:
            # an entity has a shared pointer to itself only once the game owns it, so the hooks wait for that
            lines.append("    boost::shared_ptr<Entity> lceSelf = lce_entity::Lock(const_cast<%s*>(this));" % cls["name"])
        lines += ["    if (%sModHooks::IsActive(kModHook_%s))" % (owned, entry["hook"]), "    {"]
        if result:
            if result.startswith("bytes:"):
                element = result[6:]
                lines += ["        std::string lceResult;", "        if (!%s(%s))" % (event, ", ".join(expressions + ["lceResult"])), "        {",
                          "            const unsigned int lceCount = static_cast<unsigned int>(lceResult.size() / sizeof(%s));" % element,
                          "            %s* lceData = new %s[lceCount + 1];" % (element, element),
                          "            memcpy(lceData, lceResult.data(), lceCount * sizeof(%s));" % element,
                          "            return arrayWithLength<%s>(lceData, lceCount);" % element, "        }"]
            else:
                default = {"bool": "false", "int": "0", "float": "0.0f", "string": "std::wstring()", "item": "std::wstring()", "enum": "0"}.get(result, "NULL")
                local = ("%s*" % result[7:]) if result.startswith("handle:") else {"item": "std::wstring", "enum": "int"}.get(result, ret)
                lines.append("        %s lceResult = %s;" % (local, default))
                conv = "ModItemNames::Find(lceResult)" if result == "item" else ("static_cast<%s>(lceResult)" % ret if result == "enum" else "lceResult")
                lines += ["        if (!%s(%s))" % (event, ", ".join(expressions + ["lceResult"])), "            return %s;" % conv]
        else:
            lines += ["        if (!%s(%s))" % (event, ", ".join(expressions)), "            return;"]
        lines.append("    }")
        after_event = "ModEvents::" + pascal(entry["after"])
        lines.append("    auto lceBody = [&]() -> %s" % ret)
        lines.append("    {")
        head = "\n".join(lines)
        body = text[method["brace"] + 1:end]
        tail = ["    };"]
        if ret == "void":
            tail += ["    lceBody();", "    if (%sModHooks::IsActive(kModHook_%s))" % (owned, entry["after"]), "        %s(%s);" % (after_event, ", ".join(expressions))]
        elif entry.get("after_result"):
            if result == "enum":
                tail += ["    %s lceReturn = lceBody();" % ret, "    if (%sModHooks::IsActive(kModHook_%s))" % (owned, entry["after"]), "    {", "        int lceMod = static_cast<int>(lceReturn);",
                         "        %s(%s);" % (after_event, ", ".join(expressions + ["lceMod"])), "        lceReturn = static_cast<%s>(lceMod);" % ret, "    }"]
            elif result.startswith("bytes:"):
                element = result[6:]
                tail += ["    %s lceReturn = lceBody();" % ret, "    if (%sModHooks::IsActive(kModHook_%s))" % (owned, entry["after"]), "    {",
                         "        std::string lceMod(reinterpret_cast<const char*>(lceReturn.data), lceReturn.length * sizeof(%s));" % element,
                         "        %s(%s);" % (after_event, ", ".join(expressions + ["lceMod"])),
                         "        const size_t lceKeep = lceMod.size() < lceReturn.length * sizeof(%s) ? lceMod.size() : lceReturn.length * sizeof(%s);" % (element, element),
                         "        if (lceReturn.data != NULL)", "            memcpy(lceReturn.data, lceMod.data(), lceKeep);", "    }"]
            elif result.startswith("handle:"):
                tail += ["    %s lceReturn = lceBody();" % ret, "    if (%sModHooks::IsActive(kModHook_%s))" % (owned, entry["after"]), "    {", "        %s* lceMod = const_cast<%s*>(lceReturn);" % (result[7:], result[7:]),
                         "        %s(%s);" % (after_event, ", ".join(expressions + ["lceMod"])), "        lceReturn = lceMod;", "    }"]
            else:
                tail += ["    %s lceReturn = lceBody();" % ret, "    if (%sModHooks::IsActive(kModHook_%s))" % (owned, entry["after"]), "    {", "        %s lceMod = lceReturn;" % {"string": "std::wstring"}.get(result, ret),
                         "        %s(%s);" % (after_event, ", ".join(expressions + ["lceMod"])), "        lceReturn = lceMod;", "    }"]
            tail.append("    return lceReturn;")
        else:
            tail += ["    %s lceReturn = lceBody();" % ret, "    if (%sModHooks::IsActive(kModHook_%s))" % (owned, entry["after"]), "        %s(%s);" % (after_event, ", ".join(expressions)), "    return lceReturn;"]
        edits.append((method["brace"] + 1, end, head + "\n" + body + "\n" + "\n".join(tail) + "\n"))
        if method.get("renamed"):
            edits.append((method["params_at"][0], method["params_at"][1], method["renamed"]))
    for start, end, block in sorted(edits, reverse=True):
        text = text[:start] + block + text[end:]
    text = text.rstrip("\n") + "\n" + gen_field_accessors(api, cls, path) if cls.get("fields") else text
    prelude = ['#include <string.h>', '#include "game/core/ModApiFunctions_generated.hpp"', '#include "game/core/ModText.hpp"', '#include "mod/ModApiIds_generated.hpp"',
               '#include "game/core/ModEvents_generated.hpp"', '#include "game/core/ModHandles.hpp"', '#include "game/core/ModHooks.hpp"',
               '#include "game/core/ModItemNames.hpp"', '#include "game/world/block/Block.hpp"', '#include "game/world/block/BlockState.hpp"',
               '#include "game/world/Level.hpp"', '#include "game/world/generation/Player.hpp"', '#include "game/entity/mob/LivingEntity.hpp"',
               '#include "game/entity/mob/Entity.hpp"'] + (LOCK_PRELUDE if cls.get("shared") else []) + ['#line 1 "%s"' % path, ""]
    return "\n".join(prelude) + text



def validate(api):
    handles = [h["name"] for h in api["handles"]]
    # a list of handles is written list:handle
    known = set(SCALARS) | set(handles) | set("list:" + name for name in handles)
    hook_known = known | set(CONVERTED)
    seen = set()
    for fn in api["functions"]:
        key = (fn["module"], fn["name"], fn["static"])
        if key in seen:
            raise ApiError("fn %s.%s is declared twice" % key)
        seen.add(key)
        if not fn["static"] and fn["module"] not in handles:
            raise ApiError("fn %s.%s: %s is not a handle" % (fn["module"], fn["name"], fn["module"]))
        if not fn["static"] and (not fn["args"] or fn["args"][0]["name"] != "self" or fn["args"][0]["type"] != fn["module"]):
            raise ApiError("fn %s.%s must take self: %s first" % (fn["module"], fn["name"], fn["module"]))
        for arg in fn["args"] + ([{"type": fn["result"]}] if fn["result"] else []):
            if arg["type"] not in known:
                raise ApiError("fn %s.%s: unknown type %s" % (fn["module"], fn["name"], arg["type"]))
    names = set()
    for hook in api["hooks"]:
        if hook["name"] in names:
            raise ApiError("hook %s is declared twice" % hook["name"])
        names.add(hook["name"])
        if not hook["provider"] and not hook["targets"] and not hook.get("generated"):
            raise ApiError("hook %s needs a target" % hook["name"])
        for arg in hook["args"]:
            if arg["type"] not in hook_known:
                raise ApiError("hook %s: unknown type %s" % (hook["name"], arg["type"]))
            if arg.get("from") and arg["type"] != "entity_type":
                raise ApiError("hook %s: only entity_type can come from another argument" % hook["name"])
            if arg["mut"] and arg["type"] not in GETTERS and arg["type"] not in handles:
                raise ApiError("hook %s: %s cannot be mut" % (hook["name"], arg["name"]))


def mod_type(kind):
    return CONVERTED[kind][1] if kind in CONVERTED else kind


def type_enum(kind, handles):
    kind = mod_type(kind)
    if kind is None:
        return "eType_Nil", 0
    if kind.startswith("list:"):
        return "eType_List", handles.index(kind[5:]) + 1
    if kind in handles:
        return "eType_Handle", handles.index(kind) + 1
    return {"bool": "eType_Bool", "int": "eType_Int", "float": "eType_Float", "string": "eType_String", "bytes": "eType_String"}[kind], 0


def ident(fn):
    # a generated static function can share its name with a method of the same class
    return "%s_%s%s" % (fn["module"], fn["name"], "_static" if fn.get("generated") and fn["static"] else "")


def gen_ids(api):
    out = ["#ifndef MOD_MODAPIIDS_GENERATED_HPP", "#define MOD_MODAPIIDS_GENERATED_HPP", "", "namespace mod", "{", "namespace api", "{",
           "const int kVersion = %d;" % api["version"], "", "enum EHandle", "{", "    eHandle_None = 0,"]
    out += ["    eHandle_%s = %d," % (h["name"], i + 1) for i, h in enumerate(api["handles"])]
    out += ["    eHandle_Count = %d" % (len(api["handles"]) + 1), "};", "", "enum EFunction", "{"]
    out += ["    eFunction_%s," % ident(f) for f in api["functions"]]
    out += ["    eFunction_Count", "};", "", "enum EHook", "{"]
    out += ["    eHook_%s," % h["name"] for h in api["hooks"]]
    out += ["    eHook_Count", "};", "}", "}", "", "#endif", ""]
    return "\n".join(out)


def arg_struct(arg, handles):
    enum, kind = type_enum(arg["type"], handles)
    return '{"%s", mod::Value::%s, %d, %s}' % (arg["name"], enum, kind, "true" if arg.get("mut") else "false")


def gen_table(api):
    handles = [h["name"] for h in api["handles"]]
    out = ['#include "mod/ModApi.hpp"', "", "namespace mod", "{", "namespace api", "{", "namespace", "{"]
    for fn in api["functions"]:
        out.append("const ArgumentInfo kArguments_%s[] = {%s};" % (ident(fn), ", ".join(arg_struct(a, handles) for a in fn["args"])))
    for hook in api["hooks"]:
        out.append("const ArgumentInfo kHookArguments_%s[] = {%s};" % (hook["name"], ", ".join(arg_struct(a, handles) for a in hook["args"]) or '{"", mod::Value::eType_Nil, 0, false}'))
    out += ["}", "", "const char* const kHandleNames[] = {\"\", %s};" % ", ".join('"%s"' % h for h in handles), ""]
    out.append("const FunctionInfo kFunctions[] = {")
    for fn in api["functions"]:
        result, rkind = type_enum(fn["result"], handles)
        out.append('    {"%s", "%s", kArguments_%s, %d, mod::Value::%s, %d, %s},' % (fn["module"], fn["name"], ident(fn), len(fn["args"]), result, rkind, "true" if fn["static"] else "false"))
    out += ["};", "", "const HookInfo kHooks[] = {"]
    for hook in api["hooks"]:
        out.append('    {"%s", kHookArguments_%s, %d, %s},' % (hook["name"], hook["name"], len(hook["args"]), "true" if hook["cancel"] else "false"))
    out += ["};", ""]
    handle_names = [h["name"] for h in api["handles"]]
    pairs = handle_bases(api)
    out.append("const HandleBase kHandleBases[] = {%s};" % (", ".join("{%d, %d}" % (handle_names.index(d) + 1, handle_names.index(b) + 1) for d, b, _, _ in pairs) or "{0, 0}"))
    out.append("const int kHandleBaseCount = %d;" % len(pairs))
    out += ["", "bool HandleAccepts(unsigned int expected, unsigned int actual)", "{", "    if (expected == actual)", "        return true;",
            "    for (int index = 0; index < kHandleBaseCount; ++index)", "    {",
            "        if (kHandleBases[index].derived == actual && kHandleBases[index].base == expected)", "            return true;", "    }",
            "    return false;", "}", "}", "}", ""]
    return "\n".join(out)


def gen_hook_ids(api):
    out = ["#ifndef GAME_CORE_MODHOOKIDS_GENERATED_HPP", "#define GAME_CORE_MODHOOKIDS_GENERATED_HPP", "", "enum EModHook", "{"]
    out += ["    kModHook_%s = %d," % (h["name"], i) for i, h in enumerate(api["hooks"])]
    out += ["    kModHook_Count = %d" % len(api["hooks"]), "};", "", "#endif", ""]
    return "\n".join(out)


def self_type(api, fn):
    return next(h["cpp"] for h in api["handles"] if h["name"] == fn["module"])


def gen_functions_header(api):
    out = ['#ifndef GAME_CORE_MODAPIFUNCTIONS_GENERATED_HPP', '#define GAME_CORE_MODAPIFUNCTIONS_GENERATED_HPP', "", '#include "game/core/ModHandles.hpp"', '#include "mod/ModValue.hpp"', ""]
    out += ["class %s;" % h["cpp"].rstrip("*") for h in api["handles"] if h["cpp"].endswith("*") and "::" not in h["cpp"]] + [""]
    owners = sorted({h["cpp"].split("::")[0] for h in api["handles"] if "::" in h["cpp"]})
    out += ['#include "%s"' % class_graph()[o][0] for o in owners if o in class_graph()] + [""]
    for fn in api["functions"]:
        if fn["static"]:
            out.append("bool ModApi_%s(const mod::Value* arguments, mod::Value* result);" % ident(fn))
        else:
            out.append("bool ModApi_%s(%s self, const mod::Value* arguments, mod::Value* result);" % (ident(fn), self_type(api, fn)))
    out += ["", "bool ModApiDispatch(int function, const mod::Value* arguments, size_t count, mod::Value* result);", "", "#endif", ""]
    return "\n".join(out)


def gen_dispatch(api):
    out = ['#include "game/core/ModApiFunctions_generated.hpp"', "", '#include "mod/ModApi.hpp"', "", "bool ModApiDispatch(int function, const mod::Value* arguments, size_t count, mod::Value* result)", "{",
           "    if (function < 0 || function >= mod::api::eFunction_Count)", "        return false;",
           "    const mod::api::FunctionInfo& info = mod::api::kFunctions[function];",
           "    if (count != static_cast<size_t>(info.argumentCount))", "        return false;",
           "    for (size_t index = 0; index < count; ++index)", "    {",
           "        const mod::api::ArgumentInfo& expected = info.arguments[index];",
           "        const bool number = expected.type == mod::Value::eType_Int || expected.type == mod::Value::eType_Float;",
           "        const bool numeric = arguments[index].Type() == mod::Value::eType_Int || arguments[index].Type() == mod::Value::eType_Float;",
           "        if (!(number ? numeric : arguments[index].Type() == expected.type))", "            return false;",
           "        if (expected.type == mod::Value::eType_Handle && !mod::api::HandleAccepts(expected.handle, arguments[index].Kind()))", "            return false;",
           "    }", "    switch (function)", "    {"]
    for fn in api["functions"]:
        out.append("    case mod::api::eFunction_%s:" % ident(fn))
        out.append("    {")
        if fn["static"]:
            out.append("        return ModApi_%s(arguments, result);" % ident(fn))
        else:
            out.append("        %s self;" % self_type(api, fn))
            owned_self = "shared_ptr<" in self_type(api, fn) and "<Entity>" not in self_type(api, fn)
            out.append("        if (!ModHandles::%s(arguments[0].Id(), &self))" % ("GetShared" if owned_self else "Get"))
            out.append("            return false;")
            out.append("        return ModApi_%s(self, arguments + 1, result);" % ident(fn))
        out.append("    }")
    out += ["    default:", "        return false;", "    }", "}", ""]
    return "\n".join(out)


C_TYPES = {"bool": "int", "int": "int64_t", "float": "double", "string": "const char*"}


def gen_sdk(api):
    handles = [h["name"] for h in api["handles"]]
    out = ["/* The mod SDK header for WebAssembly mods, generated from api/lce.api. Do not edit. */",
           "#ifndef LCE_H", "#define LCE_H", "", "#include <stdint.h>", "#include <string.h>", "",
           '#define LCE_IMPORT(name) __attribute__((import_module("lce"), import_name(name)))', "",
           "LCE_IMPORT(\"log\") void lce_log(const char* text, int length);",
           "LCE_IMPORT(\"say\") void lce_say(const char* text, int length);",
           "LCE_IMPORT(\"run\") int lce_run(const char* line, int length);",
           "LCE_IMPORT(\"command\") void lce_register_command(const char* name, int length);",
           "LCE_IMPORT(\"reply\") void lce_reply(const char* text, int length);",
           "LCE_IMPORT(\"call\") int lce_call(int function, const void* in, int in_length, void* out, int out_capacity);",
           "LCE_IMPORT(\"hook_set\") void lce_hook_set(int index, const void* value, int length);",
           "LCE_IMPORT(\"hook_register\") void lce_hook_register(int hook, int priority);",
           "LCE_IMPORT(\"listen\") void lce_listen(const char* name, int length);",
           "LCE_IMPORT(\"fire\") int lce_fire(const char* name, int name_length, const void* arguments, int arguments_length);", "",
           "typedef struct { int kind; uint32_t id; } lce_handle;", ""]
    for h in handles:
        out.append("enum { LCE_HANDLE_%s = %d };" % (h.upper(), handles.index(h) + 1))
    out.append("")
    for i, fn in enumerate(api["functions"]):
        out.append("enum { LCE_FN_%s_%s = %d };" % (fn["module"].upper(), fn["name"].upper(), i))
    for i, hook in enumerate(api["hooks"]):
        out.append("enum { LCE_HOOK_%s = %d };" % (hook["name"].upper(), i))
    out += ["", "/* Values travel as a count byte then, per value: a type byte (1 bool, 2 int, 3 float, 4 string, 5 handle, 6 list) and", "   its bytes; see the Value codec in source/mod/ModValue.cpp. A list, the result written list:handle, is a length of four", "   bytes and then that many bytes: a count of four bytes and each item packed as a value here. */", ""]
    out += ["int lce_pack_int(uint8_t* out, int64_t value);", "int lce_pack_float(uint8_t* out, double value);", "int lce_pack_bool(uint8_t* out, int value);",
            "int lce_pack_string(uint8_t* out, const char* text, int length);", "int lce_pack_handle(uint8_t* out, lce_handle handle);", "", "#ifdef LCE_IMPLEMENTATION",
            "int lce_pack_int(uint8_t* out, int64_t v) { out[0] = 2; memcpy(out + 1, &v, 8); return 9; }",
            "int lce_pack_float(uint8_t* out, double v) { out[0] = 3; memcpy(out + 1, &v, 8); return 9; }",
            "int lce_pack_bool(uint8_t* out, int v) { out[0] = 1; out[1] = v != 0; return 2; }",
            "int lce_pack_string(uint8_t* out, const char* t, int n) { out[0] = 4; memcpy(out + 1, &n, 4); memcpy(out + 5, t, n); return 5 + n; }",
            "int lce_pack_handle(uint8_t* out, lce_handle h) { uint64_t id = h.id; out[0] = 5; out[1] = (uint8_t)h.kind; memcpy(out + 2, &id, 8); return 10; }",
            "#endif", "", "#endif", ""]
    return "\n".join(out)


def pascal(name):
    return "".join(part[:1].upper() + part[1:] for part in name.split("_"))


def event_parameter(api, arg):
    if arg["type"] in CONVERTED:
        return "%s %s" % (CONVERTED[arg["type"]][0], arg["name"])
    if arg["type"] in NATIVE:
        return "%s %s" % ((NATIVE_MUT if arg["mut"] else NATIVE)[arg["type"]], arg["name"])
    native = next(h["native"] for h in api["handles"] if h["name"] == arg["type"])
    return "%s%s %s" % (native, "&" if arg["mut"] else "", arg["name"])


def event_signature(api, hook):
    parameters = [event_parameter(api, a) for a in hook["args"] if not a.get("from")]
    return "%s %s(%s)" % ("bool" if hook["cancel"] else "void", pascal(hook["name"]), ", ".join(parameters))


def gen_events_header(api):
    out = ["#ifndef GAME_CORE_MODEVENTS_GENERATED_HPP", "#define GAME_CORE_MODEVENTS_GENERATED_HPP", "", "#include <string>", "",
           "#include <boost/shared_ptr.hpp>", "", '#include "game/core/ModHandles.hpp"', '#include "game/world/LevelSource.hpp"', "",
           "class Block;", "class DamageSource;", "class Entity;", "class Item;", "class SoundEvent;"] + ["class %s;" % h["name"] if False else "class %s;" % h["cpp"].rstrip("*") for h in api["handles"] if h["cpp"].endswith("*") and "::" not in h["cpp"]] + ["",
           "namespace ModEvents", "{"]
    for hook in api["hooks"]:
        out.append(event_signature(api, hook) + ";")
    out += ["}", "", "#endif", ""]
    return "\n".join(out)


def gen_events_source(api):
    out = ['#include "game/core/ModEvents_generated.hpp"', "", '#include "game/core/ModHookCall.hpp"', '#include "mod/ModApiIds_generated.hpp"', "", "namespace ModEvents", "{"]
    for hook in api["hooks"]:
        out += [event_signature(api, hook), "{", "    ModHookCall hook(kModHook_%s);" % hook["name"]]
        for arg in hook["args"]:
            handle = next((h for h in api["handles"] if h["name"] == arg["type"] and h["cpp"].endswith("*")), None)
            if handle is not None and arg["mut"] and arg.get("seed"):
                out.append("    if (%s != NULL)" % arg["name"])
                out.append("        hook.AddObject(mod::api::eHandle_%s, ModHandles::Add(%s));" % (handle["name"], arg["name"]))
                out.append("    else")
                out.append("        hook.AddNil();")
                continue
            elif handle is not None and arg["mut"]:
                out.append("    hook.AddNil();")
                continue
            if handle is not None:
                out.append("    hook.AddObject(mod::api::eHandle_%s, ModHandles::Add(%s));" % (handle["name"], arg["name"]))
                continue
            out.append("    hook.%s(%s);" % (CONVERTED[arg["type"]][2] if arg["type"] in CONVERTED else "Add", arg.get("from") or arg["name"]))
        out.append("    const bool proceed = hook.Run();" if hook["cancel"] else "    hook.Run();")
        for index, arg in enumerate(hook["args"]):
            handle = next((h for h in api["handles"] if h["name"] == arg["type"] and h["cpp"].endswith("*")), None)
            if arg["mut"] and handle is not None:
                out.append("    {")
                out.append("        uint32_t id = 0;")
                out.append("        %s* object = NULL;" % handle["cpp"].rstrip("*"))
                out.append("        %s = hook.GetHandle(%d, &id) && ModHandles::Get(id, &object) ? object : NULL;" % (arg["name"], index))
                out.append("    }")
            elif arg["mut"]:
                out.append("    %s = hook.%s(%d, %s);" % (arg["name"], GETTERS[arg["type"]], index, arg["name"]))
        if hook["cancel"]:
            out.append("    return proceed;")
        out += ["}", ""]
    out += ["}", ""]
    return "\n".join(out)


def gen_docs(api):
    out = ["# The mod API", "", "Generated from `api/lce.api` by `tools/hooks/hookgen.py`; do not edit this file. API version %d." % api["version"], "",
           "How to use it from Lua and WebAssembly is in `modding.md`. A handle is a value that names a thing in the",
           "game. A handle is valid only while the call or hook that gave it to you runs, except a `surface`, which lives until you destroy it or your mod is unloaded.",
           "A result written `list:handle` is a list of those handles: a table in order in Lua, and a list value in WebAssembly (`lce.h`); an empty slot is nil.", "", "## Handles", ""]
    for h in api["handles"]:
        out.append("- `%s`: %s" % (h["name"], h["doc"]))
    out += ["", "## Functions", "", "In Lua a function of a handle is a method (`player:name()`); in C it is a call by number (`LCE_FN_ENTITY_NAME`) through `lce_call`.", ""]
    for fn in api["functions"]:
        out.append("### `%s`%s" % (fn_call(fn), (" -> " + fn["result"]) if fn["result"] else ""))
        out.append("")
        out.append(fn["doc"])
        out.append("")
    out += ["## Hooks", "", "A hook is a point where the game asks mods before it decides. In Lua, `mod.hook(\"name\", function(call) ... end)`; `call` has",
            "the arguments by name, `call.cancel = true` stops what the game was about to do, and an argument marked `mut` can be assigned.", ""]
    for hook in api["hooks"]:
        out.append("### `%s`%s" % (hook_call(hook), " - can be cancelled" if hook["cancel"] else ""))
        out.append("")
        out.append(hook["doc"])
        out.append("")
        if hook["provider"]:
            out.append("A provider: when a mod listens, the game uses the mod's work instead of its own.")
        elif hook.get("generated"):
            out.append("Generated from the game's class; it fires for objects mods register, with the hook's first argument naming which.")
        else:
            out.append("Placed in %s." % ", ".join("`%s`" % t["target"].split("(")[0] for t in hook["targets"]))
        out.append("")
    return "\n".join(out)


def gen_mob_bases(api):
    lines = ["// generated by tools/hooks/hookgen.py from the mobbase lines of api/lce.api; do not edit", "#ifndef GAME_CORE_MODENTITYBASES_GENERATED_HPP",
             "#define GAME_CORE_MODENTITYBASES_GENERATED_HPP", ""]
    lines += ['#include "%s"' % header for header in sorted({b["header"] for b in api.get("mobbases", [])})]
    lines += ['#include "game/core/EntityType.hpp"', '#include "game/entity/mob/ScriptedMob.hpp"', "",
              "// the game's own mob classes a mod mob can be made from", "#define LCE_MOD_ENTITY_BASES \\"]
    entries = ['    {L"%s", EntityType::%s, ScriptedMob<%s>::create}' % (b["name"], b["type"], b["class"]) for b in api.get("mobbases", [])]
    lines.append(", \\\n".join(entries))
    lines += ["", "#endif"]
    return "\n".join(lines)


def outputs(api):
    files = {cls["output"] and "source/" + cls["output"]: gen_class(api, cls) for cls in api["classes"]}
    files.update({
        "source/mod/ModApiIds_generated.hpp": gen_ids(api),
        "source/mod/ModApiTable_generated.cpp": gen_table(api),
        "source/game/core/ModHookIds_generated.hpp": gen_hook_ids(api),
        "source/game/core/ModEntityBases_generated.hpp": gen_mob_bases(api),
        "source/game/core/ModApiFunctions_generated.hpp": gen_functions_header(api),
        "source/game/core/ModApiDispatch_generated.cpp": gen_dispatch(api),
        "source/game/core/ModEvents_generated.hpp": gen_events_header(api),
        "source/game/core/ModEvents_generated.cpp": gen_events_source(api),
        "source/game/core/ModExposed_generated.cpp": gen_exposed(api),
        "tools/sdk/lce.h": gen_sdk(api),
        "docs/modding-api.md": gen_docs(api),
        "tools/hooks/woven.txt": "\n".join(sorted({path for cls in api["woven"] for path in cls["files"]})) + "\n",
    })
    return files


def find_target(target):
    name, _, parameters = target.partition("(")
    cls, _, method = name.rpartition("::")
    tail = ""
    if parameters:
        tail = r"\s*\(\s*" + r"\s+".join(re.escape(word) for word in parameters.split())
    else:
        tail = r"\s*\("
    owner = re.escape(cls) + "::" if cls else ""
    pattern = re.compile(r"^(?!\s)[^;\n]*\b%s%s%s" % (owner, re.escape(method), tail), re.M)
    hits = []
    for base, _, files in os.walk(os.path.join(ROOT, "source", "game")):
        for filename in files:
            if filename.endswith(".cpp"):
                path = os.path.join(base, filename)
                with open(path, encoding="utf-8") as handle:
                    text = handle.read()
                found = pattern.findall(text)
                if len(found) > 1:
                    raise ApiError("target %s matches %d definitions in %s; add the first parameters to tell them apart" % (target, len(found), path))
                if found:
                    hits.append((path, text, pattern.search(text)))
    if len(hits) != 1:
        raise ApiError("target %s matches %d definitions" % (target, len(hits)))
    return hits[0]


def called(api, hook):
    problems = []
    for entry in hook["targets"]:
        path, text, match = find_target(entry["target"])
        span = text[match.start():]
        end = re.search(r"^\}\s*$", span, re.M)
        body = span[:end.end()] if end else span
        if "ModEvents::%s(" % pascal(hook["name"]) not in body:
            problems.append("hook %s is not called in %s" % (hook["name"], entry["target"].split("(")[0]))
        if '#include "game/core/ModEvents_generated.hpp"' not in text:
            problems.append("%s does not include game/core/ModEvents_generated.hpp" % os.path.relpath(path, ROOT))
    return problems


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("action", choices=("generate", "check", "weave", "freeze"))
    parser.add_argument("source", nargs="?", help="weave: the source file, relative to source/")
    parser.add_argument("output", nargs="?", help="weave: where to write the woven copy")
    args = parser.parse_args()
    try:
        api = parse(API)
        problems = []
        if args.action == "weave":
            cls = next(c for c in api["woven"] if args.source in c["files"])
            os.makedirs(os.path.dirname(args.output), exist_ok=True)
            with open(args.output, "w", encoding="utf-8", newline="\n") as handle:
                handle.write(gen_woven(api, cls, args.source))
            return 0
        if args.action == "freeze":
            with open(STABLE, "w", encoding="utf-8", newline="\n") as handle:
                handle.write(freeze(api))
            print("wrote api/stable.txt")
            return 0
        if args.action == "generate":
            for name, text in outputs(api).items():
                path = os.path.join(ROOT, name)
                os.makedirs(os.path.dirname(path), exist_ok=True)
                with open(path, "w", encoding="utf-8", newline="\n") as handle:
                    handle.write(text if text.endswith("\n") else text + "\n")
                print("wrote", name)
        else:
            for name, text in outputs(api).items():
                path = os.path.join(ROOT, name)
                expected = text if text.endswith("\n") else text + "\n"
                if not os.path.isfile(path) or open(path, encoding="utf-8").read() != expected:
                    problems.append("%s is out of date; run hookgen.py generate" % name)
            for hook in api["hooks"]:
                if not hook.get("generated"):
                    problems += called(api, hook)
        if problems:
            for problem in problems:
                print("error:", problem, file=sys.stderr)
            return 1
    except ApiError as error:
        print("error:", error, file=sys.stderr)
        return 1
    return 0


sys.exit(main())
