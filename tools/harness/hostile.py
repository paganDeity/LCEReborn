#!/usr/bin/env python3
"""Send a dedicated server the packets a hostile client would, and check that it refuses them and stays up.

Each case starts a debug client, joins a dedicated server started for the run, sends chosen packet bytes on the
client's own connection (harness.py rawpacket), and then checks that the server cut that client off and still
answers its console.
"""

import argparse
import os
import shutil
import struct
import subprocess
import sys
import tempfile
import time
from pathlib import Path

import protocol
import session

ROOT = Path(__file__).resolve().parents[2]
SERVER = ROOT / "build" / "pc-debug" / "LCERebornServer"


def utf(text):
    return struct.pack(">h", len(text)) + text.encode("utf-16-be")


def int_array_tag():
    # a compound holding one int array that says it has 2^31 - 1 entries and carries none
    return b"\x0a" + struct.pack(">h", 0) + b"\x0b" + struct.pack(">h", 1) + b"A" + struct.pack(">i", 0x7FFFFFFF) + b"\x00"


def creative_slot_with_tag():
    tag = int_array_tag()
    return bytes([107]) + struct.pack(">hhbh", 0, 1, 1, 0) + struct.pack(">h", len(tag)) + tag


# what the server logs when it cuts a client off for each kind of case
UNREADABLE = "it sent a packet the server could not read"

CASES = [
    ("craft-count", "a crafting packet that says it carries 2^31 items",
     bytes([150]) + struct.pack(">hbbi", 0, 0, 0, 0x7FFFFFFF), 1, UNREADABLE),
    ("long-chat", "a chat message that says it is 1024 characters, over the 256 a chat packet may hold",
     bytes([222]) + struct.pack(">h", 1024) + ("a" * 1024).encode("utf-16-be"), 1, UNREADABLE),
    ("nbt-array", "an item tag with an int array that says it has 2^31 entries",
     creative_slot_with_tag(), 1, UNREADABLE),
    ("flood", "keep alive packets far over max-packets-per-second",
     bytes([226, 1]), 20000, "packets in 5 seconds"),
]


class Server:
    def __init__(self, port, folder, properties=""):
        self.folder = folder
        (folder / "server.properties").write_text("port=%d\nmax-packets-per-second=500\nlan-visible=false\n%s" % (port, properties))
        self.log = open(folder / "console.txt", "wb")
        self.process = subprocess.Popen(
            [str(SERVER), "--save-dir", str(folder), "--log-dir", str(folder), "--mods-dir", str(folder / "mods")],
            cwd=str(ROOT), stdin=subprocess.PIPE, stdout=self.log, stderr=subprocess.STDOUT)
        self.wait_for("Ready for players", 120)

    def text(self):
        return (self.folder / "console.txt").read_text(errors="replace")

    def wait_for(self, words, seconds, start=0):
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            if words in self.text()[start:]:
                return True
            if self.process.poll() is not None:
                raise SystemExit("the server stopped:\n" + self.text()[-2000:])
            time.sleep(0.25)
        return False

    def command(self, line):
        self.process.stdin.write((line + "\n").encode())
        self.process.stdin.flush()

    def answers(self):
        start = len(self.text())
        self.command("list")
        return self.wait_for("players online", 10, start)

    def stop(self):
        if self.process.poll() is None:
            self.command("stop")
            try:
                self.process.wait(30)
            except subprocess.TimeoutExpired:
                self.process.kill()
        self.log.close()


def in_world(port):
    with protocol.connect(port) as connection:
        return bool(connection.request("state").get("inWorld"))


def join(client_port, server_port, identities, extra=()):
    session.stop(client_port)
    session.start(port=client_port, launch_args=["--join", "127.0.0.1:%d" % server_port, "--identity-dir", str(identities),
                                                 "--mods-dir", str(identities / "mods")] + list(extra))
    deadline = time.monotonic() + 60
    while time.monotonic() < deadline:
        if in_world(client_port):
            return True
        time.sleep(1)
    return False


def run_case(server, packet, count, logged, client_port, server_port, identities):
    if not join(client_port, server_port, identities):
        return "the client did not join"
    start = len(server.text())
    with protocol.connect(client_port) as connection:
        connection.request("rawpacket", hex=packet.hex(), count=count)
    deadline = time.monotonic() + 20
    while time.monotonic() < deadline and in_world(client_port):
        time.sleep(0.5)
    if in_world(client_port):
        return "the server kept the client"
    if not server.answers():
        return "the server stopped answering its console"
    after = server.text()[start:]
    if logged not in after:
        return "the server did not log why it cut the client off (%r)" % logged
    if "left the game" not in after:
        return "the server did not end the client's session"
    return None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("cases", nargs="*", help="the cases to run (default: all)")
    parser.add_argument("--port", type=int, default=25581, help="the debug client's harness port")
    parser.add_argument("--server-port", type=int, default=25590, help="the dedicated server's game port")
    parser.add_argument("--keep", action="store_true", help="keep the server's folder")
    args = parser.parse_args()
    if not SERVER.is_file():
        print("%s is missing; build the debug programs first" % SERVER)
        return 1
    chosen = [case for case in CASES if not args.cases or case[0] in args.cases]
    folder = Path(tempfile.mkdtemp(prefix="lce-hostile-"))
    (folder / "mods").mkdir()
    identities = folder / "client"
    (identities / "mods").mkdir(parents=True)
    server = Server(args.server_port, folder)
    failed = 0
    try:
        for name, description, packet, count, logged in chosen:
            problem = run_case(server, packet, count, logged, args.port, args.server_port, identities)
            print("%-12s %s: %s" % (name, "ok" if problem is None else "FAIL", description if problem is None else problem))
            failed += problem is not None
    finally:
        session.stop(args.port)
        server.stop()
        if args.keep:
            print("server folder: %s" % folder)
        else:
            shutil.rmtree(folder, ignore_errors=True)
    print("%d of %d passed" % (len(chosen) - failed, len(chosen)))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
