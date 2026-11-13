# TODO

The work this project has committed to, cheapest first, so the most items
finish for the least effort. A later item still waits for any earlier one
it depends on: peer-to-peer friends (over Legacy, the Reborn protocol is
separate) and the Bedrock interface come before Android, Android before the
Windows build, and transcoding the assets comes last of all. `AGENTS.md`
is the working agreement; this file is the list.

## Menus

The shader settings and debug info menus now share `MenuNavigator`
(`source/game/ui/UIMenuNavigator`): targets and layers, scrolled views,
hit testing, keyboard movement, keeping focus visible, the animation clock
and flashes, beside the shared `MenuPainter`.

- Shared row widgets (2026-11-12): `MenuPainter::Switch` (segments, the chosen one coloured and
  able to pop) and `MenuPainter::ValueRow` (label, value box, slider, flash, modified mark, focus
  ring). The debug overlay's switches, the shader menu's Shaders toggle, its option cells and its
  settings rows all draw through them; screenshots before and after match pixel for pixel outside
  live values and the world behind the menu (the option cells need a loaded pack, not checked).
  Layouts stay in each scene because they share nothing (a categorised list, a grid with pages,
  a carousel), and so does pointer input (the shader menu has modals, a hidden mode and slider
  drags). A page model is built when the command block screen (F1) gives it a second layout.
- Fixed 2026-11-12 (G2): panels drawn after a clipped list were cut to whatever scissor an earlier
  draw had left, because `MenuFrame` restored the rectangle the renderer last recorded instead of
  the viewport's own. It now restores its viewport area. Seen with the debug overlay opened
  straight from a world: the preset panel and the right of the preview were missing.

## Harness

- Done 2026-11-12: `pointer X Y` moves the mouse so `key MouseLeft` clicks a chosen control, and a
  touch tap clicks in scenes that read their own pointer (the shader and debug overlay menus);
  until then a tap there only moved focus, because the touch layer made clicks for container menus
  alone (`SetMenuReadsClicks`).

The harness is how agents and people debug the game: boot it headless, drive
one feature, read the result, without clicking through menus.

- Cheaper headless frames. Parity with the portable-lce harness is done
  (world creation, `pause` and `resume`, a frame cap, the `screen`
  summary, muted headless runs), but llvmpipe draws a 1280x720 frame for
  about four cores, so a running headless client still costs that much. `harness.py up --size WIDTHxHEIGHT`
  (the game's debug option `--render-size`) draws at a smaller size: measured on 2026-11-10 over 10 seconds,
  a headless client used 3.3 cores at 1280x720, 2.0 at 640x360 and 1.8 at 480x270 (the rest is the game's
  own work on the CPU), and the interface lays out at the smaller size. A cheaper frame than that needs the
  renderer's cost reduced, not its size.

## Parity checks

- Check every `// Parity Not Checked` fix against the original and remove
  its marker: `grep -rn "Parity Not Checked" source`. The first is
  `MiniGameDef::GetOptionsCount`, which read past the end of an empty
  matchmaking filter list and crashed the mini game host settings screen
  on PC; the Wii U may read that address without faulting.

## Commands

The target is all of Java Edition 1.14, commands and NBT syntax, as
`docs/commands.md` records it.

- The rest of the NBT table and the gaps table in `docs/commands.md`, each with the system it
  needs.
- Command usability matches modern Java Edition (1.23.6 and 1.24): the
  suggestion list as you type, argument hints, completion with Tab,
  highlighting of invalid input, and history, the same in chat and in the
  command block screen. The 1.14 target above is for syntax; this is for
  how typing a command feels. Suggestions list names with their `minecraft:` prefix.
  Highlighting of invalid input is done for a finished word that is not a command, or not a
  name its argument kind lists (items, blocks, entities, effects, enchantments, game modes,
  loot tables, slots and fixed choices): the word and the rest of the line turn red, checked
  on screen. Numbers, coordinates, selectors and text are not judged, and an unfinished
  word is never red.
- Clickable chat elements, as Java's text components do (run a command,
  suggest a command, open a link, show hover text). Decide first whether
  this is a game feature or something the modding API provides. Mods can now
  click chat lines (`input` hook); a game feature would build on that.

