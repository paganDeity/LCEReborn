"""Named checks that boot a world, run steps and report pass or fail.

A scenario is a JSON file in tools/harness/scenarios/:

    {
      "description": "what the scenario proves",
      "world": {"seed": "12345", "mode": "creative"},
      "steps": [
        {"cmd": "setblock ~ ~ ~ stone", "expect": "Block placed"},
        {"cmd": "data get block ~ ~ ~", "expect_error": "no data"},
        {"wait": 20},
        {"hold": true},
        {"step": 5},
        {"state": "ticksHeld", "equals": true}
      ]
    }

"expect" and "expect_error" are regular expressions searched in the reply
lines; a reply that starts with the chat error colour counts as an error.
"allow_error": true on a command step accepts an error too, for housekeeping that
may have nothing to do (clearing an empty inventory, killing items that are not there).
"client" is an NBT path into the client player's own data, as the client
received it, matched with "expect" or "expect_error".
"state" is a dotted path into the state reply, compared with "equals".
"type" sends text as if typed on a keyboard.
"save" queues a save of the hosted world, as Save Game does; follow it with a wait.
"key" presses a key or mouse button by name, holds it for "ticks" (default 5) game ticks and releases it.
"mods" is the loaded mods as the "mods" request reports them (folder, id, name, version, state, error, commands), as JSON text matched with "expect".
"""
import json
import re
from pathlib import Path

import protocol

SCENARIO_DIR = Path(__file__).resolve().parent / "scenarios"
ERROR_PREFIX = "§c"


class ScenarioError(Exception):
    """A scenario file is missing or does not follow the format."""


def names():
    return sorted(path.stem for path in SCENARIO_DIR.glob("*.json"))


def load(name):
    path = SCENARIO_DIR / ("%s.json" % name)
    if not path.is_file():
        raise ScenarioError("no scenario %r; known: %s" % (name, ", ".join(names()) or "none"))
    try:
        scenario = json.loads(path.read_text(encoding="utf-8"))
    except ValueError as error:
        raise ScenarioError("%s is not valid JSON: %s" % (path, error))
    if not isinstance(scenario.get("steps"), list) or not scenario["steps"]:
        raise ScenarioError("%s has no steps" % path)
    world = scenario.get("world", {})
    if not isinstance(world, dict):
        raise ScenarioError("%s: world must be an object" % path)
    return scenario


def mods_directory(scenario):
    """A folder with the example mods the scenario names (and the library mod they depend on), or None: a scenario that names "mods" runs with only those."""
    import shutil
    import tempfile
    mods = scenario.get("mods")
    if not mods:
        return None
    source = Path(__file__).resolve().parents[2] / "docs" / "examples" / "mods"
    folder = tempfile.mkdtemp(prefix="lce-scenario-mods-")
    for name in list(mods) + (["lcestd"] if "lcestd" not in mods else []):
        shutil.copytree(source / name, Path(folder) / name)
    return folder


def world_arguments(scenario):
    world = scenario.get("world", {})
    return str(world.get("seed", "12345")), world.get("mode", "creative")


def _state_value(state, path):
    value = state
    for part in path.split("."):
        if not isinstance(value, dict) or part not in value:
            return None
        value = value[part]
    return value


def _check_command(connection, step):
    lines = [entry["text"] for entry in connection.request("command", line=step["cmd"])["lines"]]
    text = "\n".join(lines)
    failed = any(line.startswith(ERROR_PREFIX) for line in lines)
    if "expect_error" in step:
        if not failed:
            return False, "expected an error, got: %s" % (text or "no reply")
        if not re.search(step["expect_error"], text):
            return False, "error did not match %r: %s" % (step["expect_error"], text)
        return True, text
    if failed and step.get("allow_error"):
        return True, text
    if failed:
        return False, text
    if "expect" in step and not re.search(step["expect"], text):
        return False, "reply did not match %r: %s" % (step["expect"], text or "no reply")
    return True, text


def _run_step(connection, step):
    if "cmd" in step:
        return _check_command(connection, step)
    if "wait" in step:
        result = connection.request("wait", count=int(step["wait"]), unit=step.get("unit", "ticks"))
        return True, "reached %d" % result["reached"]
    if "hold" in step:
        connection.request("hold", hold=1 if step["hold"] else 0)
        return True, "held" if step["hold"] else "released"
    if "step" in step:
        result = connection.request("step", count=int(step["step"]))
        return True, "stepped to %d" % result["reached"]
    if "client" in step:
        try:
            text = connection.request("client-nbt", path=step["client"])["nbt"]
        except protocol.ProtocolError as error:
            text = None
            if "expect_error" in step and re.search(step["expect_error"], str(error)):
                return True, str(error)
            return False, str(error)
        if "expect_error" in step:
            return False, "expected an error, got: %s" % text
        if "expect" in step and not re.search(step["expect"], text):
            return False, "client data did not match %r: %s" % (step["expect"], text)
        return True, text
    if "save" in step:
        connection.request("save")
        return True, "save queued"
    if "type" in step:
        connection.request("type", text=step["type"])
        return True, "typed %s" % step["type"]
    if "key" in step:
        connection.request("key", name=step["key"], down=1)
        connection.request("wait", count=int(step.get("ticks", 5)), unit="ticks")
        connection.request("key", name=step["key"], down=0)
        return True, "pressed %s" % step["key"]
    if "mods" in step:
        text = json.dumps(connection.request("mods", action="list")["mods"], sort_keys=True)
        if not re.search(step["expect"], text):
            return False, "mods did not match %r: %s" % (step["expect"], text)
        return True, text
    if "state" in step:
        value = _state_value(connection.request("state"), step["state"])
        if value != step.get("equals"):
            return False, "%s is %r, expected %r" % (step["state"], value, step.get("equals"))
        return True, "%s is %r" % (step["state"], value)
    raise ScenarioError("unknown step %r" % step)


def _label(step):
    for key in ("cmd", "wait", "hold", "step", "client", "state", "mods", "key", "type", "save"):
        if key in step:
            return "%s %s" % (key, step[key])
    return repr(step)


def run(name, port=protocol.DEFAULT_PORT, report=print):
    scenario = load(name)
    passed = True
    with protocol.connect(port, timeout=None) as connection:
        for index, step in enumerate(scenario["steps"], 1):
            try:
                ok, detail = _run_step(connection, step)
            except protocol.ProtocolError as error:
                ok, detail = False, str(error)
            report("%s %2d %s" % ("ok  " if ok else "FAIL", index, _label(step)))
            if not ok:
                report("        %s" % detail)
                passed = False
        connection.request("hold", hold=0)
    return passed
