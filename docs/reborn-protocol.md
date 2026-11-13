# The Reborn wire protocol

Design, with step 2 of the order built (2026-11-11): `source/reborn` (library `lce_reborn`, C++98,
test `lce_reborn_test`) has varints, frames, a frame reader that skips numbers it does not take, the
preamble check, the hello and the negotiation; `source/game/network/RebornCodec.cpp` puts them on
a connection (see "What is built"). Legacy stays frozen (`compatibility.md`); this is
the second protocol, incompatible on purpose, and it owns everything Legacy cannot do:
1024 players or more, chat and commands of 1024 characters, cross-version play, mod
content, and a lower cost per player.

## Choosing a protocol (decision)

A Reborn client opens the connection with a 4-byte preamble, `LCE` and `0xB1`, and the
protocol range it speaks. A Legacy first packet never starts with those bytes, so one
listening port serves both: the server peeks at the first four bytes and picks. A
Reborn client that gets no answer in two seconds, or a close, retries as Legacy when
its profile allows it; a server setting (`protocols=reborn,legacy`) says what it
accepts. A lobby record already names the protocol and its version, so Join Game can
mark a lobby the player's build cannot join, and a peer-to-peer link carries whichever
one the lobby names. This replaces the open question in `todo.md`.

## What the bytes look like

- Transport framing: `varint length`, `varint packet id`, payload. A reader that does
  not know an id skips `length` bytes, so an older build ignores what a newer one adds
  and never loses its place.
- Everything countable is a varint or a varlong (entity ids, player indexes, counts,
  lengths), so no field has a 255, 1024 or 65535 ceiling baked into it. Strings are
  `varint length` plus UTF-8, with a per-packet limit that is a number in the packet
  table, not a type: chat is 1024 characters, a command 1024 (more when a packet asks).
- Batching: the server packs one tick's packets for a connection into one frame, which
  the transport compresses with zstd when it is large enough to pay for it. Movement
  and entity updates inside it use deltas from the last acknowledged state, in
  quantised fixed point, and absolute values only on first sight or after a gap.
- NBT inside a packet is written with the capability set the two sides agreed, which is
  where `NbtCapabilities` stops being `kLegacy`.

## Negotiation

After the preamble each side sends a hello: protocol version range, a feature bit set
(batching, deltas, long chat, long arrays, zstd, resume), the registry hash and the
mod list (`modding.md`). The server answers with the version and the set both agreed.
A feature neither side agreed to is never sent. A client with mods it lacks is told
which ones and why it was refused before any world data is sent.

## What is built

Opt in by setting: `protocols` in `server.properties` (`legacy`, the default; `reborn,legacy`;
`reborn`), read by both servers, and `--protocol reborn` on a client (`pc-platform.md`). The host's
own player always uses Legacy, and a packet mod (`modding.md`) takes the place of either protocol.

- The client's first write is the preamble, a hello frame (number `0x100`) and its first packet
  frame. The hello carries the version range (1 to 1), no features yet, and the client's running
  mods as `id@version`; the registry hash is left empty, so registries are still matched by the
  `LCE|Registry` payload after login.
- The server's first read takes four bytes. If they are not the preamble they are handed back to
  the Legacy reader in front of the rest of the stream, and the connection is Legacy from then on;
  the first Legacy packet, `ServerboundPreLoginPacket`, is at least five bytes, so the four are
  always there. Anything the server writes before it has decided is held and sent in order after.
- A Reborn server answers with its own hello, or with a refusal frame (`0x101`: a disconnect reason
  and the reason in words). The client shows the reason the game already has a screen for
  (`MissingMods` for a mod it lacks, `ClientOutOfDate` for a version) and logs the words. A server
  set to `reborn` alone sends a Legacy client the game's own disconnect packet with
  `ClientOutOfDate`, in Legacy bytes, so it leaves the way a client of another version does.
- A packet frame is `varint length, varint number, the packet's Legacy fields`. A frame whose number
  the reader does not know in that direction is skipped by its length.
- Batches (step 3, 2026-11-12): both hellos offer batching, long chat and zstd (features 21). Once they agree,
  the frames a connection writes between two flushes of its output (one write burst) go out as one
  batch frame (`0x102`): `varint flags, varint size, the frames`, compressed with zstd at level 1
  when there are at least 512 bytes and the result is smaller (flag 1). A reader unpacks a batch
  and reads its frames as it reads a stream, skipping unknown numbers the same way; a batch inside
  a batch is skipped, an unknown flag, a size over the frame limit or data that does not unpack
  ends the connection. Writing waits for the agreement, so the opening is never batched. Code:
  `source/reborn/RbBatch`, `PacketFrames` in `RebornCodec.cpp`, `PacketCodec::Flush`.
  Measured on 2026-11-12 with `ss` on one server, a Reborn and a Legacy client side by side:
  10 percent fewer bytes over 30 idle seconds (74,655 against 82,809) and 5 percent fewer over
  three teleports with chunk loading (1,906,220 against 2,011,615); chunk payloads are already
  compressed by the game, so the rest waits on varint fields and deltas.