## Command blocks

Built: the logic of impulse, repeating and chain blocks, conditional mode and Always
Active, driven by redstone and by data (`docs/commands.md`, Command blocks).

- The command block screen. Stop before writing it: the interface is designed
  with the project owner first.
- The server setting that turns command blocks off, the minecart command block, the
  chain length game rule, the block's name in command output, and command stats.

## Standalone server

- A dedicated server with minimal overhead and very few requirements: no
  window, renderer, audio or UI code linked in, low memory, and nothing to
  install beyond the binary and the game content it needs. Start from
  `LCERebornServer` and measure what it still carries. Measured on the debug
  build on 2026-11-10: 39 MB of code (`size`), and `ldd` lists only libc, libm,
  libstdc++ and libgcc, so nothing is loaded from SDL or a GL library. The
  code is linked in statically all the same: `nm -C` finds 5544 SDL symbols,
  406 `GameRenderer`, 392 `LevelRenderer`, 593 `ConsoleUIController` and 2731
  `Renderer` symbols, because shared game code (`Level`, `Minecraft`) still
  reaches the client classes. The release build (`build/pc`, 2026-11-10): 57 MB on
  disk with symbols (47 MB of code by `size`), the same static renderer, UI and SDL symbols (5383 SDL,
  419 `GameRenderer`, 407 `LevelRenderer`, 587 `ConsoleUIController`), 103 MB resident for an
  idle server with a new world, and 24 clock ticks of CPU in 10 seconds with nobody connected (about a
  quarter of a core). Getting the renderer out of the binary is the open part, and two things in
  `CMakeLists.txt` explain why it is there: `lce_game` is an OBJECT library, so
  `lce_add_program` links every game object into `LCERebornServer` (an archive would pull only
  the ones something refers to), and the programs set `ENABLE_EXPORTS` (`-rdynamic`, for the
  function names in a crash report), which makes every global symbol a root, so
  `-ffunction-sections` with `--gc-sections` alone would drop almost nothing. The work is to
  list the client-only sources (the renderer, the UI scenes, the audio, the input) and leave
  them out of the server target, which means the shared game code must stop reaching them;
  `nm` on the server shows how far that is.

## Saves

The plan, with the numbers that drive it, is `docs/saves.md`. Measuring is done
for the container and the chunk stages. The native container (an appended journal, zstd
chunks) is built and PC writes it; open: the save screen's Export entry, the Wii U byte order, and a zstd dictionary for chunks. The data layout comes first only where a
measurement points at it.

## Reborn protocol

Reborn is the second wire protocol, beside Legacy and behind the same network
contract (`compatibility.md`, On the wire). The design is `docs/reborn-protocol.md`;
the varint, frame, preamble, hello and negotiation library is written and tested
(`source/reborn`), and is not yet wired into `Connection`. Legacy stays as it is and is never extended. How a
connection chooses is decided there (a 4-byte preamble on the same port). Take these
in order:

1. The contract: game code talks to one interface and does not know which
   protocol is in use. `Connection` and `PacketCodec` are the start; what is
   still reached around them has to be found and closed. Audited on 2026-11-10:
   `Packet::readPacket` and `Packet::writePacket` have one caller, `LegacyPacketCodec`, and
   `Connection` is the only user of a socket's streams; the rest of the game reaches a
   socket for metadata only (`getSocket()` for the player id, `isLocal()`, the remote
   address), which a second protocol has to answer too. Done on 2026-11-11: each `Connection`
   owns the codec `CreateCodec` makes for it and deletes it. The metadata stays with the
   socket: Reborn is chosen in-band on the same socket and port (step 2), so it is a codec over
   that socket, not a second transport, and the socket answers the player id, `isLocal()` and
   the address for both. A datagram transport (step 6) is the first thing that needs that
   interface, and gets it then. Choosing a codec by preamble is step 2.
2. Preamble, framing and negotiation, with the codec and skip tests. Built on 2026-11-11
   (`reborn-protocol.md`, "What is built"); left: a game-side test that round trips every
   packet, retrying as Legacy, the registry hash in the hello.
