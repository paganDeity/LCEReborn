#!/usr/bin/env python3
"""Drive an LCEReborn debug client that stays up between commands.

    harness.py up [--world FOLDER] [--seed TEXT] [--mode MODE] [--headless] [--idle SECONDS]
    harness.py down
    harness.py pause
    harness.py resume
    harness.py state
    harness.py cmd "<line>" ...
    harness.py console [--since SEQ]
    harness.py shot <path>
    harness.py key <name> <down|up>
    harness.py type "<text>"
    harness.py screen [name]
    harness.py ask "<text>" [--option TEXT ...]
    harness.py answers [id] [--wait] [--timeout SECONDS]
    harness.py wait <count> [--frames]
    harness.py hold | release
    harness.py step <ticks>
    harness.py check [name ...] [--list]
    harness.py profile [--seconds N] [--json]

`up` starts the debug build and returns once it answers; the game keeps
running for the next command. It quits itself after --idle seconds without a
request, so a forgotten session does not hold the CPU. `pause` stops the
client at zero CPU, keeping its world in memory, and `resume` or `up`
continues it; stopped time does not count as idle.
"""
import argparse
import json
import shutil
import sys
import time
from pathlib import Path

import protocol
import scenario
import session

DEFAULT_SEED = "12345"
PROFILE_INTERVAL = 0.5
ANSWER_POLL_INTERVAL = 0.5
DEFAULT_MODE = "creative"


def report_error(message):
    print("error: %s" % message, file=sys.stderr)
    return 1


def state_text(state):
    lines = []
    lines.append("in world: %s" % ("yes" if state.get("inWorld") else "no"))
    lines.append("ticks: %d" % state.get("ticks", 0))
    lines.append("frame rate: %.1f" % state.get("frameRate", 0.0))
    player = state.get("player")
    if player:
        lines.append(
            "player: %.3f %.3f %.3f yaw %.1f pitch %.1f (%s)"
            % (player["x"], player["y"], player["z"], player["yaw"], player["pitch"], player["dimension"])
        )
    level = state.get("level")
    if level:
        lines.append(
            "level: %s (%s) day %d %s%s"
            % (
                level["name"],
                level["seed"],
                level["dayTime"],
                "raining" if level["raining"] else "clear",
                ", thundering" if level["thundering"] else "",
            )
        )
    return "\n".join(lines)


def generated_world(seed, mode):
    return "harness-%s-%s" % (seed, mode)


def bring_up(port, world, seed, mode, headless=None, frame_rate=None, **options):
    try:
        state = session.start(
            port=port, world=world, seed=seed, game_mode=mode, headless=headless, frame_rate=frame_rate, **options
        )
        if world:
            return session.wait_in_world(port)
        return state
    except session.SessionBusy:
        raise
    except session.SessionError:
        session.stop(port)
        raise


def command_check(args):
    if args.list:
        for name in scenario.names():
            print("%-24s %s" % (name, scenario.load(name).get("description", "")))
        return 0
    chosen = args.names or scenario.names()
    failed = []
    for name in chosen:
        loaded = scenario.load(name)
        seed, mode = scenario.world_arguments(loaded)
        print("== %s" % name)
        mods = scenario.mods_directory(loaded)
        # a scenario runs with the hints off, so a hint popping up never takes a key or a click meant for the game
        launch = ["--hints", "off"] + (["--mods-dir=%s" % mods] if mods else [])
        session.stop(args.port)  # the mods and the hints are chosen when the client starts
        try:
            bring_up(args.port, generated_world(seed, mode), seed, mode, launch_args=launch)
        except session.SessionBusy:
            print("   restarting the client in %s" % generated_world(seed, mode))
            session.stop(args.port)
            bring_up(args.port, generated_world(seed, mode), seed, mode, launch_args=launch)
        # the scenarios share worlds that are never saved; an autosave in the middle of a long one would keep its state for the next
        with protocol.connect(args.port) as connection:
            connection.request("command", line="save-off")
        if not scenario.run(name, args.port):
            failed.append(name)
        if mods:
            # the client keeps reading its mods folder until it stops, so the copy goes once the scenario is done with it
            session.stop(args.port)
            shutil.rmtree(mods, ignore_errors=True)
    print("%d of %d passed" % (len(chosen) - len(failed), len(chosen)))
    return 1 if failed else 0