- Long chat (step 3, 2026-11-12): once both hellos agree it, a chat message or command a player
  types carries 1024 characters instead of 256. The connection's codec is the one source of the
  limit (`PacketCodec::ChatLimit`, through `Connection::chatLimit`): the chat box stops typing at it,
  the server cleans a message to it, and the chat packet reads under it (`PacketRead::ChatLimit`,
  set by the Reborn codec for its connection's reads). A Legacy connection keeps 256 on both sides.
  `protocols.py` sends 599 characters from each client: the Reborn one arrives whole, the Legacy one
  as its first 255 (256, less the trailing space the chat box trims).
- Not built yet in step 3: varint fields and the per-packet table of string limits for the other
  packets.

`tools/harness/protocols.py` repeats the mixed check: one server with `protocols=reborn,legacy`, a
Reborn client that must agree batching and zstd, a Legacy client, both moved twice by `tp` and read
back. Checked with two processes on 2026-11-11: a Reborn client and a Legacy client on one dedicated
server with `protocols=reborn,legacy` at once, both moved by the server's `tp` and seen moving; a
Reborn client joining a game hosted from the client (Open to LAN) with the same setting; a server
with a mod the client lacks refusing it before world data with "the server requires the mod
hello@1.0" and the client showing the game's missing-mods message; the same client with the mod
joining; a `reborn` server turning a Legacy client away and taking a Reborn one; a Reborn client
against a Legacy server giving up. `lce_reborn_test` covers the frame reader (bytes arriving a few
at a time, skipping, the length limits, a cut frame). Not built: a game-side test that round trips
every packet through a frame (the two-process runs carry every packet a session sends, not every
packet the game has), retrying as Legacy, the registry hash, and features.

## 1024 players and beyond

Nothing assumes a number. The costs that grow with players, and what bounds them:

- Entity ids are 32 bits on the wire (varint) and the server allocates them from one
  counter, so an id never limits the count.
- The player list is sent as a first full list in pages and then as joins, leaves and
  changes only; a client builds its tab list lazily.
- Tracking (who is sent what) uses interest management: a spatial grid of entities
  and one visible-chunk set per player, so the cost of a tick is the entities in
  range, not players squared. Spawn and despawn work is queued and spread over ticks.
- Each connection has a byte budget per tick, with priorities (nearby movement before
  distant, chat and inventory never starved), so one slow client cannot hold the
  tick or the server's memory.
- The server tick is split into jobs (network encode, chunk send, entity update) that
  run on worker threads, so adding players adds cores, not latency.
- The game's own limits move with this: `PlayerList`, the leaderboard and mini game
  code that assume a small number are changed or bounded by a setting (`max-players`,
  default 8, up to the protocol's limit of 2^31).

The target is measured, not asserted: a headless load test connects N scripted
clients to a dedicated server and reports tick time and bytes per client at 8, 64, 256
and 1024.

## Chat and commands

The Reborn chat and command packets carry 1024 characters (the limit is a field of
the packet table, so it can go up). Legacy cannot: its chat packet is capped at 256
by the Legacy build's own reader, and a longer string disconnects a Wii U peer, so no
Legacy connection is ever sent more. Command text that never crosses the wire, as in a
command block or a mod, is not limited by this.

## Resilience

The Reborn session survives a short break: the client holds a resume token and the
last acknowledged sequence, the server keeps the connection's state for a time and
replays what was missed. It runs over TCP first. The peer-to-peer transport already
provides a reliable stream over datagrams (`p2p.md`); a datagram transport for the
game's own latency-sensitive packets is a later step behind the same interface.

## Order of work

1. The contract: close what is still reached around `Connection` and `PacketCodec`.
2. Preamble, framing, hello and negotiation, with a codec test that round trips every
   packet and a skip test for unknown ids.
3. Varint ids, strings with per-packet limits, batching and compression.
4. Interest management and the per-connection budget, then deltas.
5. The load test and the work it points at, up to 1024.
6. Resume, then the datagram transport.
7. A test with a real Legacy peer, so the two protocols are shown not to disturb each
   other.