3. Varint fields, per-packet string limits (chat and commands 1024), batching, zstd.
4. Interest management and per-connection budgets, then deltas.
5. The 1024-player load test and what it points at; no hard player limit.
6. Resume, then a datagram transport.
7. A test with a real Legacy peer, so the contract is shown to keep Legacy byte for byte.

## Modding API

Built: Lua and WebAssembly mods loaded at start and reloaded while the game runs, with
chat commands, chat, join, leave and tick events (`docs/modding.md`). The target is Fabric-class flexibility, designed in `docs/modding.md` (Direction):
the interface description and hook generator are built (`api/lce.api`, three hooks and
11 calls to start), more hooks and calls follow; mod dependencies, ordered hooks, events between mods and host limits are built; then engine subsystems as replaceable services (renderer passes, light, terrain), mod-defined events, the dependency
graph, open registries, then client content. The command-block work
below is designed so command blocks run through the same command path mods use.

### Mod API: where it stands

The API is generated from the game's own code, not written call by call (`docs/modding.md`, "Game classes as the API",
"The game's own methods, generated"): `override Class from header` makes a hook for every virtual method and a `ModClass`
template; `expose Class from header` makes every public method callable, with typed handles, static methods, enums and
class-hierarchy upcasts. Built on that: redstone, the packet codec, the chunk store, block kinds (door, trapdoor, button,
plate, lever, ladder, fence), mob kinds (eight bases), item kinds (food, tools, swords).

Technical debt left, in the order to remove it:

1. Done: `ModApiFunctions.cpp` is gone and `api/lce.api` declares no function ("Hand-declared functions audit" below).
2. `ScriptedBlock`, `ScriptedItem`, `ScriptedMob` and the block-entity classes are hand-written; they should be a Lua library
   over `ModBlock`/`ModItem`/`ModEntity` plus the kind registries.
3. Decided, not debt: the kind tables stay, as the one place that holds the game's variant rules, and what a mod derives from is a prototype
   ("Content definitions" below).
4. Conversion gaps in the generator: arrays and vectors (a vector of entities is done as a result: the ids one on each line, and `std.world.entities` makes handles of them; a list parameter the game fills is passed as NULL), by-value objects, callbacks, nested classes, positions as results, and the item
   stack types; every skipped method is listed at the foot of its generated header.
5. Subsystems still not behind an interface: the renderer's frame (stages can be cancelled and drawn over, the classes are exposed, but there is
   no pipeline object to swap). The UI scene factory is `ConsoleUIController_NavigateToScene`: `docs/examples/mods/pausemenu` cancels the pause menu and opens a page of its own in a running game (Escape showed "My pause menu" with a Resume button); only a scene's `sceneData` pointer is not passed to the hook. The level and player data in the save are behind `DirectoryLevelStorage_save`, `_load`, `_loadPlayerDataTag` and `_saveLevelData` (in a running game a test mod counted a player load on joining, then a player save and four level saves after the harness `save`); a mod that replaces how a player is stored cancels the hook and writes with `file.write`, but the loaded `CompoundTag` result is the game's own, so reading a player back from a mod's own format is not checked. The pathfinder and the move control are behind hooks now (`PathFinder_findPath`, `MoveControl_tick`, `MoveControl_setWantedPosition`: with two mobs about, a test mod counted 12557 ticks and 30 searches in a running game); the hook's `result` is a settable `Path` handle, so a mod can answer a search with a path it got from another call (`PathNavigation:createPath` and its siblings), but it cannot build one from its own nodes (the generator has no array of nodes), and the search's level, mob and target are not passed to the hook (the generator drops those parameter types), so a substitute search cannot be aimed from inside it. Cancelling a search and setting no result was not checked: the test cows fell out of the world before the comparison.
