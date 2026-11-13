# Commands

Last checked: 2026-11-02.

Chat commands are a project choice, not a fidelity claim. The target syntax is
Java Edition 1.14, including NBT arguments (`dataTag`) and the 1.13 NBT path
grammar. The retail LCE command surface is context for what the console build
shipped, not a limit on this target.

## Where the code is

- `source/game/command/ChatCommands.cpp` owns the registry, name lookup, and
  the console path. The command objects are the families under
  `source/game/command/chat/`.
- `CommandArguments` parses words, numbers, coordinates and positions.
  `TargetSelector` resolves `@p`, `@a`, `@r`, `@e`, `@s`.
- `CommandSource` carries the sender, level, position, rotation and
  permission. `ChatCommand::Tell` and `Fail` write to chat for a player and to
  the console sink otherwise.
- `CommandDispatcher`, `GameCommandPacket` and `EGameCommand` are the binary
  LCE command path. Menu actions use it, and several text commands hand off to
  a binary command.

## Current inventory

39 commands and 5 aliases, from the constructors under `chat/`:

| Command | Usage |
| --- | --- |
| `clear` | `[player] [item]{<data>} [maxCount]` |
| `data` | `get block <x y z> [path]`, `get entity <target> [path]`, `merge block|entity <target> <data>`, `modify block|entity <target> <path> set|merge|append|prepend value <data>`, `modify block|entity <target> <path> insert <index> value <data>`, `modify block|entity <target> <path> set|merge|append|prepend from block|entity <source> [path]`, `modify block|entity <target> <path> insert <index> from block|entity <source> [path]`, `remove block|entity <target> <path>` |
| `defaultgamemode` | `<mode>` |
| `deop` | `<player\|uuid>` |
| `difficulty` | `<peaceful\|easy\|normal\|hard>` |
| `effect` | `<targets> <effect\|clear> [seconds] [amplifier] [hideParticles]` |
| `enchant` | `<targets> <enchantment> [level]` |
| `execute` | `<as\|at\|positioned\|align\|in\|rotated\|facing\|if\|unless\|run> ...`; `if\|unless` take `entity <targets>`, `block <x y z> <block> [data]`, `blocks <start> <end> <destination> all\|masked` and `data block <x y z> <path>\|entity <target> <path>` |
| `fill` | `<x1 y1 z1> <x2 y2 z2> <block>{<data>} [data] [mode] [replaceBlock] [replaceData]` |
| `gamemode` | `<mode> [player]` |
| `give` | `<player> <item>{<data>} [amount] [data]` |
| `help` | `[page\|command]`, alias `?` |
| `kick` | `<player> [reason]` |
| `kill` | `[targets]` |
| `list` | |
| `me` | `<action>` |
| `msg` | `<player> <message>`, aliases `tell`, `w` |
| `op` | `<player\|uuid>` |
| `bossbar` | `add <id> <name>`, `remove <id>`, `list`, `set <id> name\|color\|style\|value\|max\|visible\|players`, `get <id> value\|max\|visible\|players`: the game's own boss bars (`ServerBossEvent`), saved with the world (`data/bossbars`, `CustomBossEvents`); a name is plain text, not Java's JSON text, and the console HUD draws every bar in its own pink style, so color and style are kept and sent but not drawn |
| `clone` | `<x1 y1 z1> <x2 y2 z2> <x y z> [replace\|masked\|filtered <block>] [normal\|force\|move]`: up to 32768 blocks, block entities with them; it counts the blocks it changed, as `fill` does, where Java counts every block it copies |
| `locate` | `<structure>`: Java 1.14's names, answered by the game's own structure finder (the one eyes of ender use); the game keeps the desert and jungle pyramids, the swamp hut and the igloo as one feature, so each of those names finds the nearest of any of them. Checked that it answers; that a structure stands at the answer was not confirmed by eye |
| `particle` | `<name> [x y z] [dx dy dz] [speed] [count] [force\|normal]`: the game's own particle names, which are Java 1.12's (`explode`, `fireworksSpark`), not 1.14's; a particle that needs a block or an item is refused |
| `playsound` | `<sound> <source> <targets> [x y z] [volume] [pitch] [minVolume]`: the game's own sound names (`mob.pig.idle`), not Java 1.14's (`entity.pig.ambient`); a player out of range hears it at `minVolume` or not at all |
| `save-all` | (an operator; saves the game in either server) |
| `save-off`, `save-on` | (an operator; turns either server's automatic saves off and on, `ServerControl`) |
| `say` | `<message>` |
| `seed` | |
| `spreadplayers` | `<x> <z> <spreadDistance> <maxRange> <respectTeams> <targets>`: on the top solid block of a loaded column, never on liquid, the given distance apart; the game has no teams, so `respectTeams` must be false |
| `stop` | (the console's permission; stops the dedicated server, refused in a hosted game) |
| `setblock` | `<x y z> <block>{<data>} [data] [replace\|destroy\|keep]` |
| `setworldspawn` | `[x y z]` |
| `spawnpoint` | `[player] [x y z]` |
| `summon` | `<entity> [x y z]` |
| `teleport` | `[targets] <destination\|x y z> [yaw pitch\|facing <x y z>\|facing entity <target> [eyes\|feet]]`, alias `tp` |
| `time` | `<set\|add\|query> <value>[d\|s\|t]` |
| `toggledownfall` | |
| `weather` | `<clear\|rain\|thunder>` |
| `xp` | `<add\|set\|query> <targets> <amount> [points\|levels]`, alias `experience` |

`data` in these usages is the numeric block or item data value. It is not an
NBT tag.

`clear` matches an item's tag as a subset: every key in the argument must
exist in the item with a matching value, and every element of a list in the
argument must match some element of the item's list, in any order. An empty
list matches only an empty list. That is the item predicate rule, not full
tag equality, and NBT path filters use the same rule.

NBT paths accept filters: `{...}` at the start tests the whole data,
`name{...}` keeps a compound child that matches, and `list[{...}]` keeps the
list elements that match. A filter after an index or another filter is
rejected, and the whole data cannot be set or removed through a root filter.

## Gaps against the 1.14 target

The prerequisite column is the reason a command is absent, not an excuse to
invent one.

| Target | State | Prerequisite |
| --- | --- | --- |
| `/data modify` | done (`value` and `from`) | `set`, `merge`, `append`, `prepend`, `insert <index>` with `value <data>` or `from block|entity <source> [path]`; `from storage` too, and `string` is a 1.19 source outside the 1.14 target |
| `/data modify from`, `/data modify string` | from done | `string` is a 1.19.4 source outside the 1.14 target, so it is not a gap |
| `/data ... storage` | done | `get`, `merge`, `modify` and `remove` on command storage, a compound per namespaced id saved with the world (`CommandStorage`, `data/command_storage`); storage is Java 1.15, one version past the target |
| Numeric `data` argument on `give`, `fill`, `setblock` | present | 1.13 removed it in favour of NBT; this port still keeps data values as a separate field. `clear` no longer takes one |
| `/loot` | `give`, `insert`, `spawn` and `replace entity|block`, from `loot <table>` or `kill <target>` | a table is a built-in one (`BuiltInLootTables`) by name, with or without `minecraft:`, rolled with no looted entity; `kill <target>` rolls the first mob the selector finds with its default loot table and itself as the looted entity (its own `DeathLootTable` is not read, since `Mob::mLootTable` is protected, and an armour stand is not a `Mob`); `replace` fills the slot and the ones after it, up to the count, with the slot code `/replaceitem` uses; `fish` is `loot <table>` with `gameplay/fishing` (the game rolls the same table when a bobber is reeled in; checked), as 1.14's `fish` adds only a position and a tool the table does not read here, and `mine` is absent because a block's drops are written in its class, not in a loot table (`loot-command.json` checks the rest); the chat box completes the table names |
| `/datapack` | not planned | a data pack system; content comes from mods (`modding.md`), which already cover what data packs carry here |
| `/schedule` | absent | function files, which wait with `/function` on the scoreboard decision |
| `/teammsg` | absent | Java teams, which live in the objective scoreboard the game does not have (its mini game teams are a separate system); waits with `/scoreboard` |
| `/replaceitem` | done for players and containers | `replaceitem entity <player> <slot> <item>{<data>} [count]` (`slot.hotbar.0-8`, `slot.inventory.0-26`, `slot.armor.feet|legs|chest|head`, `slot.weapon.mainhand|offhand`) and `replaceitem block <x y z> slot.container.N <item> [count]`; a mob's equipment slots wait for an equipment API (`replaceitem.json` checks it). The 1.14 syntax is `replaceitem entity` and `replaceitem block`; there is no `with`/`from` form; the chat box completes the slot names (`ReplaceItemChatCommand::PlayerSlotNames`, one table for completing and parsing) |
| `/scoreboard`, `/function`, `/trigger` | absent | a Java objective scoreboard and function files, which the game does not have: its `Scoreboard` holds the mini games' round statistics, and the binary `TriggerCommand` is empty in its own source; building them (with a sidebar on the HUD) is an owner decision |
| `/testfor`, `/testforblock`, `/testforblocks` | not a gap | removed in Java 1.13, which replaced them with `execute if entity`, `if block` and `if blocks`; those are built |
| `/worldborder` | not built | the console world is bounded by its size, chosen when it is made, and has no border that moves or shrinks; a command would need that system first, which is an owner decision |
| `/title` | absent | the game has the packet but draws no titles (`ClientPacketListener::handleSetTitles` is empty in its own source), so the command waits on an interface decision about where a title is drawn |

## Selectors, namespaces and units

Selector arguments: `type` (also `!`), `name` (a player's name, or an entity's
custom name, case sensitive, quotes allowed), `limit`, `c` (a negative count takes the
furthest), `sort` (`nearest`, `furthest`, `random`, `arbitrary`), `tag` (repeat it;
`tag=!x`, `tag=` for none, `tag=!` for any), `nbt` (a subset match, `nbt=!{...}`
negated), `gamemode` and `m`, `distance` and `r`/`rm` (ranges like `1..5`, `..5`,
`3..`), `level` and `l`/`lm`, `x_rotation`, `y_rotation`, `x y z`, `dx dy dz`. `@p`
and `@r` default to one result, nearest and random.

Namespaces: a name typed in a command may be bare or prefixed, and `minecraft:` is
the default namespace, as in Java. Any other namespace is not a name this game has
and is refused (`Unknown entity type foo:pig`), never read as the bare name.
`/data get entity` shows the entity's `id` in its namespaced form
(`id:"minecraft:pig"`) as 1.14 does; items and block entities already did. Saves
keep the legacy ids for the Wii U. The chat box (`ChatScreen`) suggests names in their prefixed form
(`minecraft:pig`, `minecraft:diamond_pickaxe`) for entities, items, blocks, effects
and enchantments, and in `type=` of a selector. Typing `pi` finds `minecraft:pig` and
`minecraft:piston` (names that start with it come first) and also names with a word
starting there (`minecraft:diamond_pickaxe`); typing `minecraft:pi` works too.
Suggestions only list the commands the player may use, so a host without cheats sees
the few commands everyone may use.

Time arguments (`time set|add`) take `d` (24000 ticks), `s` (20) or `t`, and a
fraction: `0.5d`.

## NBT changes from 1.13 to 1.14

Source: the Java Edition 1.14 page on the Minecraft wiki. Everything that
does not need 1.14 game content is done or listed here as a task. What
needs the 1.14 content waits for the session that backports the whole
version.

| 1.14 change | State | Needs |
| --- | --- | --- |
| NBT path syntax (filters, `[]`, negative indices, multiple results) | done | |
| `/data modify`, `insert`, `from`, `merge` | done | |
| `Passengers` list (mobs riding mobs) in `summon`, saves and `data get` | done | the list was saved as `Riding`; it is `Passengers` now and old `Riding` lists still load (`Parity Not Checked`) |
| `Glowing`, `Tags`, `NoGravity`, `Silent`, `CustomName` | done | already read and written by `Entity` |
| Single-quoted strings in commands and NBT | done | |
| Command length: chat and commands up to 256 characters | done | that is the legacy wire cap; longer needs the new protocol |
| `CustomModelData` on items | stored as any item tag | the item model property needs resource packs |
| Thrown item entities' optional `Item` tag (egg, snowball, pearl, bottle, eye of ender, fireballs) | absent | a renderer that reads it |
| `BlockStateTag` on block items | absent | block states; this port still uses data values |
| Item lore as chat components | absent | JSON text components |
| Book and quill: 100 pages and the new editor | absent | the editor scene (`MAX_PAGES` is 50) |
| `VillagerData` (`level`, `profession`, `type`) replaces `Profession`, `Career`, `CareerLevel`; zombie villager `VillagerData` and `Offers` | wait | the 1.14 villager rework (professions, brain) |
| `WanderTarget`, cat, fox and panda genes | wait | those mobs |
| Armor stand `DeathLootTable` and loot table NBT | wait | the loot table system |
| `nbt` text component (`block` and `entity` variants, `interpret`) | absent | JSON text components |
| Entity type tags in `type=#tag` selectors | absent | a tag system |
| Selector arguments `scores`, `team`, `advancements`, `predicate` | absent | the scoreboard, team, advancement and predicate systems |

Do not infer a missing-command count from this table. The registry in
`ChatCommands.cpp` is the inventory; this table names the 1.14 targets that
are not registered.

## Merging and removing

`/data merge` writes the tag through the entity or block entity's own load
path, so a field that load does not read is dropped, exactly as it is on a
world load. `/data remove` removes the value from the saved data; a field the
entity keeps in a member of its own, `CustomName` for one, keeps its current
value until the entity is reloaded, because load only sets such fields when
the tag has them. Removing a list element or a compound child that the block
entity reads back, such as a chest's `Items[0]`, takes effect at once.

`/data modify` selects the path as a whole and replaces it with the value:
`set` on a list replaces the whole list, `set` on a missing name creates it.
`merge`, `append`, `prepend` and `insert <index> value` work inside the
selected value. A value whose type does not match a list's element type is
rejected. `insert` counts a negative index from one past the end, so
`insert -1` appends; an index outside `-(size + 1) .. size` fails outright
instead of clamping. Several values go in at consecutive positions, so
`prepend` and `insert` keep their order.

Long arrays (`[L;1L,2L]`) are read, written and indexed like the others.
They are a format capability: the world records that it uses them and the
legacy network protocol leaves them out (`compatibility.md`).

Byte, int and long arrays are collections like lists: `[n]`, `[-n]` and `[]`
select their elements as byte, int or long tags, `set` on an element and
`append`, `prepend` and `insert` take any number and convert it to the
array's type, and `remove` deletes elements. Any other value is rejected.
Element filters (`[{...}]`) apply to lists only.

The insert index rule and the number conversion follow 1.14's
`DataCommands` and `CollectionTag` as recalled, not checked against its
source.

`from` copies from another block or entity instead of a literal. The rules
match the documented 1.14 behaviour, whose error strings (`expected_list`
for list targets, `expected_object` for merge, `invalid_index` for insert)
are unchanged from 1.14.4 through today's game; the wording here is the
common style of this port:

- one source value applies to every selected target, otherwise the counts
  must match and the source values pair with the targets in order.
- `set` does not flatten: a selected list replaces the target list whole.
- `append`, `prepend` and `insert` flatten a selected list or array into
  its elements, so one command can copy a whole inventory item by item.
- a source path that selects nothing fails like `/data get` does.

`from storage` needs command storage, which this engine does not have, and
the `string` source was added in 1.19.4, outside the 1.14 target. Both are
therefore absent by design.

Like `merge`, `modify` writes through the entity's or block
entity's own load path, so a field that load does not read, `CustomName` for
one, keeps its current value until the entity is reloaded. A field the game
saves only when it is set, such as a mob's `NoAI`, disappears from the saved
data when modify sets it to its default rather than staying present as `0b`.

## Command blocks

The three blocks (`command_block` impulse, `repeating_command_block`,
`chain_command_block`) run commands the way 1.14 does, with no screen to write the
command in yet. Set the command and the options with data:

```
setblock <x y z> command_block{Command:"say hi",auto:1b} <facing + 8 if conditional>
data merge block <x y z> {Command:"time set day"}
```

The data value is the facing (0 down, 1 up, 2 north, 3 south, 4 west, 5 east) plus 8
for conditional; `auto:1b` is Always Active. What runs:

- Impulse: a rising redstone signal runs it once, a tick later; with `auto` it runs
  once when it is set.
- Repeating: runs every tick while powered or `auto`.
- Chain: runs when the block behind it (the one it faces away from) has just run,
  in order, up to 65536 blocks in a chain, only if powered or `auto`; a chain stops at
  the first block whose command fails.
- Conditional: runs only if the block behind it succeeded last time, else its success
  count is set to 0.
- A command block runs commands as an operator (permission 2) at its own position, so
  `~ ~ ~` is the block. Success is a command that did not report a failure; the success
  count is 0 or 1 (1.14 counts what the command affected), and `LastOutput` keeps the
  command's reply. A comparator reads the success count.

`harness.py check command-blocks` covers an impulse block, a chain with a failing
block and conditional blocks around it, and a repeating block that puts a removed
block back.

Not done: the screen (it is designed with the project owner first), the setting that
disables command blocks on a server (they are always on), the minecart command block,
`/gamerule maxCommandChainLength` (the 65536 is fixed), the command block's name in
`say` and `msg` output, and command stats (`successCount` into a scoreboard).

## The next piece

`source/game/data/StringNbtReader` reads 1.13 SNBT, `/give`, `/summon`,
`/setblock` and `/fill` apply a tag, `/clear` matches items by one, and
`/data get|merge|modify|remove` read and write entity and block data through
`NbtPath`, `StringNbtWriter` and `NbtMatch`, including the `from` forms of
`modify` and path filters. What comes next is listed in `docs/todo.md`.

## Rules

1. Reuse `ChatCommands`, `CommandArguments`, `TargetSelector` and the
   permission checks. Do not add a second dispatcher or a per-command parser.
2. Register by adding to the `defaults` array in `ChatCommands.cpp`, or with
   `ChatCommands::Register` for content that is not part of the base set.
3. Append new binary command ids to `EGameCommand`; never renumber one.
4. A text command that hands off to a binary command must have that binary
   command registered in `CommandDispatcher`.
5. Syntax and capability are separate. If the behaviour does not exist, the
   command does not exist; say which system is missing.
6. Fail with a message that names the bad argument. Silent failure is a bug.
7. Test through the running game, not only by reading the parser:
   `python tools/harness/harness.py cmd "time set day"` runs the same
   `RunConsole` path the console uses.