def command_up(args):
    if not args.world and (args.seed or args.mode):
        args.world = generated_world(args.seed or DEFAULT_SEED, args.mode or DEFAULT_MODE)
        args.seed = args.seed or DEFAULT_SEED
        args.mode = args.mode or DEFAULT_MODE
    state = bring_up(
        args.port,
        args.world,
        args.seed,
        args.mode,
        headless=args.headless,
        frame_rate=args.frame_rate,
        size=args.size,
        idle=args.idle,
        username=args.username,
        ui_mode=args.ui_mode,
        launch_args=args.launch_arg,
        sound=args.sound,
    )
    if args.world:
        print(state_text(state))
    else:
        print("client on port %d, log %s" % (args.port, state["log"]))
    return 0


def command_down(args):
    if session.stop(args.port):
        print("stopped")
    else:
        print("no session on port %d" % args.port)
    return 0


def command_pause(args):
    session.pause(args.port)
    print("paused")
    return 0


def command_resume(args):
    session.resume(args.port)
    print("resumed")
    return 0


def command_wait(args):
    session.require(args.port)
    with protocol.connect(args.port, timeout=None) as connection:
        result = connection.request("wait", count=args.count, unit="frames" if args.frames else "ticks")
    print("at %s %d" % ("frame" if args.frames else "server tick", result["reached"]))
    return 0