7. The packet codec is chosen per connection only as loopback or remote (`packet_encode(server, loopback, ...)`), and a peer that runs the same packet mod with different settings is not told apart from one that runs the same (the frame fingerprint covers the mod's id and version); a modded server with an unmodded client was checked with two processes.

### The rule: no hand-placed hooks (the Fabric rule)

Every hook that copied a game function has been replaced by the woven `Class_method` and `Class_method_after`
(`intercept Class from file.cpp`). No hook is declared by hand in `api/lce.api` for anything but the engines a mod replaces (`provider`).
The mod loader's own events were three hand-declared hooks, then methods of classes that existed only to be hooked. Now none is: registering is a hook on the game's own `Item::staticCtor` (the registries are open
around it), the screen events are `ModScreen::Draw` and `ModScreen::Input` (which carry the pointer drawing and the close-on-Escape the game does itself),
the chat pointer is `ChatScreen::Pointer` (which carries the wheel scroll), and a console menu is the scene's own handlers. A hook belongs on code that
does something; a function written only to be hooked is the debt to avoid.

Weaving cannot yet do constructors or a parameter that is an array (other than bytes) or a by-value object; a static method, an unnamed
parameter and the return value in the after hook are done, and a parameter it cannot convert is left out of the hook.

### Content definitions: prototypes, kinds and texture layouts

Design, decided; not built yet except where marked.

What a mod wants is object-oriented authoring over data-oriented storage: "this block is a door", "this mob's
texture is laid out like the ender dragon's", and everything not overridden (behaviour, sounds, texture assembly,
properties) comes from the parent.

- A kind table is the wrong long-term shape. `ModBlockKinds` and `ModItemKinds` are hand-written name to constructor lists
  (they share `ModKindTable.hpp` now, so the lookup exists once), and what they really encode is "which game class, built with
  which arguments", which the C++ already says. Fixing the arguments in a string per kind would only move the code into a
  generator input.
- The replacement is a prototype. A definition is `{parent, overrides}`. A parent is either a game class (found from the exposed
  classes the generator already knows, the same list `ModBlock<Base>`, `ModItem<Base>` and `mobbase` come from) or a definition
  already registered, a game one or a mod's. Registration resolves the chain once into an immutable definition (cycle and missing
  parent rejected with a message, depth unlimited, an explicit default distinct from an absent value), and the block, item or
  entity is built from it. `registry.block_of(name, parent, ...)` is the first step: `parent` is a kind today.
- Constructor arguments: decided to keep the kind tables, as the single place that holds the game's variant rules. Some rules are game
  knowledge that no property supplies (a pressure plate is `EVERYTHING` for wood and `MOBS` for stone, a button is a different class
  by material), and a generator fed with them would only move the same branches into a string in `api/lce.api`. So a kind stays one
  entry, now built on `ModKindTable.hpp`, and what a mod derives from is a prototype (done for blocks, items and mobs), not a table
  entry. Revisit only if a kind appears that no rule of this shape covers.
