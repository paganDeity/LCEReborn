#!/usr/bin/env python3
"""Join one dedicated server with a Reborn client and a Legacy client at once, and check that both play.

The server takes both protocols (protocols=reborn,legacy). The check reads what the server agreed with the Reborn
client from its log, moves both players with the server's tp, and reads each client's own position back.
"""

import argparse
import shutil
import sys
import tempfile
import time
from pathlib import Path

import protocol
import session
from hostile import SERVER, Server, in_world, join

# what the server logs for a Reborn client whose hello agreed batching (1), long chat (4) and zstd (16)
AGREED = "a Reborn client, protocol version 1, features 21"
# longer than the 256 a Legacy chat packet carries, shorter than the 1024 a Reborn one does
LONG_MESSAGE = ("".join("word%03d " % index for index in range(75))).strip()


def position(port):
    with protocol.connect(port) as connection:
        player = connection.request("state").get("player") or {}
    return player.get("x"), player.get("z")


def press(connection, key):
    connection.request("key", name=key, down=1)
    time.sleep(0.05)
    connection.request("key", name=key, down=0)


def chat(port, text):
    with protocol.connect(port) as connection:
        press(connection, "t")
        time.sleep(1)
        connection.request("type", text=text)
        time.sleep(0.5)
        press(connection, "Return")


def moved_to(port, x, z, seconds):
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        px, pz = position(port)
        if px is not None and abs(px - x) < 1.5 and abs(pz - z) < 1.5:
            return True
        time.sleep(0.5)
    return False


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reborn-port", type=int, default=25581, help="the Reborn client's harness port")
    parser.add_argument("--legacy-port", type=int, default=25582, help="the Legacy client's harness port")
    parser.add_argument("--server-port", type=int, default=25591, help="the dedicated server's game port")
    parser.add_argument("--keep", action="store_true", help="keep the server's folder")
    args = parser.parse_args()
    if not SERVER.is_file():
        print("%s is missing; build the debug programs first" % SERVER)
        return 1
    folder = Path(tempfile.mkdtemp(prefix="lce-protocols-"))
    (folder / "mods").mkdir()
    clients = {}
    for name in ("reborn", "legacy"):
        clients[name] = folder / name
        (clients[name] / "mods").mkdir(parents=True)
    server = Server(args.server_port, folder, "protocols=reborn,legacy\n")
    problems = []
    try:
        if not join(args.reborn_port, args.server_port, clients["reborn"], ["--protocol", "reborn", "--username", "reborn"]):
            problems.append("the Reborn client did not join")
        elif AGREED not in server.text():
            problems.append("the server did not agree batching and zstd with the Reborn client")
        if not join(args.legacy_port, args.server_port, clients["legacy"], ["--username", "legacy"]):
            problems.append("the Legacy client did not join")
        # a fall from the teleport would kill a survival player, and a dead one is not moved
        server.command("gamemode creative @a")
        for x, z in ((40.5, -60.5), (-75.5, 90.5)):
            server.command("tp @a %s 120 %s" % (x, z))
            for name, port in (("Reborn", args.reborn_port), ("Legacy", args.legacy_port)):
                if not moved_to(port, x, z, 20):
                    problems.append("the %s client was not moved to %s %s" % (name, x, z))
        # the server writes a chat line longer than the 123 characters its chat packet's field says
        server.command("say " + "long line " * 25)
        time.sleep(3)
        for name, port in (("Reborn", args.reborn_port), ("Legacy", args.legacy_port)):
            if not in_world(port):
                problems.append("the %s client left after a 250 character chat line" % name)
        # what a player typed reaches the server whole over Reborn, and cut to what its packet carries over Legacy
        chat(args.reborn_port, LONG_MESSAGE)
        chat(args.legacy_port, LONG_MESSAGE)
        time.sleep(3)
        text = server.text()
        if "<reborn> " + LONG_MESSAGE + "\n" not in text:
            problems.append("the Reborn client's %d character message did not arrive whole" % len(LONG_MESSAGE))
        if "<legacy> " + LONG_MESSAGE[:256].rstrip() + "\n" not in text:
            problems.append("the Legacy client's message was not cut to 256 characters")
        if not server.answers():
            problems.append("the server stopped answering its console")
    finally:
        session.stop(args.reborn_port)
        session.stop(args.legacy_port)
        server.stop()
        if args.keep:
            print("server folder: %s" % folder)
        else:
            shutil.rmtree(folder, ignore_errors=True)
    for problem in problems:
        print("FAIL " + problem)
    print("ok   a Reborn client with batches and a Legacy client play on one server" if not problems else "FAILED")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
