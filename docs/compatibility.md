# Compatibility and capabilities

LCEReborn plays with the Wii U build and with other projects built on
LCERecomp, and it can open their saves. It also goes beyond what those
read. Every format extension is a named capability: data that needs one
is kept away from a reader that has not declared it.

## Format capabilities

`source/game/data/NbtCapabilities` lists what NBT data can need beyond
the legacy format and checks a tag tree against a set:

| Capability | Bit | Needed by |
| --- | --- | --- |
| Long arrays | `0x1` | a `TAG_Long_Array` (id 12) anywhere in the tree |
| Mod block palettes | `0x2` | a chunk saved with a mod block palette (record version 13), recorded when the chunk is saved |

`kLegacy` is the set every LCE reader understands, and is empty.
`Required(tag)` reports what a tree needs, and `Restricted(tag, allowed)`
returns a copy without the parts the allowed set excludes; it returns the
tag itself, with no copy, when nothing needs removing.

## On the wire

There are two wire protocols behind one network contract (`Connection` reads
and writes through a `PacketCodec`; game code never sees which one):

- Legacy: the format as it was before our changes. LCERecomp projects and the
  Wii U build speak it. Its bytes are frozen and it has no negotiation, so
  it carries only `NbtCapabilities::kLegacy`: an item reaches a Legacy peer
  without its long arrays while the server keeps them (see Creative inventory
  below). It is never extended.
- Reborn: the preamble, hello, negotiation and framed packets are built (`reborn-protocol.md`,
  "What is built"); it is incompatible with Legacy on purpose.
  Capability negotiation, feature gating and play between versions belong to
  it alone. The capabilities it agrees include the format capabilities above,
  which then replace `kLegacy` where NBT is written to the wire
  (`DataOutputStream` already carries the set per stream).

A connection chooses by its first four bytes on the same port: a Reborn client opens with a preamble,
and a server set to accept Reborn tells the two apart (`reborn-protocol.md`). Legacy is the default
on both sides.

Peer-to-peer (`p2p.md`) is a transport that carries either protocol; it is not
a third one.

## In saves

`level.dat` records the capabilities the world's data uses, as
`NbtCapabilities` in its `Data` compound. The key is written only when
the set is not empty, so a world that never used an extension is saved
exactly as before and stays readable by the Wii U build and other
LCERecomp projects.

Long arrays only enter a world through commands, so the shared command
parse (`ChatCommand::ReadData` and `ReadCompound`) adds what the parsed
data requires to the world's set as it reads it. `/clear` parses a
pattern it never stores and does not record anything.

A world that records a capability cannot be opened by a reader without
it: the Wii U build stops on the unknown tag id. This build checks the
recorded set against `kKnown` before it loads anything else: a world that
needs a capability this build does not know is refused with "Can't Open
World", and the game returns to the main menu. The log names what is missing ("The world needs
what this build cannot read: unknown 0x40"; a capability this build knows is named, one from a newer
build is given as its number, since the name is not in the save). Checked on 2026-11-12 by marking a
world with an unknown capability through the mod API, saving and reopening it; `capabilities` in
ctest covers the rule, and the `capabilities` scenario checks that saving a chunk with a mod block records `0x2`. The server reports the
failed start (`MinecraftServer::s_startFailed`) before it signals that it
is ready, so the client cannot miss it.

## The save container

A save is a flat container of files. PC builds write it in a native layout (magic
`LCES`, `saves.md`) and read it and the old zlib-wrapped layout; an earlier build
cannot open the native one. `LCERebornSaveTool` converts either way, in PC byte
order. The Wii U byte order is not produced. PC compresses chunks with zstd, which
no other build reads; the tool converts them back to the old compression when it
exports.

## Creative inventory

A client on the legacy wire sends an item back as it received it, without
the parts it never saw. The server keeps, per player, the last 64 items
with such parts that a creative inventory packet replaced or cleared, and
when an item arrives whose tag equals the legacy form of a stashed item or
of an item still in the inventory, it uses the full tag instead. Moving,
dropping and cloning items in the creative inventory are meant to keep
their long arrays this way; that has not yet been checked in a running
client.

## Parity

Long array support adds a case to `Tag::newTag`, an entry to the tag type
names, the wire restriction to `DataOutputStream` and a field to
`LevelData`. These are deliberate extensions, so `tools/measure/parity.py`
reports those functions as differing from the original, and that is
expected.