- Texture layout belongs to the parent and is data: a door has `top` and `bottom`, the ender dragon's model has its atlas
  rectangles. A mod supplies pictures by slot name; the layout (where each slot lands in the atlas, how a model's UVs read it)
  is inherited and overridable, so nobody redoes the maths. It extends `registry.block_face` and `registry.block_texture`
  (a slot name instead of a side) and needs each model's layout recorded once as data.
- Behaviour and sound are inherited by being the parent's class: the `Block_*`, `Item_*` and `Entity_*` hooks already answer a
  mod object as the parent class does.
- Runtime stays compact: a resolved definition is a row indexed by the registry id, and an instance keeps only its own state.

Done: the loader's events are woven symbols, kinds share one lookup, and `registry.block_of` takes a registered block as its parent
(`ModBlocks::Inherit` copies properties, pictures, shapes, drops and settings; the call's own material, hardness, resistance and light
are the overrides; the parent must be declared earlier, so a cycle cannot occur). `registry.item_of` does the same for items (class and stack size). Mobs derive from a registered mob the same way (`registry.entity`). Texture slots are done for blocks made with `block_of`: each kind lists the pictures its class reads (`ModBlockKinds::Slots`, a door has `upper` and `lower`), `registry.block_texture(name, image, slot)` supplies one, the block's icon name is set so the class finds them by its own naming, and a derived block wears its parent's. The item a kind places with a class of its own (the door) takes `registry.item_icon` too. Mob textures are done too: `registry.entity_texture` gives a mob a picture laid out as its base mob's own texture is, a derived mob wears its parent's, and `EntityRenderer::bindTexture` is the one place it is read. A scripted block has the six sides and states as its slots, which is what its shape reads, so it needs nothing more.

### Hand-declared functions audit

Result: `api/lce.api` declares no function and `ModApiFunctions.cpp` is gone: every call is a method of a game class or namespace (`expose`), of a loader class
with a documented method and an `alias` line (`registry.*`, `surface.*`, `audio.*`, `gui.*`, `canvas.*`, `sound.decode`), or Lua in `lcestd`. 87 hand-declared
functions with host code at the start of this work.

How the rest went, so the next call follows it: a duplicate of a generated symbol was deleted; a convenience over several calls moved to `lcestd`; a game
object the generator could not reach got the generator feature that reaches it (static members and constants as getters, namespaces, shared pointer results,
`family Derived of Base`, an `owner` parameter, a class whose handle lives until the mod frees it (`persistent Class`, and `surface` and `stream`, which share the same store: 64 live objects per mod), `shared_cast A to B` (an owned object asked for as another class it is), `owned Class` (a class whose objects are held by shared pointer: the handle owns what a mod makes until a call takes it, so a stack made in Lua is never freed twice), `alias`, descriptions from header comments, nil for no object, an entity handle where a pointer is taken).

### Data-oriented direction: where it stands

`AGENTS.md` ("Data-oriented direction") is the rule, and it applies where this project is free to restructure: the mod layer, the PC platform, the tools and any new
subsystem. Mod content follows it: a definition is `{parent, overrides}` resolved once at registration into a table the runtime reads by id (`ModEntities::Find`
is a shift and an index, not a scan, on the render and tick paths; the chunk palette of mod blocks is a bitmap over the block id range; a derived block, item or
mob copies its parent's resolved definition), and the Lua API tables are built when first used. The game's own block, item and entity classes are the matching
decompilation of the Wii U executable (`AGENTS.md`, "The method"): turning their virtual getters into id-indexed rows would change the compiled code that parity checks, so
that is not done and not planned; it would be a decision about the method, not a refactor. A new content system, or a subsystem rewritten for a behavioural
reason, starts as a definition row indexed by id with the instance holding only its own state.

### Mod interfaces still to do

Decisions, with their reasons, so they are not mistaken for gaps:

- A mod key binding is read by name (`std.input`) and kept in the mod's own configuration (`std.config`), so a player rebinds it in the mod's settings; it is not a row of the
  controls screen. That screen builds each row by hand against the game's fixed action enum, and a dynamic row would need new storage and a new rebind flow for a feature
  Fabric leaves to each mod too.
- Light and shader passes for a picture drawn in the world: light is the `light` argument of `canvas:image` and the colour of a triangle's corners, and a shader pack acts on the
  whole frame, a mod's draws included, so there is no separate pass to expose.
- The number map moves into the Reborn hello when the Reborn protocol is built (`AGENTS.md`, Network); until then it travels on the `LCE|Registry` Legacy channel, whose bytes
  are frozen.
- Pointer input for a mod's picture on a screen of the game's own: the chat hands its pointer to `ChatScreen_Pointer`, and a mod that wants another screen's pointer hooks
  that screen's own handler (every screen class is interceptable); there is no second pointer system.

Open: nothing in this list is a gap in what a mod can do that I know of; new ones are added here when found. `MenuNavigator` is exposed (`persistent MenuNavigator`, nested
structs of numbers as their numbers), an item has a picture and no other model (items are pictures in this game), and the registry sync compares items, blocks, mobs and
dimensions by name.

## Sanitizer findings

From the sanitizer build (`harness.md`) playing a world on 2026-11-12. Fixed: signed overflow in the
ported Java arithmetic (hashes, random numbers, seeds, block position packing), now defined by
`-fwrapv` on GCC and Clang; `Language::getInstance` returned a null pointer the Java-style screens
called members on; a missing registry key (see LegacyEX leads); calls through a function pointer of
another type (the 262 control factories in `FJ_Factory.cpp`, now one typed thunk each, and the
sign-in callback, which returns a value its pointer type does not). Open:

- Unaligned reads and writes through cast pointers: `DLCManager.cpp` (the pack reader, about 30
  sites), `DLCAudioFile.cpp`, `StatsCounter.cpp`. x86 and 64-bit ARM accept them for plain loads, so
  nothing fails today, but a compiler may assume alignment; each wants a byte copy (`memcpy`) under
  the PC guard, file by file, with a pack loaded before and after. Bounds are not the problem: on PC
  every pack goes through `SwapDLCDataToHost` first, which walks the whole structure with checks
  before either parser reads it.
- A `_eTerrainFeatureType` read as 3, outside the enumeration (`CMinecraftApp.cpp`).

## LegacyEX leads

`../LegacyEX-PC` (GPL-3, same decompilation origin) lists fixes without history, so each claim is
checked here before anything is taken (plan, phase I).

- Spawn egg crash: reproduced 2026-11-12 and fixed. A spawn egg naming no creature used on a pig
  crashed the server: `SimpleRegistry::get` used `at()` on a name never registered, and with Boost's
  exceptions off `at()` returned garbage. A missing key now reads as nothing, as in Java, and
  `boost::throw_exception` logs and stops the game on PC instead of returning, so any other missing
  key shows where it happened. Scenario `spawn-egg`. Found with the sanitizer build: the plain
  build's crash report only showed a call through address 0.
- Lever hitbox: not taken. Its boxes are the ones this game already has, number for number.
- Red tint on a hurt mob: not reproduced (a pig hurt by instant damage turns red).
- Water transparency: not reproduced (a block under water shows through it).
- Armour stand interaction: not reproduced. A quick click takes a helmet off a stand and puts one
  on. Holding the button repeats the use every 5 ticks, as Java does, so a held click takes the
  helmet and puts it back (in creative it places a copy, as creative does).
- Not checked yet: the offhand slot's side (needs a retail capture), the
  retail duplication and world crash bugs (no detail given), and the mini game items (Battle chests,
  a guest leaving, map loading, Tumble names, Cavern spawns, the Glide ghost).

## World generation

- Compare generated terrain with the retail game for the same seed. It needs
  a retail world save as the reference; `docs/measuring.md` records what is
  measured today and what is not.

## Bedrock interface

The game ships one interface today, the console one. It gains two more
modes and keeps the current one:

| Mode | State |
| --- | --- |
| Legacy | the interface as it is today, the default |
| Bedrock | being built, one slice at a time, listed below |
| Java | planned, not started; nothing is written for it yet |

The mode is a per-profile setting. A slice is finished when it runs in the
harness, its layout and behaviour are drawn from Bedrock's own data where
that data exists, and its differences from Legacy are written down.

### Done so far

- The interface mode is a per-profile setting (`eGameSetting_UIMode`, packed
  into the spare profile bits so saves keep working), with `--ui-mode` on the
  command line for a single run. Java stays reserved.
- Screen selection goes through a factory: in Bedrock mode `UILayer` asks for
  a Bedrock screen first and falls back to the Legacy one. A screen with no
  Bedrock implementation is still reachable.
- The shared overlay renderer now takes its target framebuffer and
  orientation, so a native screen can draw into the game's UI buffer.
- The font and text renderer moved out of the touch controls into `PcFont`,
  so the touch controls and the Bedrock interface use one implementation.
- `PcOreUI` draws the OreUI palette, rem scale, panels, raised buttons,
  rows, switches, sliders, the header bar and the title artwork. Its
  geometry follows Cinnabar's home, pause and settings screens.
- The Bedrock home screen is the title screen: title, LET'S PLAY panel with
  Play, Servers and Settings, the Skins entry, and the character panel. It
  uses the game's single-line wordmark.
- The Bedrock pause menu matches Cinnabar's pause screen: Resume game,
  Settings, Save & Quit, and the character panel. Save & Quit reuses the
  Legacy exit dialog and save flow.
- The Bedrock settings screen has General, Video, Audio, Controls and Touch
  sections, with switches, sliders and choices backed by `GameSettings` and
  written to the profile.
- Harness: `screen` opens a named screen, `touch` sends fingers, `state`
  reports the mode and whether a pause screen is open.
- The screens, their input and their differences from Cinnabar are recorded
  in `docs/bedrock-ui.md`.

### 1. In-game controls and HUD

- Touch layout: the stick, buttons and spacing come from a data file
  (`touch-layout.json`), not from constants, with Bedrock's proportions as
  the defaults.
- An in-game editor for that layout: drag controls, resize them, choose
  which are shown, reset to the defaults.
- Bedrock's HUD: hotbar, hearts, hunger, armour, experience, the selected
  item name, and the tooltip style, laid out as Bedrock lays them out.
- Safe areas: keep controls and HUD clear of notches, rounded corners and
  the gesture bar.
- Portrait and landscape: the controls and HUD follow the orientation.

### 2. Menus and screens

- Inventory, crafting and the creative menu, with Bedrock's layouts.
- Containers: chests, furnaces, crafting tables, enchanting, trading.
- Skins, achievements, leaderboards and the world list.
- Chat and the command keyboard.
- Prompts, tooltips and tutorials in Bedrock's words and icons.

## Friends and peer to peer

- A peer-to-peer friends system that is not open to abuse: friend lists,
  presence and invitations, joining a friend's world, mini game lobbies and
  servers. Identity and invitations must be authenticated, nothing a peer
  sends is trusted without checking, and no peer learns another's address
  unless that player has agreed. It is a transport that carries Legacy today and
  Reborn when that exists.
  The design is `docs/p2p.md` (a tiny rendezvous server, key-based
  identity, an authenticated and encrypted handshake). Build order: the
  identity and handshake library, the rendezvous server, the relay, the
  friends and lobby screens, then mini game lobbies and servers.

## Friends screens, review by a person

The screens in `p2p.md` (Screens) were built and smoke-tested, not reviewed. A
person checks, with a controller and with mouse and keyboard, in single and split
screen: the Friends row in Join Game; the Friends screen (own code, Add Friend,
list, A/X/Y/B); Add Friend with a good code, a bad code and a code the server does
not know; the nickname change; removing a friend; inviting from Players/Invite
while hosting; and Join Game showing an outdated lobby. Wording in other
languages still needs translating.

## Identities, review by a person

The Identities screen and its entry in Help and Options are built and smoke-tested,
not reviewed (`identity.md`). A person checks, with a controller and with mouse and
keyboard: the button's place in Help and Options; the carousel turning (not observed
moving in the headless client); using, creating, renaming and removing; the Skin button
opening the skin screen and the skin being kept; Skin file... and Open identities folder
(system windows, not exercised); the grey placeholder (not shown with the current
actions, as no entry is unnamed); the
messages when a name is not valid, when the identity is online, and when a file does
not verify; and that moving an identity file to another machine keeps the name, skin,
UUID and friend code. Still open: a passphrase for the file, wording in other languages.

## Android

- Android app, targeting Android 10 through 17. The phone build is the PC
  build with a phone-first interface, so it needs four pieces: the build
  target (NDK, SDL 3, GLES, Gradle, signing), the platform seams a phone
  needs (lifecycle, safe areas, storage, assets, on-screen keyboard), one
  APK that installs and runs on Android 10 through 17, and the Bedrock
  interface above.

## Windows build

- Build and run on Windows, checking every `_windows` file, once all the
  items above are done.

## Microsoft account ownership

Lowest priority: the project is not published and will not be for a while.

- Verify ownership through a Microsoft account: an account that owns Bedrock
  owns this game, because every console platform moved Legacy Console
  Edition players to Bedrock.

## Assets

- Transcode every image and audio file to lighter formats with no
  perceptible loss, to cut CPU load, VRAM use and storage significantly.
  This is the last item.

### Mod-built console scenes

Done through the page seam the game already had: `UIScene_CustomMenu` builds its controls from a `UICustomMenuPage`, `UIMenuPage` is a page held as data (the
rows are given to it, so the game's own pages can use it too), `ConsoleUIController::NavigateToMenuPage` hands one to the scene, and a mod listens to the
scene's own handlers, which are woven. The scene's controls are the button, checkbox, slider, value and label its movie has; that is the console menu, and a mod that needs another control opens a
`ModScreen`, which draws anything with the game's `MenuPainter`. That is the decision: two kinds of screen, one for the console's look and one for free drawing, not a
third scene class that would need a movie per layout.
