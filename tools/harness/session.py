"""Boot, keep and stop one debug client.

The session is what stays between harness commands: a running game, its log,
the X server this tool started (if any), and the pid file that names them.
Every other tool asks this module instead of starting or stopping a game.
"""
import json
import os
import signal
import socket
import subprocess
import sys
import tempfile
import time
from pathlib import Path

import protocol

ROOT = Path(__file__).resolve().parents[2]
# LCE_HARNESS_BINARY runs another build of the game, such as one with sanitizers
BINARY = Path(os.environ["LCE_HARNESS_BINARY"]) if os.environ.get("LCE_HARNESS_BINARY") else ROOT / "build" / "pc-debug" / "LCEReborn"
STATE_DIR = Path(tempfile.gettempdir()) / "lcereborn-harness"
HEADLESS_SIZE = "1280x720x24"
HEADLESS_FRAME_RATE = 10


class SessionError(Exception):
    """The session could not be started, reached or stopped."""


class SessionBusy(SessionError):
    """A client is already up for a different world; it is left running."""


def state_path(port):
    return STATE_DIR / ("%d.json" % port)


def log_path(port):
    return STATE_DIR / ("%d.log" % port)


def _read_state(port):
    path = state_path(port)
    if not path.is_file():
        return None
    try:
        with path.open("r", encoding="utf-8") as handle:
            return json.load(handle)
    except (OSError, ValueError):
        return None


def _write_state(port, state):
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    with state_path(port).open("w", encoding="utf-8") as handle:
        json.dump(state, handle)


def _process_alive(pid):
    if not pid:
        return False
    try:
        # a child that has exited stays signalable until it is reaped
        if os.waitpid(pid, os.WNOHANG)[0] == pid:
            return False
    except ChildProcessError:
        pass
    try:
        os.kill(pid, 0)
    except OSError:
        return False
    return True


def _wait_for_exit(pid, timeout):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if not _process_alive(pid):
            return True
        time.sleep(0.1)
    return not _process_alive(pid)


def _signal(pid, number):
    try:
        os.kill(pid, number)
    except OSError:
        pass


def _free_display():
    for number in range(90, 200):
        if not Path("/tmp/.X11-unix/X%d" % number).exists():
            return number
    raise SessionError("no free X display between :90 and :199")


