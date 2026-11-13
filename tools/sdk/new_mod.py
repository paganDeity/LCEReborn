#!/usr/bin/env python3
"""Make a new mod from a template: new_mod.py <id> [--kind lua|wasm] [--dir FOLDER] [--name TEXT].

The template is the smallest mod that does something (a hook, a command, a setting for Lua; a command for WebAssembly). `--self-test` makes both kinds in a
temporary folder and checks them, which is what ctest runs."""
import argparse
import os
import re
import shutil
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ID_RULE = re.compile(r"^[a-z0-9_-]{3,32}$")


def make(mod_id, kind, directory, name):
    if not ID_RULE.match(mod_id):
        raise SystemExit("a mod id is 3 to 32 characters of a-z 0-9 _ -")
    source = os.path.join(HERE, "templates", kind)
    target = os.path.join(directory, mod_id)
    if os.path.exists(target):
        raise SystemExit("%s already exists" % target)
    os.makedirs(target)
    for filename in sorted(os.listdir(source)):
        with open(os.path.join(source, filename), encoding="utf-8") as handle:
            text = handle.read().replace("{{id}}", mod_id).replace("{{name}}", name)
        with open(os.path.join(target, filename), "w", encoding="utf-8") as handle:
            handle.write(text)
    if kind == "wasm":
        shutil.copy(os.path.join(HERE, "lce.h"), os.path.join(target, "lce.h"))
    return target


def manifest(path):
    values = {}
    with open(os.path.join(path, "mod.txt"), encoding="utf-8") as handle:
        for line in handle:
            if "=" in line:
                key, value = line.strip().split("=", 1)
                values[key] = value
    return values


def self_test():
    with tempfile.TemporaryDirectory() as folder:
        for kind, entry in (("lua", "main.lua"), ("wasm", "samplemod.wasm")):
            os.makedirs(os.path.join(folder, kind))
            path = make("samplemod", kind, os.path.join(folder, kind), "Sample Mod")
            values = manifest(path)
            assert values["id"] == "samplemod" and values["api"] == "1" and values["entry"] == entry, (kind, values)
            for filename in os.listdir(path):
                with open(os.path.join(path, filename), encoding="utf-8", errors="replace") as handle:
                    assert "{{" not in handle.read(), "%s still has a placeholder" % filename
            if kind == "lua":
                with open(os.path.join(path, "main.lua"), encoding="utf-8") as handle:
                    text = handle.read()
                assert 'mod.hook("MinecraftServer_tick"' in text and 'mod.command("samplemod"' in text
            else:
                assert os.path.exists(os.path.join(path, "lce.h"))
        try:
            make("Bad Id", "lua", folder, "x")
        except SystemExit:
            pass
        else:
            raise AssertionError("a bad id was accepted")
    print("sdk templates ok")


def check_loads(binary):
    """Make the Lua template and the library it needs in a folder and have the mod test load them: the template must run, not only exist."""
    import subprocess
    with tempfile.TemporaryDirectory() as folder:
        make("samplemod", "lua", folder, "Sample Mod")
        shutil.copytree(os.path.join(HERE, "..", "..", "docs", "examples", "mods", "lcestd"), os.path.join(folder, "lcestd"))
        result = subprocess.run([binary], env=dict(os.environ, LCE_EXTRA_MODS=folder), capture_output=True, text=True)
        if result.returncode != 0:
            sys.stderr.write(result.stdout + result.stderr)
            raise SystemExit("the template mod does not load")
    print("the template mod loads")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("id", nargs="?")
    parser.add_argument("--kind", choices=("lua", "wasm"), default="lua")
    parser.add_argument("--dir", default="mods")
    parser.add_argument("--name")
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--check-loads", metavar="MOD_TEST", help="the mod test program, to load the template with")
    args = parser.parse_args()
    if args.self_test:
        self_test()
        return
    if args.check_loads:
        check_loads(args.check_loads)
        return
    if not args.id:
        parser.error("give the mod's id")
    os.makedirs(args.dir, exist_ok=True)
    print("made", make(args.id, args.kind, args.dir, args.name or args.id.replace("_", " ").replace("-", " ").title()))


if __name__ == "__main__":
    main()
