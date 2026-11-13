# Saves

Goal: a world is cheaper to hold, load and save. The order is the project's:
data layout first, then the container, and the chunk compression only if the
measurements still point at it (`measuring.md`, Saves).

## What the numbers say

- A chunk is 10 to 13 KB before compression and 3.4 KB after (LZX + RLE), and the
  blocks and light are already sparse. Loading costs about 125 us a chunk, split
  between decompressing (48) and building the chunk (72); reading is 4.5.
- Saving a chunk costs 377 us of compression in the save threads against 18 us to
  serialise it. Only changed chunks, and chunks with entities, are written.
- The container is a whole file rewritten on every save: it is zlib-compressed
  as one block (143 ms for 7.2 MB, constant whatever changed) and gains nothing
  over the chunks inside it beyond dropping the 4 KB sector slack (31 to 41%).

## Decisions

1. The new container is a capability (`compatibility.md`), named in `level.dat`
   like long arrays. A reader without it refuses the world with "Can't Open
   World". This build still opens the old container.
2. One-way export: a world in the new container can be written back to the old
   container, so the Wii U build and other LCERecomp projects can open it. A
   world is not kept writable in both at once. (Project owner, yes.)
3. Chunks keep their record format and LZX + RLE compression for now; they are
   what the Wii U reads, and a changed codec is a later step if measuring asks.
4. The container changes: no outer compression, no 4 KB sectors. It holds the
   same files, each region as an index of exact chunk lengths plus the records
   appended where they fit, with a free list and compaction. A save touches the
   records of changed chunks, the index, and nothing else. Each record carries a
   checksum, so a torn write is found when it is read instead of trusted.
5. A save is atomic: new records are written, then the index entry is switched,
   so a crash leaves the previous chunk. The index and the free list are small
   and are written last.

## The native container (built)

`source/savecontainer` (library `lce_savecontainer`, C++98, no game code; test
`lce_savecontainer_test`, run by `ctest`). On PC the game writes `world.bin` in
this container and reads either container, told apart by the first four bytes:
`LCES` is native, a zero word is the old zlib-wrapped one.

All integers are little endian with explicit widths. The file is an 8-byte header
(magic `LCES` = `0x5345434C`, version 2, flags 0) followed by segments. Each save
appends one segment, so a save writes what changed and nothing else.

| Segment field | Bytes |
| --- | --- |
| `SEG1` (`0x31474553`), payload length | 4, 4 |
| payload | length |
| CRC-32 of the payload | 4 |

Payload: a flags byte (bit 0, reset: forget everything before this segment); the
original and current save versions (2, 2); the file table (count, then per file:
kind 0 raw or 1 region, name length in UTF-16 units, the units, timestamp); then
items. An item is a raw file (index, size, bytes, CRC-32), region chunks (index,
count, then per chunk: slot, timestamp, record header word, decompressed length,
CRC-32, payload) or removed chunks (index, count, slots). The table is complete
every time, so a file missing from it is gone. A region file (`*.mcr`) is stored as
its chunk records with no sector padding; the payload is exactly the compressed
bytes, and the length is in the record header word as in the old format, so LZX
and RLE stay as they are. Replaying the segments in order gives the state;
`Decode` rebuilds a valid region file with the old sector rule.

Writing (`Writer`): it keeps a 64-bit hash of every chunk record (header word,
decompressed length and payload, not the timestamp) and of every raw file as last
saved. A save hashes the in-memory save (about 7 MB, a few milliseconds), writes
the records whose hash changed or is new, lists the chunks that disappeared, and
appends that segment. It writes a whole file (a segment with the reset flag) when
there is no history, when the file on disk is not the length the writer expects
(another save was dropped, or something else touched it), or when the file has
grown past twice the live data plus 1 MB. A loaded save carries on with appends,
since the writer adopts the state it just decoded. `CStorage` appends only if the
file is exactly the length the writer expects, else the append fails and the next
save is a whole file.

