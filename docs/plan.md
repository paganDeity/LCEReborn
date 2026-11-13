# LCEReborn: the plan for everything pending

Repository: `/home/anto/Documents/ItAll/portable-lce/scratchpad/LCEReborn`, branch
`harness-and-nbt-filters`, last commit `27272e00`. Executor: Opus 5.5.

## Context

The project is a matching decompilation of Minecraft: Wii U Edition with a native PC build, a
generated mod API, a dedicated server, a debug harness, and several planned systems that are
designed but not built. The owner asked for one plan that covers every pending, planned or
discussed item, so the next model can execute without re-deriving state. This plan is that list,
ordered, with what exists, what to do, where, how to check it, and which decisions belong to the
owner. `docs/todo.md` remains the project's own ordered list and `docs/handoff.md` the dated
status; this plan is the execution order over both, plus what was discussed and never written
down.

Owner's standing requirements (verbatim sense):
- Everything behind orthogonal, DRY, maintainable interfaces with no technical debt.
- The modding API finished: extensible, reliable, comparable to Fabric, low and high level, with
  no hand-maintained hooks.
- The server competent and safe; the standalone and the built-in server build from the same
  source and have the same features.
- The interface faithful to the original Legacy Edition; new features placed with taste;
  Identities, Friends, chat and voice chat rethought.
- Take from `../LegacyEX-PC` what is worth taking; its quality is unchecked.
- Reborn protocol and save format are the next large pieces.

## Rules the executor must keep (read `AGENTS.md` first, then `docs/handoff.md`)

- Game and shared code is C++03; `source/platform/pc/` is C++17. No comments in game code except
  why-comments already in use and `// Parity Not Checked` above an unchecked fix to original code.
- A fix to decompiled code is PC-guarded (`#if LCE_PLATFORM_PC`) when it would change Wii U code.
- Never write C++ only for the mod API to call; reshape the system. No hand-declared function in
  `api/lce.api`. After any header or `.api` change: `python3 tools/hooks/hookgen.py generate`,
  commit generated files.
- A server feature is written once (game code or a PC module both programs start), never in
  `PcServer.cpp` alone, never behind `IsDedicatedServer()` unless the difference is the absence of
  a window or local player (`AGENTS.md`, "Servers").
- Legacy wire bytes are frozen. Saves that use no capability are written exactly as the original.
- State every result as checked (how), ported with a disclosed divergence, or not implemented.
  Never commit before reading the check's output.
- Screens are designed with the owner before any is written.

## Verification toolkit (used by every work package)

```sh
cmake --build build/pc-debug -j 3 && ctest --test-dir build/pc-debug     # 9 tests
cmake --build build/pc -j 3 && ctest --test-dir build/pc                 # Release; required after header/.api/generator changes
python3 tools/harness/harness.py check                                   # all 11 scenarios; run twice (worlds are reused)
python3 tools/hooks/hookgen.py check
```

- Running game: `harness.py up --headless --seed N [--size 640x360]`, `cmd`, `state`, `screen`,
  `shot`, `key NAME down|up` (press and release at once; `MouseLeft|MouseRight|MouseMiddle`),
  `type`, `down`. Operator suggestions need a new world with
  `--launch-arg=--cheats --launch-arg=on`.
- Two processes: `LCERebornServer --save-dir D --log-dir D --mods-dir M` with its console on a
  FIFO (`tail -f fifo | server`), then
  `harness.py up --headless --launch-arg=--join --launch-arg=127.0.0.1:25570`. Check
  `ss -ltnp | grep 25570` first; always `harness.py down` before a join test (a leftover client is
  silently reused).
- Read the build log before running the game; a build is not finished when a tail line prints.
- Every new behaviour gets a scenario (`tools/harness/scenarios/*.json`, re-runnable) or a unit
  test (`tests/*.cpp`, C++98 where the code is game code), plus a line in the relevant doc.

---

## Phase A: finish the server (owner priority; mostly bounded)