def command_hold(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        connection.request("hold", hold=1 if args.hold else 0)
    print("server ticks held" if args.hold else "server ticks released")
    return 0


def command_step(args):
    session.require(args.port)
    with protocol.connect(args.port, timeout=None) as connection:
        connection.request("step", count=args.count)
        state = connection.request("state")
    print("stepped %d, at server tick %d" % (args.count, state.get("serverTicks", 0)))
    return 0


def profile_samples(state):
    profile = state.get("profile", {})
    samples = {"frame rate": state.get("frameRate", 0.0), "server tick ms": profile.get("serverTickMs", 0.0)}
    for name, value in profile.get("sectionsMs", {}).items():
        samples["%s ms" % name] = value
    for name, value in profile.get("gpuSectionsMs", {}).items():
        if value is not None:
            samples["%s gpu ms" % name] = value
    if profile.get("residentBytes") is not None:
        samples["resident MiB"] = profile["residentBytes"] / (1024.0 * 1024.0)
    return samples


def command_profile(args):
    session.require(args.port)
    series = {}
    deadline = time.monotonic() + args.seconds
    with protocol.connect(args.port) as connection:
        while True:
            for name, value in profile_samples(connection.request("state")).items():
                series.setdefault(name, []).append(value)
            if time.monotonic() >= deadline:
                break
            time.sleep(PROFILE_INTERVAL)
    summary = {
        name: {"average": sum(values) / len(values), "min": min(values), "max": max(values)}
        for name, values in series.items()
    }
    if args.json:
        print(json.dumps({"seconds": args.seconds, "samples": len(series["frame rate"]), "metrics": summary}, indent=2))
        return 0
    print("%d samples over %.0f seconds" % (len(series["frame rate"]), args.seconds))
    print("%-26s %10s %10s %10s" % ("", "average", "min", "max"))
    for name, values in summary.items():
        print("%-26s %10.2f %10.2f %10.2f" % (name, values["average"], values["min"], values["max"]))
    return 0


def task_text(task):
    lines = ["%d. %s" % (task["id"], task["text"])]
    for index, option in enumerate(task["options"], 1):
        lines.append("   %d) %s" % (index, option))
    lines.append("   answer: %s" % (task["answer"] if task["answer"] is not None else "(waiting)"))
    return "\n".join(lines)


def command_ask(args):
    state = session.require(args.port)
    if state.get("headless"):
        print("note: this client is headless, so no person can see the request", file=sys.stderr)
    with protocol.connect(args.port) as connection:
        task = connection.request("ask", text=args.text, options=args.option)["id"]
    print("request %d sent; read the answer with harness.py answers %d --wait" % (task, task))
    return 0


def command_p2p(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        result = connection.request("p2p", action=args.action, text=args.text, nickname=args.nickname)
    result.pop("id", None)
    print(json.dumps(result, indent=1))
    return 0


def command_identity(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        result = connection.request("identity", action=args.action, text=args.text, other=args.other)
    result.pop("id", None)
    print(json.dumps(result, indent=1))
    return 0


def command_save(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        result = connection.request("save")
    result.pop("id", None)
    print(json.dumps(result))
    return 0


def command_mods(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        result = connection.request("mods", action=args.action)
    result.pop("id", None)
    print(json.dumps(result, indent=1))
    return 0


def command_chunkstats(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        result = connection.request("chunkstats", reset=1 if args.reset else 0)
    result.pop("id", None)
    print(json.dumps(result, indent=1))
    return 0


def command_rawpacket(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        result = connection.request("rawpacket", hex=args.hex, count=args.count)
    print("queued %d" % result.get("queued", 0))
    return 0


def command_answers(args):
    session.require(args.port)
    deadline = time.monotonic() + args.timeout if args.timeout > 0 else None
    while True:
        with protocol.connect(args.port) as connection:
            tasks = connection.request("state").get("tasks", [])
        if args.id:
            tasks = [task for task in tasks if task["id"] == args.id]
            if not tasks:
                raise session.SessionError("there is no request %d" % args.id)
        waiting = [task for task in tasks if task["answer"] is None]
        if not args.wait or not waiting:
            break
        if deadline is not None and time.monotonic() >= deadline:
            print("no answer within %d seconds" % args.timeout)
            return 3
        time.sleep(ANSWER_POLL_INTERVAL)
    for task in tasks:
        print(task_text(task))
    if not tasks:
        print("no requests")
    return 0


def command_state(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        state = connection.request("state")
    if args.json:
        print(json.dumps(state, indent=2))
    else:
        print(state_text(state))
    return 0


def command_cmd(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        for line in args.lines:
            result = connection.request("command", line=line)
            for entry in result["lines"]:
                print(entry["text"])
    return 0


def command_console(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        result = connection.request("console", since=args.since)
    for entry in result["lines"]:
        print("%5d  %s" % (entry["seq"], entry["text"]))
    print("next: %d" % result["next"])
    return 0


def command_shot(args):
    session.require(args.port)
    path = Path(args.path).expanduser().resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    with protocol.connect(args.port) as connection:
        result = connection.request("capture", path=str(path))
    print("wrote %s (%dx%d)" % (result["path"], result["width"], result["height"]))
    return 0


def command_key(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        connection.request("key", name=args.name, down=1 if args.direction == "down" else 0)
    return 0


def command_pointer(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        connection.request("pointer", x=args.x, y=args.y)
    return 0


def scene_text(state):
    scenes = state.get("scenes", [])
    if not scenes:
        return "no scene"
    return "\n".join("%-10s %-10s %s" % (scene["group"], scene["layer"], scene["name"]) for scene in scenes)


def command_type(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        connection.request("type", text=args.text)
    return 0


def command_screen(args):
    session.require(args.port)
    with protocol.connect(args.port) as connection:
        if args.name:
            connection.request("screen", name=args.name)
            return 0
        print(scene_text(connection.request("state")))
    return 0


def command_touch(args):
    session.require(args.port)
    phases = {"down": 0, "move": 1, "up": 2, "cancel": 3}
    with protocol.connect(args.port) as connection:
        connection.request("touch", id=args.id, x=args.x, y=args.y, phase=phases[args.phase])
    return 0


def build_parser():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--port", type=int, default=protocol.DEFAULT_PORT, help="debug protocol port")
    subparsers = parser.add_subparsers(dest="command", required=True)

    up = subparsers.add_parser("up", help="start or keep a debug client")
    up.add_argument("--world", default="", help="save folder in saves/games to load, made new if it has no save")
    up.add_argument("--seed", default="", help="seed for a new world (default with --mode: %s)" % DEFAULT_SEED)
    up.add_argument(
        "--mode",
        choices=("survival", "creative", "adventure"),
        default="",
        help="game mode for a new world (default with --seed: %s)" % DEFAULT_MODE,
    )
    up.add_argument("--ui-mode", choices=("legacy", "bedrock"), default="", help="interface mode for this run")
    up.add_argument("--headless", action="store_true", default=None, help="run on a private Xvfb display")
    up.add_argument("--windowed", action="store_true", help="run on the current display")
    up.add_argument("--sound", action="store_true", help="let a headless client play sound (it is silent by default)")
    up.add_argument(
        "--launch-arg",
        action="append",
        default=[],
        help="pass this argument to the game as it starts; repeat for more (for example --launch-arg=--p2p-dir=DIR)",
    )
    up.add_argument("--idle", type=int, default=120, help="seconds without a request before the game quits")
    up.add_argument("--username", default="harness", help="player name")
    up.add_argument(
        "--frame-rate",
        type=int,
        default=None,
        help="frames a second to draw (default: %d headless, 60 windowed)" % session.HEADLESS_FRAME_RATE,
    )
    up.add_argument(
        "--size",
        default="",
        help="WIDTHxHEIGHT to draw at, at least 320x180 (default: 1280x720); a smaller one costs a headless client's software renderer less",
    )
    up.set_defaults(function=command_up)

    down = subparsers.add_parser("down", help="stop the client")
    down.set_defaults(function=command_down)

    pause = subparsers.add_parser("pause", help="freeze the client at zero CPU")
    pause.set_defaults(function=command_pause)

    resume = subparsers.add_parser("resume", help="continue a paused client")
    resume.set_defaults(function=command_resume)

    state = subparsers.add_parser("state", help="print the client state")
    state.add_argument("--json", action="store_true", help="print the raw reply")
    state.set_defaults(function=command_state)

    cmd = subparsers.add_parser("cmd", help="run console commands and print their replies")
    cmd.add_argument("lines", nargs="+", help="command line, without the leading slash")
    cmd.set_defaults(function=command_cmd)

    console = subparsers.add_parser("console", help="print captured console output")
    console.add_argument("--since", type=int, default=0, help="first sequence number to print")
    console.set_defaults(function=command_console)

    shot = subparsers.add_parser("shot", help="save the current frame as a PNG")
    shot.add_argument("path", help="file to write")
    shot.set_defaults(function=command_shot)

    key = subparsers.add_parser("key", help="press or release a key")
    key.add_argument("name", help="key name, such as w or F3")
    key.add_argument("direction", choices=("down", "up"))
    key.set_defaults(function=command_key)

    pointer = subparsers.add_parser("pointer", help="move the mouse pointer, so a later MouseLeft clicks there")
    pointer.add_argument("x", type=float, help="from 0 (left edge) to 1 (right edge)")
    pointer.add_argument("y", type=float, help="from 0 (top edge) to 1 (bottom edge)")
    pointer.set_defaults(function=command_pointer)

    type_text = subparsers.add_parser("type", help="send text as if typed on a keyboard")
    type_text.add_argument("text")
    type_text.set_defaults(function=command_type)

    screen = subparsers.add_parser("screen", help="say what is on screen, or open a named screen")
    screen.add_argument("name", nargs="?", choices=("pause", "help", "controls", "settings", "skins", "main-menu", "shaders", "debug-overlay", "friends", "identities"))
    screen.set_defaults(function=command_screen)

    wait = subparsers.add_parser("wait", help="return after this many server ticks or frames have passed")
    wait.add_argument("count", type=int)
    wait.add_argument("--frames", action="store_true", help="count drawn frames instead of server ticks")
    wait.set_defaults(function=command_wait)

    hold = subparsers.add_parser("hold", help="stop server ticks; the client keeps drawing")
    hold.set_defaults(function=command_hold, hold=True)

    release = subparsers.add_parser("release", help="let held server ticks run again")
    release.set_defaults(function=command_hold, hold=False)

    step = subparsers.add_parser("step", help="run this many server ticks while they are held")
    step.add_argument("count", type=int)
    step.set_defaults(function=command_step)

    check = subparsers.add_parser("check", help="run named scenarios and report pass or fail")
    check.add_argument("names", nargs="*", help="scenarios to run (default: all)")
    check.add_argument("--list", action="store_true", help="list the scenarios")
    check.set_defaults(function=command_check)

    profile = subparsers.add_parser("profile", help="sample frame, section, server tick and memory figures")
    profile.add_argument("--seconds", type=float, default=10.0, help="how long to sample (default: 10)")
    profile.add_argument("--json", action="store_true", help="print the summary as JSON")
    profile.set_defaults(function=command_profile)

    ask = subparsers.add_parser("ask", help="send the user a request in the game chat")
    ask.add_argument("text", help="what to ask the user, at most 400 characters")
    ask.add_argument("--option", action="append", default=[], help="an answer to offer; repeat for more")
    ask.set_defaults(function=command_ask)

    p2p = subparsers.add_parser("p2p", help="drive peer-to-peer play: status, friends, lobbies, join")
    p2p.add_argument(
        "action", choices=("status", "add-friend", "friends", "lobbies", "open", "invite", "join"), help="what to do"
    )
    p2p.add_argument("text", nargs="?", default="", help="a friend code (add-friend) or a key (invite, join)")
    p2p.add_argument("--nickname", default="", help="the name to give a friend added by code")
    p2p.set_defaults(function=command_p2p)

    identity = subparsers.add_parser("identity", help="list and change identities: list, select, create, rename, remove, skin")
    identity.add_argument("action", choices=("list", "select", "create", "rename", "remove", "skin"))
    identity.add_argument("text", nargs="?", default="", help="an identity file name (a new name for create)")
    identity.add_argument("other", nargs="?", default="", help="the new name (rename) or a PNG file (skin)")
    identity.set_defaults(function=command_identity)

    save = subparsers.add_parser("save", help="save the hosted world now (the chunk and container timers then hold the numbers)")
    save.set_defaults(function=command_save)

    mods = subparsers.add_parser("mods", help="list the loaded mods, or reload them all")
    mods.add_argument("action", nargs="?", default="list", choices=("list", "reload"))
    mods.set_defaults(function=command_mods)

    chunkstats = subparsers.add_parser("chunkstats", help="show the chunk save and load timers")
    chunkstats.add_argument("--reset", action="store_true", help="zero the timers after reading them")
    chunkstats.set_defaults(function=command_chunkstats)
    rawpacket = subparsers.add_parser("rawpacket", help="send chosen packet bytes to the server on this client's connection")
    rawpacket.add_argument("hex", help="the packet number then its fields, as hex digits")
    rawpacket.add_argument("--count", type=int, default=1, help="send it this many times")
    rawpacket.set_defaults(function=command_rawpacket)

    answers = subparsers.add_parser("answers", help="show requests and their answers")
    answers.add_argument("id", type=int, nargs="?", default=0, help="only this request")
    answers.add_argument("--wait", action="store_true", help="wait until the shown requests are answered")
    answers.add_argument("--timeout", type=int, default=0, help="seconds to wait, 0 for no limit")
    answers.set_defaults(function=command_answers)

    touch = subparsers.add_parser("touch", help="send a finger to the touchscreen")
    touch.add_argument("x", type=float, help="horizontal position, 0 at the left edge and 1 at the right")
    touch.add_argument("y", type=float, help="vertical position, 0 at the top edge and 1 at the bottom")
    touch.add_argument("phase", choices=("down", "move", "up", "cancel"))
    touch.add_argument("--id", type=int, default=1, help="finger id, for more than one finger")
    touch.set_defaults(function=command_touch)
    return parser


def main(argv):
    parser = build_parser()
    args = parser.parse_args(argv)
    if getattr(args, "windowed", False):
        args.headless = False
    try:
        return args.function(args)
    except (session.SessionError, protocol.ProtocolError, scenario.ScenarioError, OSError) as error:
        return report_error(error)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