Reading: segments are replayed in order. A last segment that is cut short or
fails its checksum is a torn write from a crash and is ignored; the file is then
shorter than the writer thinks and the next save rewrites it whole. A damaged
segment with more data after it is not a torn write: the save is refused.

Measured on world A (the same world as above): a save after nothing changed takes
about 5 ms in the container and appends under 1 KB; one with 173 chunks changed
appends 684 KB in 5 ms (the whole-file container took 38 ms, zlib 143 ms). After a
reload the first save wrote about 500 chunks (2.1 MB in 13 ms). Counting what marked
them unsaved showed they are chunks the world generated after loading (the world
file only held part of the terrain around the spawn), so that write is real.

Durability: an append is flushed but not synced to the disk, so a power cut can
lose the last save; a crash of the game cannot damage an earlier one.

The capability is the magic: an earlier build reads `LCES` as an unknown save and
does not open it. There is no `level.dat` key, because the container is the file
around `level.dat`, not data inside it.

Export: `LCERebornSaveTool to-legacy NATIVE LEGACY` writes the old
container (`to-native` does the reverse), so a world can go back to a build that
only reads the old one. It writes the PC byte order. Converting to the Wii U's
byte order and platform tag is not done. The save screen's "Export" entry is not
built.

A damaged save is refused: when either container fails to decode (a bad checksum,
length, slot or count in the native one, a failed zlib stream in the old one), the
save file is marked damaged, the server start stops before anything is created
and the player sees "Can't Open World" with a message to restore a copy. The file
is not touched. Checked by flipping one byte in the middle of a native save: the
dialog appeared and the file's hash was the same afterwards.

## Chunk compression: zstd