Done already (do not redo): `AccessList`, `ServerAccess` (whitelist, bans, address bans, max
players, connection limits, packet rate), `PcServerAccess` (files and settings for both
programs), commands `whitelist|ban|pardon|banlist|ban-ip|pardon-ip`, `PacketRead::Refuse`,
`NbtBudget`, `ServerWatchdog`, unit test `tests/access_test.cpp`, scenario `server-access`.

A1. Done (`PcWorldBackup`, `--log-keep-days`; `pc-platform.md`). World backups and log pruning.
- Add to `PcServerAccess` (or a sibling `PcServerUpkeep` both programs start): `backup-on-start`
  and `backup-keep` settings; copy the world folder to `backups/<world>-<UTC timestamp>` before
  the world loads; prune to `backup-keep`. Check whether `logs/` ever prunes its dated `.log.gz`
  files (`Log::Start` in `source/platform/pc/Log.cpp`); add `log-keep-days` if not.
- Check: unit test for the pruning rule; two-process run showing a backup folder and a pruned one.

A2. Done for one address (2026-11-11: `rawpacket`, `tools/harness/hostile.py`; the server now closes a cut-off peer's link). A hostile test client (the missing tool for the packet limits).
- A small test program linking `lce_net` that completes the link join and then sends chosen raw
  packet bytes (`source/net/NetLink.hpp`, `NetMessages.hpp`). Cases: `CraftItemPacket` with a
  huge count, texture packets with negative lengths, an over-long string, NBT with a huge list
  count, a packet flood.
- Check: for each case the server logs a refusal or a flood cut and stays up; add as a ctest that
  starts `LCERebornServer` on a spare port (needs a `port` override and game content, so gate it
  like the harness checks if content is absent).
- Also covers the per-address and pending limits from a non-exempt path if the client binds
  another local address (127.0.0.2).

A3. Done (`CGameNetworkManager::GetPlayerAddress`). `ban-ip <player>` and the address of an online player.
- `INetworkPlayer` must not gain a virtual (decompiled class). Add a lookup on the PC network
  manager (`CPlatformNetworkManagerLan`: system by player, `link->Peer()`), reached through an
  existing seam (`CGameNetworkManager`), and let `BanChatCommand::BanAddress` accept a name.

A4. More than 8 players (needs owner decision D1).
- `GameSessionData::MAX_SESSION_PLAYERS` (8) sizes session arrays and wire fields
  (`ClientboundPreLoginPacket` player list, player index bytes). Legacy bytes are frozen, so more
  than 8 is a Reborn capability: plan it inside Phase B step B3, not as a Legacy change. Until
  then `max-players` only lowers the limit.

A5. Done (2026-11-11; 50 clean sanitizer runs, findings fixed, `handoff.md`). Unexplained crash: `LootingEnchantFunction::apply` from `/loot give @p kill <mob>` (seen
twice, not reproduced in about 200 runs after `LootTableContext::Builder` was initialised).
- Run the `loot-command` scenario 50 times under ASan/UBSan (add a sanitizer preset to
  `CMakePresets`/build dir `build/pc-asan`); inspect `LootItem::_createItem`,
  `LootItemConditions::allApply`, `LootingEnchantFunction` members for uninitialised state.
- Check: 50 clean sanitizer runs, or a found and fixed cause. Keep the note in `handoff.md` until
  then.

A6. The server binary without the renderer (large; last in this phase).
- Facts: `lce_game` is an OBJECT library linked whole into both programs; `ENABLE_EXPORTS` roots
  every symbol (`CMakeLists.txt`, `lce_add_program`).
- Steps: (1) list client-only sources by directory (`source/game/client/`, `source/game/ui/`,
  audio, input, `platform/pc` window/renderer files) and build a symbol-reference report from the
  server link (`nm -u` per object, or a linker map) showing which shared files reach them; (2)
  cut each reach with an interface the server answers inertly (the pattern `ModHost_wiiu.cpp`
  uses), smallest first; (3) split the CMake source lists into shared, client and server; (4)
  keep one `lce_add_program` so both programs still build from the same shared sources.
- Check: `nm -C build/pc/LCERebornServer | grep -c GameRenderer` is 0; binary size and idle RSS
  recorded in `docs/todo.md` beside the 2026-11-10 numbers; `ctest`, `legacy_stream`, a
  two-process join, and all scenarios still pass. Game behaviour must not change.

## Phase B: the Reborn protocol (`docs/reborn-protocol.md`, `docs/compatibility.md`)

Exists: `source/reborn` (`RbVarint`, `RbFrame`, `RbHello` with `Preamble::Sniff`, `Negotiate`),
test `lce_reborn_test`; `PacketCodec` with `LegacyPacketCodec` and the mod-driven
`ScriptedPacketCodec` (`source/game/network/PacketCodec.cpp`); step 1 audit done.

B1. Done (2026-11-11). The codec is per connection; the metadata needs no new interface, because
Reborn is negotiated on the same socket and port, so the socket answers it for both protocols
(see `todo.md`, the Reborn order, step 1). Finish the contract.
- Make the codec per connection: `Connection` owns a `PacketCodec` created by a factory that
  takes the socket's facts (loopback today). Replace the shared statics in `CreateCodec`.
- Define the metadata interface a second protocol must answer (player id, `isLocal()`, remote
  address: today reached through `getSocket()`), as a small class the Legacy socket implements.
- Check: `legacy_stream` ctest byte-identical; two-process join; all scenarios.

B2. Done (2026-11-11; `reborn-protocol.md`, "What is built", lists what is left). Preamble, hello, negotiation, framed codec (design already reached, see `handoff.md`).
- In-band and lazy, no separate handshake phase: a Reborn client's first write emits preamble +
  hello frame + packet frame; the server's first read takes four bytes, and either reads the
  hello and answers at its first write, or hands the four bytes back to the Legacy reader through
  a prefixed `InputStream` and continues as Legacy. Verify first that the first Legacy client
  packet is at least four bytes (`ServerboundPreLoginPacket`).
- First Reborn codec frames each Legacy-encoded packet (`varint length, varint id, payload`);
  unknown ids are skipped by length.
- Setting `protocols` (`legacy` default; `reborn,legacy` on a server; `reborn` on a client),
  read by the shared server module and the client launch options, same for both servers.
- Hello carries the mod list and the registry number map (moves off the `LCE|Registry` Legacy
  channel for Reborn connections) and the packet-mod fingerprint.
- Check: game-side codec test round-tripping every registered packet; skip test for an unknown
  id; two-process: Reborn client and Legacy client on one server at once; a client with
  different mods refused with the reason before world data.

B3. Batching, zstd and long chat done 2026-11-12 (`reborn-protocol.md`); varint fields and the other packets' limits open. Varint fields, per-packet string limits (chat and commands 1024), batching per tick, zstd
above a size threshold; lift the 8-player session limit for Reborn connections (A4); voice
channel reserved (Phase H). The packet table (id, limits, encoder) is data, one row per packet,
with the Legacy encoder as the fallback so packets convert one at a time.

B4. Interest management (spatial grid, per-player visible set), per-connection byte budget with
priorities, then deltas from the last acknowledged state.

B5. Load test: a headless scripted-client tool (reuse A2's client) connecting N clients; report
tick time and bytes per client at 8, 64, 256, 1024; fix what it shows; `max-players` up to the
protocol limit.

B6. Resume token and replay, then a datagram transport behind the same interface (the P2P
transport already carries a reliable stream).

B7. Test with a real Legacy peer (`neoLegacy` build or the Wii U): Legacy bytes unchanged.
Write each step's result into `docs/reborn-protocol.md` and `docs/todo.md`.

## Phase C: the save format (`docs/saves.md`)

Exists: native container (journal, zstd chunks), `LCERebornSaveTool to-native|to-legacy`,
`lce_savecontainer_test`, mod block palette per chunk (version 13).

C1. Done (2026-11-12; `compatibility.md`, "In saves"). Capability record in `level.dat` and refusal: a save lists the capabilities its data uses; a
build that lacks one refuses with a message naming it; a world using none is written exactly as
the original. Unit test both directions.
C2. "Export for Wii U" in the save's options (screen placement is owner decision D2); the export
runs inside the game (today only the tool).
C3. Wii U byte order and LZX for the exported container; round-trip test against a Wii U save if
the owner provides one.
C4. Done (2026-11-12; measured, no layout change, `saves.md` order step 2). Measure in-memory chunk bytes and what a save copies on the way to disk (step 2 of the
order in `saves.md`); change layout only where the measurement points.
C5. zstd dictionary: measured once at 7 to 11 percent on one world (2026-11-10). Repeat over at
least five worlds and seeds with native chunk bytes; build it only if the gain holds, as a named
capability (a record names its dictionary).
C6. Done (2026-11-12; `saves.md`, "Block numbers no block is registered under"). Loading a pre-palette world with unknown block ids crashes (`LevelChunk::recalcHeightmap`,
null block). Decide: map unregistered ids to air at chunk load with a log line, PC-guarded.

## Phase D: finish the modding API (`docs/modding.md`, `docs/fabric-comparison.md`)

Definition of finished (owner to confirm, D3): every row of `fabric-comparison.md` is checked in a
running game or carries the reason it cannot be; every limit in "Where it is narrower" is closed
or a recorded decision; a mod author can build, run and publish from the docs and `tools/sdk`
alone; the API has a written version and stability promise.

D1. Partly done (events, chat and key binding are scenario steps). Checks that are cheap (do first; each becomes a `mod-examples` step with an example mod):
interaction and entity events fired by a real action (`ServerPlayerGameMode_destroyBlock`,
`useItemOn`, `LivingEntity_hurt|die`), the message API (`handleChat`), particles spawned
(`level:addParticle`), `ModScreen` input, key bindings (`std.input`), `FishingHook_retrieve`
cancel using `entity:get_mFishing()` without the mouse (call the rod's use from the mod),
cancelling a path search on flat ground, reading a player back from a mod's own save format,
`LootTable_fill` cancel (already in the scenario) and `DeathLootTable`.
D2. Partly done (2026-11-12: lists of owned objects as results, `entity:getHandSlots` and three more; lists as arguments, out-parameters behind pointers, by-value objects and the 247 methods without a definition the generator can find are open). Generator conversions (`tools/hooks/hookgen.py`): vectors and arrays of handles and scalars
(generalise the entity-id list into "a list result is ids or values, one per line" plus a typed
list parameter), by-value objects, positions as results, nested classes, item stack types,
callbacks. Each conversion lands with the methods it unlocks listed in the commit.
D3. Partly done (2026-11-12: the search's level, mob and target reach `PathFinder_findPath_2` to `_4`, checked by `path-search`; a `Path` built from a mod's nodes is open). Path replacement: pass the search's level, mob and target to `PathFinder_findPath` (falls
out of D2), and let a mod build a `Path` from nodes.
D4. `ScriptedBlock`, `ScriptedItem`, `ScriptedMob` and the block-entity classes become a Lua
library in `lcestd` over `ModBlock`/`ModItem`/`ModEntity` and the kind tables; delete the C++
that only existed for scripting. Keep every example mod passing.
D5. Texture layouts as data: each model's layout recorded once (ender dragon atlas rectangles,
door slots exist); `registry.block_texture`/`entity_texture` by slot name for every base.
D6. Content gaps, each a decision (D4) then work: tags (none in this game: build a small
name-to-set registry or record "not planned"); a mod particle type (`ParticleProvider` needs a
generated `ModClass`, as mobs have); a fluid renderer; a crash-report section for mods;
changing a game rule (`GameRules::set` is empty in the original); `getEntitiesOfClass`
(by type name); mob equipment slots for `/replaceitem`; `/loot mine` (block drops are code).
D7. Renderer frame as a pipeline object a mod can swap (today stages can be cancelled and drawn
over); do it only when a real mod needs it, behind `rp`-style descriptors, not a new abstraction.
D8. Two packet codecs at once; a packet mod's settings in the frame fingerprint.
D9. Done (2026-11-12; `api/stable.txt`, `modding.md`: the promise, numbers fixed for WebAssembly, `hookgen.py freeze` and `check`). API version and stability: decide what `api=1` covers (generated names follow game method
names, so a rename is a break); write it in `modding.md`; add a compatibility check to the
loader and to `tools/sdk/new_mod.py --check-loads`.
D10. Author documentation pass and the Fabric comparison re-audited row by row against the
Fabric API sources, not the module list.
D11. Wii U mod host: do not start without a Wii U toolchain and hardware (`ModHost_wiiu.cpp`
stays inert).
D12. Clickable chat elements (run a command, suggest, open a link, hover text): owner decision
D5 whether it is a game feature or the mod API's; mods can already click chat lines.

## Phase E: commands (`docs/commands.md`)

E1. In progress (2026-11-12: `/save-all`, `/save-off`, `/save-on`, `/stop`, `/particle`, `/clone`, `/playsound`, `/locate`, `/spreadplayers` and `/bossbar` are game commands, `/data` reads and writes command storage; `/title` is not, because the game draws no titles (`handleSetTitles` is empty in its own source), an interface decision in both servers through `ServerControl`). Gaps table, each with its prerequisite: `/scoreboard`, `/function`, `/trigger` from chat
(systems exist as binary commands); `/clone`, `/testfor`, `/testforblock`, `/testforblocks`,
`/spreadplayers`, `/locate`, `/playsound`, `/save-all|off|on`, `/stop`, `/particle`, `/title`,
`/worldborder` from chat; `/data ... storage` (needs command storage); `/bossbar`, `/teammsg`,
`/schedule`, `/datapack` (need their systems: build or record as not planned).
E2. NBT table: thrown item `Item` tag (needs a renderer read), `BlockStateTag`, item lore and
the `nbt` text component (need JSON text components), book 100 pages (editor scene),
`CustomModelData`; selector arguments `scores`, `team`, `advancements`, `predicate`, `type=#tag`.
E3. Usability: argument hints while typing, history parity between chat and the command block
screen, highlighting extended to numbers, coordinates and selectors; slot and table completion
after `/loot replace`.
E4. Every command gets scenario steps; append binary command ids, never renumber.

## Phase F: command blocks

F1. The command block screen: design with the owner first (D6), then build on the shared menu
model (Phase G) with the chat box's completion and highlighting.
F2. The server setting that turns command blocks off (shared server module, both servers), the
minecart command block, the chain length game rule, the block's name in command output, command
stats.

## Phase G: menus and harness

G1. Partly done 2026-11-12: shared row widgets in `MenuPainter` (`todo.md`, Menus); the page
model waits for a second layout (F1). The shared menu model: pages, rows and values (switch with N states, slider, choice,
button, label, preview pane) with one renderer and one input handler over `MenuNavigator` and
`MenuPainter`; then move `UIScene_DebugOverlayMenu` (759 lines) and `PcShaderMenu.cpp` (2950
lines) onto it, screen by screen, comparing screenshots before and after (`harness.py shot`;
key path in `harness.md`). It must know nothing about shaders or debug options. `UIMenuPage`
(console movie controls) stays for console-styled pages; this model is the free-drawn one.
G2. Done 2026-11-12 (`todo.md`, Menus). The debug overlay preview's undrawn area after the list scrolls: reproduce with the key
path, find the clip or scissor, fix.
G3. Pointer and tap done 2026-11-12; cheaper frames open. Harness: move the pointer (`pointer X Y` request injecting SDL motion) so `MouseLeft` can
click a chosen control; a tap that clicks in the shader and debug menus; cheaper headless frames
need renderer cost reduced (measure where the 2.0 cores at 640x360 go with the profiler request).

## Phase H: interface and social features (design with the owner before any screen)

Design document delivered 2026-11-11 (`interface-design.md`); waiting on the owner's answers.
Deliver first a design document with mock layouts built from existing scenes, covering D7 to D11
below, then build one slice at a time with `screen`/`shot` checks and a disclosed-divergence note
per screen.

H1. Identities (`docs/identity.md`, `UIScene_IdentityMenu`): placement (recommendation: stay in
Help and Options, show the active identity on the main menu where retail shows the gamertag); a
passphrase for the identity file; translations; the person-review checklist in `todo.md`.
H2. Friends (`docs/p2p.md`, `UIScene_FriendsMenu`): two doors only (Join Game row, Players/Invite);
presence as the retail joinable marker; the review checklist; translations. P2P order of work
still open: the peers' own handshake after the punch (step 2 remainder), the relay (step 3),
mini game lobbies and servers (step 5).
H3. Chat: keep the console's opening and tooltips; the Java-style box is the PC keyboard
behaviour (a disclosed divergence); history, mute and block per identity in the Players screen.
H4. Voice chat (nothing exists): Opus codec (vendored, pinned), capture and playback through the
PC audio backend (`PcMixer`), push-to-talk key and controller button, speaker icon in the Players
screen and HUD list, device and volume settings in Audio, proximity as a host option, mute and
block by identity. Transport: a Reborn feature (B3 reserves the channel); never a peer's address
(`p2p.md` threat model); no voice from an unauthenticated peer. Same for both servers.
H5. Moderation shipped with H2 to H4: block list by identity, host kick and mute.
H6. Bedrock interface slices (`docs/bedrock-ui.md`, `todo.md`): touch layout from
`touch-layout.json` and its in-game editor, Bedrock HUD, safe areas, portrait and landscape;
then inventory, crafting, creative, containers, skins, achievements, leaderboards, world list,
chat and command keyboard, prompts and tutorials. Java mode: reserved, do not start.

## Phase I: LegacyEX leads (started 2026-11-12, results in `todo.md`) (`../LegacyEX-PC`, GPL-3, same decompilation origin, quality unchecked)

Method for each: write the claim, find the code there, check whether this project has it,
reproduce the bug or measure the claim here, then implement here with a test; record taken or
not taken in `todo.md`. Never copy code unreviewed.
- Bug leads to reproduce: a retail duplication bug, a retail world crash, Battle chests filled
  only with apples, a guest leaving crashing the host in a mini game, mini game map loading
  timeouts, Tumble function names, spawn egg crash, Glide ghost ten seconds ahead, players
  spawning under the floor in Cavern, water transparency, red hit tint, lever hitbox, offhand
  slot side, armour stand interaction.
- Ideas to weigh with the owner (D12): Massive world size, more than 8 players (with B3),
  in-game key binding, proximity voice (with H4), soft anticheat (compare Phase A limits),
  Golden Age options (old textures, models, water, logos, panoramas, tutorial worlds: a settings
  design and an asset question), extra skin packs, custom skin compatibility.
- Not taken: its central online service with Discord codes, its mod support, its in-game
  keyboard.

## Phase J: evidence-bound items (blocked on material the owner must supply)

J1. `// Parity Not Checked` (21 markers, `grep -rn "Parity Not Checked" source`): needs the
original `Minecraft.Client.rpx` for `tools/measure/parity.py`. With it: check each, remove the
marker or correct the fix.
J2. World generation compared with retail for a seed: needs a retail world save.
J3. A real Legacy peer for B7 and a Wii U save for C3.

## Phase K: platforms and last items (order fixed by `todo.md`)

K1. Android 10 to 17: build target (NDK, SDL 3, GLES, Gradle, signing), platform seams
(lifecycle, safe areas, storage, assets, keyboard), one APK; needs H6.
K2. Windows: build and run, check every `_windows` file; after everything above.
K3. Microsoft account ownership check: lowest priority, unpublished project.
K4. Asset transcoding to lighter formats with no perceptible loss: last of all.

## Standing direction (applies inside every phase, never as its own rewrite)

- Data-oriented design where the project is free to restructure (mod layer, PC platform, tools,
  new subsystems): definitions resolved once into id-indexed rows, instances hold state only.
  It does not reach the matched decompiled classes.
- A measured duplication pass after each phase (the block-scan script approach used on the mod
  code), fixing what it finds in project-owned code only.
- Keep `docs/handoff.md` and `docs/todo.md` current in the same commit as the work.

## Decisions for the owner (ask before the package that needs it)

D1. More than 8 players: Reborn-only (recommended) or also a non-Legacy-compatible session mode.
D2. Where "Export for Wii U" sits in the save options.
D3. The definition of a finished mod API above.
D4. Tags, mod particle types, fluid renderer, game rule changes: build or record as not planned.
D5. Clickable chat: game feature or mod API.
D6. The command block screen design.
D7. Identities placement. D8. Friends doors and presence. D9. Chat divergence and moderation
placement. D10. Voice chat scope and settings placement. D11. Bedrock slice order.
D12. Which LegacyEX ideas to pursue.
D13. Whether unknown block ids in a chunk become air on load (C6).

## Execution order

1. A1, A3, A5 (small). 2. D1 (cheap API checks). 3. B1, B2. 4. A2 (the client tool also serves
B5). 5. The Phase H design document, sent to the owner early because it waits on them. 6. C1,
C6, C4. 7. D2, D3, D9, D10. 8. E1 to E3 with scenarios. 9. G1, G2, G3 (G1 before F1 and before
new Phase H screens). 10. B3 to B6, A4, H4. 11. D4 to D8. 12. F, H1 to H3, H5, H6. 13. I
interleaved wherever a reproduction is cheap. 14. A6. 15. C2, C3, C5, B7 and J as material
arrives. 16. K.

Do not start: the Wii U mod host, the Java interface mode, a central online service, a rewrite of
decompiled classes toward data-oriented storage.

## Critical files

- Server: `source/game/server/{AccessList.hpp,ServerAccess.*,ServerWatchdog.*}`,
  `source/platform/pc/{PcServer.cpp,PcServerAccess.*,Main.cpp}`,
  `source/net/CPlatformNetworkManagerLan.cpp`, `source/game/network/{Connection.cpp,
  ServerConnectionListener.cpp,PacketRead.hpp,Packet.cpp}`, `source/game/data/NbtBudget.*`,
  `source/game/command/chat/*`, `tests/access_test.cpp`.
- Protocol: `source/reborn/*`, `source/game/network/PacketCodec.*`, `source/net/NetLink.hpp`,
  `tests/reborn_test.cpp`.
- Saves: `source/savecontainer/*`, `source/game/world/chunk/OldChunkStorage.cpp`,
  `source/game/core/ModBlocks.cpp` (palette), `tools/measure/saves.py`.
- Mod API: `api/lce.api`, `tools/hooks/hookgen.py`, `source/mod/*`, `source/game/core/Mod*`,
  `docs/examples/mods/*` (`lcestd/std.lua`), `tests/mod_test.cpp`, `tools/sdk/*`.
- Interface: `source/game/ui/{UIMenuNavigator,UIMenuPage,UIScene_DebugOverlayMenu,
  UIScene_IdentityMenu,UIScene_FriendsMenu}.*`, `source/platform/pc/PcShaderMenu.cpp`,
  `source/game/core/{ChatScreen.cpp,ModScreen.cpp}`, `source/identity/*`, `source/p2p/*`.
- Tools and docs: `tools/harness/*`, `CMakeLists.txt`, `docs/{todo,handoff,pc-platform,
  harness,modding,fabric-comparison,commands,reborn-protocol,saves,p2p,identity,bedrock-ui}.md`.

## Verification of the plan as a whole

Each package ends with: both builds and `ctest` green, `harness.py check` green twice, the new
scenario or unit test for that package, a two-process check where a server path changed, the
doc line stating what was checked and what was not, and `handoff.md`/`todo.md` updated. A phase
is closed when its `todo.md` section says done or records the decision that replaced it.