def _start_xvfb():
    number = _free_display()
    process = subprocess.Popen(
        ["Xvfb", ":%d" % number, "-screen", "0", HEADLESS_SIZE, "-ac", "-terminate", "-nolisten", "tcp"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        start_new_session=True,
    )
    socket_path = Path("/tmp/.X11-unix/X%d" % number)
    deadline = time.monotonic() + 10.0
    while time.monotonic() < deadline:
        if socket_path.exists():
            return process, number
        if process.poll() is not None:
            raise SessionError("Xvfb exited before it was ready")
        time.sleep(0.1)
    _stop_process(process.pid)
    raise SessionError("Xvfb did not create display :%d" % number)


def _stop_process(pid):
    if not _process_alive(pid):
        return
    _signal(pid, signal.SIGTERM)
    if _wait_for_exit(pid, 5.0):
        return
    _signal(pid, signal.SIGKILL)
    _wait_for_exit(pid, 5.0)


def _reachable(port, timeout=1.0):
    try:
        connection = protocol.connect(port, timeout)
    except OSError:
        return False
    connection.close()
    return True


def pause(port=protocol.DEFAULT_PORT):
    state = _require_running(port)
    if not state.get("paused"):
        _signal(state["pid"], signal.SIGSTOP)
        state["paused"] = True
        _write_state(port, state)
    return state


def resume(port=protocol.DEFAULT_PORT):
    state = session_state(port)
    if state.get("paused"):
        _signal(state["pid"], signal.SIGCONT)
        state["paused"] = False
        _write_state(port, state)
    return state


def stop(port=protocol.DEFAULT_PORT):
    state = _read_state(port)
    if state is None:
        return False
    pid = state.get("pid")
    if state.get("paused"):
        resume(port)
    if _reachable(port):
        try:
            connection = protocol.connect(port)
            connection.request("quit")
            connection.close()
        except (OSError, protocol.ProtocolError):
            pass
    if pid and not _wait_for_exit(pid, 90.0):
        _signal(pid, signal.SIGTERM)
        if not _wait_for_exit(pid, 20.0):
            _signal(pid, signal.SIGKILL)
            _wait_for_exit(pid, 5.0)
            print("warning: the game did not stop and was killed; a save may be incomplete", file=sys.stderr)
    _stop_process(state.get("xvfbPid"))
    state_path(port).unlink(missing_ok=True)
    return True


def start(port=protocol.DEFAULT_PORT, world="", headless=None, idle=120, username="harness", binary=None,
          ui_mode="", seed="", game_mode="", frame_rate=None, launch_args=(), sound=False, size=""):
    running = _read_state(port)
    if running is not None:
        if running.get("paused") and _process_alive(running.get("pid")):
            running = resume(port)
        if _reachable(port):
            if world and running.get("world") != world:
                raise SessionBusy(
                    "the client on port %d was started with world %r, not %r; run harness.py down first"
                    % (port, running.get("world") or "", world)
                )
            return running
        stop(port)

    if headless is None:
        headless = True

    executable = Path(binary) if binary else BINARY
    if not executable.is_file():
        raise SessionError(
            "%s is missing; build the debug client with\n"
            "  cmake -S . -B build/pc-debug -DCMAKE_BUILD_TYPE=RelWithDebInfo\n"
            "  cmake --build build/pc-debug --target LCEReborn" % executable
        )

    environment = dict(os.environ)
    command = [
        str(executable),
        "--username",
        username,
        "--debug-port",
        str(port),
        "--debug-idle-quit",
        str(idle),
        "--skip-intro",
        "on",
    ]
    if world:
        command += ["--world", world]
    if seed:
        command += ["--seed", seed]
    if game_mode:
        command += ["--game-mode", game_mode]
    if ui_mode:
        command += ["--ui-mode", ui_mode]
    command += list(launch_args or [])

    xvfb = None
    display = environment.get("DISPLAY")
    if frame_rate is None:
        frame_rate = HEADLESS_FRAME_RATE if headless else 0
    if frame_rate:
        command += ["--frame-rate", str(frame_rate)]
    if size:
        width, _, height = size.partition("x")
        if not (width.isdigit() and height.isdigit()):
            raise SessionError("--size is WIDTHxHEIGHT, not %r" % size)
        command += ["--render-size", "%s,%s" % (width, height)]

    if headless:
        if not sound:
            environment["SDL_AUDIODRIVER"] = "dummy"
        xvfb, number = _start_xvfb()
        display = ":%d" % number
        environment["DISPLAY"] = display
        environment["SDL_VIDEODRIVER"] = "x11"
        environment["LIBGL_ALWAYS_SOFTWARE"] = "1"

    STATE_DIR.mkdir(parents=True, exist_ok=True)
    log = log_path(port).open("wb")
    process = subprocess.Popen(
        command,
        cwd=str(ROOT),
        env=environment,
        stdout=log,
        stderr=subprocess.STDOUT,
        start_new_session=True,
    )
    state = {
        "pid": process.pid,
        "xvfbPid": xvfb.pid if xvfb else None,
        "display": display,
        "port": port,
        "log": str(log_path(port)),
        "world": world,
        "seed": seed,
        "gameMode": game_mode,
        "idle": idle,
        "headless": headless,
        "frameRate": frame_rate,
        "paused": False,
    }
    _write_state(port, state)

    deadline = time.monotonic() + 90.0
    while time.monotonic() < deadline:
        if process.poll() is not None:
            _write_state(port, state)
            stop(port)
            raise SessionError("the game exited during boot; see %s" % log_path(port))
        if _reachable(port):
            return state
        time.sleep(0.25)

    stop(port)
    raise SessionError("the debug protocol did not answer within 90 seconds; see %s" % log_path(port))


def _last_error(port):
    try:
        lines = log_path(port).read_text(errors="replace").splitlines()
    except OSError:
        return ""
    for line in reversed(lines):
        if "/ERROR]" in line or "/FATAL]" in line:
            return line.split("]: ", 1)[-1]
    return ""


def wait_in_world(port=protocol.DEFAULT_PORT, timeout=120.0):
    deadline = time.monotonic() + timeout
    pid = (_read_state(port) or {}).get("pid")
    while time.monotonic() < deadline:
        if pid and not _process_alive(pid):
            reason = _last_error(port) or "see %s" % log_path(port)
            raise SessionError("the game exited before the world loaded: %s" % reason)
        try:
            with protocol.connect(port) as connection:
                state = connection.request("state")
        except OSError:
            time.sleep(0.25)
            continue
        if state.get("inWorld") and not state.get("menuDisplayed"):
            return state
        if state.get("eula"):
            for down in (1, 0):
                with protocol.connect(port) as connection:
                    connection.request("key", name="Return", down=down)
            time.sleep(0.5)
        time.sleep(0.25)
    raise SessionError("no world loaded within %.0f seconds" % timeout)


def session_state(port=protocol.DEFAULT_PORT):
    state = _read_state(port)
    if state is None:
        raise SessionError("no session on port %d; run harness.py up first" % port)
    return state


def _require_running(port):
    state = _read_state(port)
    if state is None or not _process_alive(state.get("pid")) or not _reachable(port):
        raise SessionError("no session on port %d; run harness.py up first" % port)
    return state


def require(port=protocol.DEFAULT_PORT):
    state = _require_running(port)
    if state.get("paused"):
        raise SessionError("the client on port %d is paused; run harness.py resume first" % port)
    return state