What PC calls LZX + RLE is run lengths followed by zlib level 6 (the game's
`Compression` on PC; the Wii U's LZX is a different codec). Measured on the 1388
chunks of world A, in Python on the raw chunk bytes: zlib level 6 takes 364 us a
chunk to compress and 38 us to decompress and gives 4.79 MB; zstd level 1, 3, 5
and 9 give 5.03, 4.95, 4.79 and 4.74 MB, compress in 35, 46, 101 and 279 us and
decompress in 14 us. PC now compresses chunk records with zstd level 3
(`Codec::kZstdLevel`): 8 times faster to write and 2.6 times faster to read, for
3.5% more bytes. Level 5 gives the same size as zlib at 100 us if that is wanted.

A chunk record marks its codec in its header word: bit 31 run lengths and zlib,
bit 29 (new) zstd, bit 30 compressed as before; the length mask is now 29 bits
(a record is under a megabyte). Zstandard 1.5.7 is vendored in
`tools/build/third_party/zstd` (BSD-3-Clause, with the pin in its README) and built
without its assembly. Records in an old save stay as they are and are read as
before; a chunk is rewritten in zstd when it is next saved, so a save moves over
chunk by chunk. A build before this one cannot read a zstd chunk.

Export converts every zstd chunk back to run lengths and zlib
(`Export::ToLegacyChunks`, used by `LCERebornSaveTool to-legacy`), so an exported
world has no zstd. The Wii U's LZX is not produced; its byte order is not either.

In the game, saving 515 chunks took 32 ms of compression (it was 171 to 192 ms for
about as many) and loading 606 chunks 9 ms of decompression (it was 26 to 29 ms).
The chunk bytes written were 3.84 KB a chunk against 3.76. After a save, a reload
and an export, the exported world has the same 1388 chunks.

Each thread keeps one zstd compression and one decompression context for its
lifetime (freed with the process, not per thread).

Not done: a build of the export that runs inside the game, and a trained zstd dictionary.
The dictionary was measured on 2026-11-10 with the `zstd` command line (1.5.7, level 3 as
`ScCodec::kZstdLevel`): the 1278 chunks of one 4.8 MB world were exported with `to-legacy`, their
bytes after the zlib layer (still run-length coded, so not the exact bytes the native codec
sees) split 4 to 1 into a training set and a held-out set, and the held-out 255 chunks
(1.74 MB) compressed to 938 KB with no dictionary, to 875 KB with a 16 KB dictionary (6.8%
smaller), 843 KB with a 64 KB one (10.2%) and 837 KB with a 110 KB one (10.8%). Limits of that
number: one world, one seed, a dictionary trained on the same world it was tested on, and not
the native bytes, so it is an upper bound for other worlds. A dictionary would be part of the
format (every reader needs the same one, and a record would name it), which makes it a
capability; about 10% of chunk bytes does not yet justify that, so it waits for a measurement
over several worlds.

## Block numbers no block is registered under

A chunk can hold a block number that nothing in this build is registered under: a mod block from a
world saved before chunks carried a mod palette, or a number from another build. The game itself has
no answer for it (`Block::byId` returns null and the first height map pass crashes). On PC such a
number loads as air, with one warning per chunk ("Chunk X, Z holds blocks this build does not have;
they load as air"). `ModBlocks::BuildRemap` holds the rule for both cases, through the palette for mod
blocks and air for an unregistered number; the chunk is only rewritten when it needs to be, which
`LevelChunk::holdsUnknownBlock` answers from the storage's own section palettes without unpacking the
chunk. Checked on 2026-11-12: both stale test worlds crashed on load without the change and load with
it (one chunk each); loading 748 chunks of a generated world took 0.05 to 0.06 ms a chunk to
deserialise with the check and 0.05 without. This is a PC change to the original loader (owner
decision D13, taken as the plan recommended: air with a log line).

## Order of work

1. Measure (done): `ChunkStats`, `harness.py save` and `chunkstats`.
2. In-memory layout, only where a measurement points at it. Measured on 2026-11-12
   (`harness.py chunkstats`, its `memory` entry, on a generated world with 748 chunks loaded
   from disk): a loaded chunk holds about 10.6 KB, 6.0 KB of block elements (the palettised
   storage; 128 KB unpacked), 3.3 KB of light and 1.3 KB of the object and its fixed arrays. A save
   of 677 chunks took about 90 ms: 6.5 ms to serialise them (7.3 MB written), 53 ms to compress
   them (to 2.6 MB), 15 ms to encode the container and 15 ms to write it. On the way to disk a
   chunk is copied four times (the record into a growing buffer, the compressed record, the
   in-memory save file, the container). Compression is most of the cost and the in-memory layout
   is already compact, so the layout stays as it is; a faster level or a dictionary for zstd
   (step C5) is where a gain would come from.
3. The new container behind the existing `ConsoleSaveFile` interface (done, see
   above, as a transform at the file boundary rather than a new implementation), with a test that round-trips every chunk of a real world
   through it and through export back to the old container, byte for byte.
4. The capability in `level.dat` and the refusal on older builds (done 2026-11-12,
   `compatibility.md`), and "Export for Wii U" in the save's options (not built; the tool's
   `to-legacy` does not yet warn that a world recording mod block palettes cannot be read by a
   Wii U build).
5. Measure again against the table in `measuring.md`.

Not decided: whether chunk records themselves change (a layout that skips the
decompress-then-deserialise double pass). That depends on step 2.

## Chunks with entities that are not simulated

The original rule rewrites a chunk that holds entities whenever the game time has
moved on (or 600 ticks have passed for an autosave). An entity that is not
ticked does not change: `Entity::shouldTick` keeps one that is far from every
player still, apart from the despawn check, which removes it and so touches the
chunk. So a chunk now carries `mEntitiesActive`: it is set when an entity in it
ticks, is added to it at run time or is removed from it, and cleared when its
entities are saved and when it loads. `LevelChunk::shouldSave` asks for a rewrite
for entities only while it is set. (`Parity Not Checked`: this differs from the
original rule on purpose.) After loading a world and saving twice a few seconds
apart with a player at the spawn, the second save wrote 178 chunks instead of
202; those are the chunks near the player, where mobs move. A far chunk is not
written again until something in it ticks.

Known gap: a command that changes an entity far from every player (data merge,
teleport) does not mark its chunk, so the change is saved only once the entity
ticks or the chunk is otherwise unsaved.
