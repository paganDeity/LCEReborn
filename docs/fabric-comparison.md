# The mod API next to Fabric's

Fabric is the reference for what a mod can do. This page puts its API modules beside the way each is done here, with an honest status. "Generated" means the call or hook
comes from a game class by `expose` or `intercept` with no host code (`docs/modding-api.md`); "lcestd" is the Lua layer in `docs/examples/mods/lcestd`. Checked means a running
game or a test proved it (`ctest`, and `python3 tools/harness/harness.py check mod-examples`); the rest is built and not covered by a scenario.

| Fabric | Here | Status |
| --- | --- | --- |
| Mixins (hook any method, head, tail, cancel, change an argument or result) | `intercept Class from file.cpp`: `Class_method` and `Class_method_after`, woven at build time from the source | Checked: many hooks; limits: weaving needs the class's source file, a parameter the generator cannot convert is left out |
| Accessors and invokers (private members) | field getters and setters for an intercepted class (`get_mX`), any exposed public method | Checked |
| Lifecycle events (server starting, ticking, stopping) | `MinecraftServer_tick`, `Minecraft_tick`, `Item_staticCtor_after` (registries open), `PlayerList_add` and `_remove` | Checked |
| Registry (`Registry.register`) | `registry.item`, `block`, `entity`, `item_of`, `block_of`, `recipe`, `sound`, `dimension`, `fuel`, `creative`, `translate`: documented static methods of loader classes | Checked; blocks, items and mobs derive from content already registered |
| Object builder (block settings, item settings, entity types and attributes) | arguments of the `registry.*` calls, `registry.block_property`, `block_shape`, `block_drop`, `block_entity`, `entity` health and speed | Checked |
| Block entities and their renderers | `registry.block_entity` (a store, an inventory), `registry.block_model` | Checked |
| Entity renderers and models | `registry.entity_model`, `entity_texture`, `entity_name` (spawn egg), `entity_spawn` | Checked; head and leg motion not covered by a scenario |
| Data attachments | `std.data.entity_set` (saved with the entity), `world_load` and `world_save`, a block's store | Checked |
| Networking (custom payloads) | `std.net.broadcast` and `send_to_server`, hooks on both handlers | Checked in the integrated server; between two machines the same packets |
| Command API | `mod.command(name, fn, usage, permission)`: a game command listed by `/help` | Checked; arguments are declared as kinds in the game's own words (`targets`, `coord`, `number`, `item` ...) that the command box completes by (in a running game, `/givepoints @` offered `@p`, `@a`, `@r`, `@s` and `@e` for a command declared `targets number`); branches are `std.command.tree` (a `/shop buy|list|help` command offered `buy`, `help` and `list` in the box in a running game), a branch declares its own argument kinds (`/shop buy ` offered items in a running game), and receives the rest of the line as text |
| Key bindings | `std.input.bind(name, fn)`, a key kept in the mod's configuration | Checked; not a row of the controls screen (decision, `docs/todo.md`); the `mod-examples` scenario presses G and `docs/examples/mods/keys` counts it |
| Screens (`Screen`, widgets) | `ModScreen` (free drawing with `MenuPainter`) and console menus (`UIMenuPage`); `MenuNavigator` | Checked |
| HUD and world rendering (`HudRenderCallback`, `WorldRenderEvents`) | `Gui_renderOverlay`, `GameRenderer_renderWeather` and the other stages, `canvas`, `gui` | Checked |
| Rendering pipeline replacement | every render stage is a hook that can cancel; the example skips all of them and draws its own | Checked |
| Resource loader (language, textures, sounds) | `registry.translate` and `std.lang.load`, `surface`, `registry.sound` | Checked; mod models are text, not a resource pack |
| Sound (`SoundEvent`, positional audio) | `registry.sound`, `std.world.play_sound`, `std.audio.play` (loop, stop, volume, pan) | Checked; headless runs are muted, so playback itself was not heard |
| Biomes and world generation | `BiomeInitLayer` and `Biome` hooks, `generate_terrain`, `std.gen` | Checked |
| Loot API (`LootTableEvents`) | `LootTable_fill` (chest or minecart fill) and `Mob_dropAllDeathLoot` (a mob's loot-table drops), each with `_after` | `Mob_dropAllDeathLoot` fired in a running game when a cow died (the hook ran twice for two deaths); `LootTable_fill` fired in a running game when a chest with a `LootTable` was opened with `key MouseRight`, and the `mod-examples` scenario now checks it with `docs/examples/mods/lootwatch` (it also counts `Mob_dropAllDeathLoot` and `FishingHook_retrieve`, and cancelling `Mob_dropAllDeathLoot` with `call.cancel = true` left a killed cow with no item, checked by the scenario). Fishing catches are `FishingHook_retrieve` (built; its result is the damage the rod takes (`FishingRodItem.cpp:38` passes it to `hurtAndBreak`: 0 nothing, 1 a catch, 2 in the ground, 3 a hooked item, 5 a hooked entity), and the catch is made inside it, so a mod cancels or replaces the whole retrieve; fired in a running game after `key MouseRight` cast a rod and reeled it in, once on the main thread and once on the server, because the client runs `retrieve` too, so a mod that changes the catch must check which side it is on). `getRandomItems` itself is not hooked (the generator has no list of item results); cancelling `LootTable_fill` is checked by the scenario (a chest opened with the hook cancelled held no item, one opened without held items; `data get block` could not show it, so the scenario reads the slots with `std.world.get_item_count`), and cancelling `FishingHook_retrieve` is checked by the scenario too: with the cancel on, a reel-in ran the hook and left the bobber out (`entity:get_mFishing()`, read after an `execute as @p`). The earlier runs that disagreed had the fishing-rod hint taking clicks, and the bobber falling far from a floating platform removes itself; scenarios now run with `--hints off`, and the check counts the retrieve so a fresh cast is not mistaken for a kept bobber |
| Game rule API | `GameRules` (`level:getGameRules()`, `getBoolean`, `getInt`) | Checked in a running game (`getBoolean(1)` and `getInt(15)` read the game's values); `GameRules::set` is an empty function in the game's own source, so a mod cannot change a rule through it, and a rule that is not in the game's list cannot be added |
| Interaction and entity events (`UseBlockCallback`, `AttackBlockCallback`, `ServerLivingEntityEvents`) | the `ServerPlayerGameMode_*` hooks (`startDestroyBlock`, `destroyBlock`, `useItem`, `useItemOn`) and `LivingEntity_hurt`, `LivingEntity_die` | Checked by the `mod-examples` scenario with `docs/examples/mods/events`: a block broken with the mouse (`destroyBlock`), planks placed on a block (`useItemOn`), and a cow hurt and killed (`hurt`, `die`) each moved its count |
| Message API (`ServerMessageEvents`) | `ServerGamePacketListenerImpl_handleChat` (and `_2`), hooked by `docs/examples/mods/hello` and counted by `events` | Checked by the `mod-examples` scenario: a chat line typed in the chat box moved the count |
| Entity AI and navigation (goals, `EntityNavigation`) | `std.entity.clear_goals` and goal hooks, `entity:setPathfindingMalus`, and the `PathFinder_findPath_2` to `_4` hooks with the level, mob and target | Checked by the `path-search` scenario with `docs/examples/mods/pathwatch`: searches seen with their mob, and answering them with no path kept the cows in place; a mod cannot build a path of its own nodes |
| Tags (`TagKey`, convention tags) | none | Not implemented: the game has no tag system, blocks and items are matched by class and id |
| Particles (`ParticleFactoryRegistry`) | `level:addParticle` spawns the game's particles, `ParticleEngine` methods and fields are exposed, `ParticleEngine_render` hooks drawing | Built, not checked in a running game; a mod cannot add a particle type, because `registerParticle` wants a `ParticleProvider` object and a mod cannot make a game class instance with its own behaviour |
| Dimensions | `registry.dimension` and the `Dimension_*` hooks | Checked by the `mod-examples` scenario: `/enter 5` run as the player moved them to `dimension_hollow` and `/enter 0` brought them back |
| Transfer API (inventories) | `ScriptedBlockEntity` and `Container` methods, `std.world.get_item` and `set_item` | Checked |
| Events a mod defines (`Event.create`) | `mod.listen` and `mod.fire` between mods | Checked (`ctest`) |
| Mod dependencies and versions | `depends=lib>=1.0` in `mod.txt`, a library is a mod | Checked (`ctest`) |
| Configuration | `std.config` | Checked (`ctest`) |
| Mod menu and metadata | `mod.txt`, the harness `mods` command | Checked by the `mod-examples` scenario (`mods` step: id, name, version and state of a loaded mod); the in-game mod menu itself has no scenario |
| Developer tooling (templates, hot reload, test server) | `tools/sdk/new_mod.py`, reload on change, `tools/harness` | Checked (`ctest`) |
| Sandboxing | not in Fabric either: a Lua mod has a memory and instruction budget, a WebAssembly mod too | Checked (`ctest`) |

## Fabric modules with no row above

These are the rest of the module list in Fabric API's repository (read from GitHub on 2026-11-10), with what stands in for each here. None of them is checked in a running game unless it says so.

| Fabric module | Here |
| --- | --- |
| API lookup, block API, block getter (`BlockApiLookup`, `FluidStorage`) | no lookup layer; a mod asks the world for a block entity (`level:getBlockEntity`) and calls its methods, and a chest answers `Container` calls (checked) |
| Item API (item settings, `ItemStack` events, tooltip callbacks) | `registry.item` and `item_of` for settings; stack methods are exposed; the tooltip text of an item is the game's own and has no callback |
| Model loading, renderer API, Indigo, fluid rendering | `registry.block_model` and `entity_model` take the game's own box-part format; there is no mesh API, and a fluid has no mod-side renderer (no liquid class is exposed in `api/lce.api` and `docs/modding-api.md` has no fluid renderer call) |
| Data generation, resource conditions, recipe API | recipes are `registry.recipe`; there is no data generator and no conditional resource |
| Loot API, tag API, convention tags | see the Loot API row; the game has no tags |
| Entity events, events interaction, message API, lifecycle | see the event rows above |
| Debug API, crash report info, client gametest, gametest | the debug protocol and `tools/harness` (`docs/harness.md`); `check mod-examples` is the nearest to a game test; a mod cannot add a line to the crash report (no line of `api/lce.api` touches `PcCrashReport`) |
| Serialization API, transitive access wideners | a mod reaches every public method and field of an exposed class, so there is nothing to widen; saved data is NBT through `std.data` |
| Particles, key mapping, sound API, screen API, menu API | see the rows above |

## Where it is narrower than Fabric

- Hooks reach what the build can weave, which is the game's own source (the matched decompilation), not code that is not in the tree.
- A mod command declares the kinds of its arguments in turn, and `std.command.tree` gives it branches (the first word picks a branch, the first-word choices come from the `a|b|c` arguments string); a branch declares its own kinds, and receives the rest of the line as text.
- Items are pictures in this game, so an item has no model of its own.
- The console menu has the controls its movie has; a mod needing others draws a `ModScreen`.
- The Wii U build has no mod host (`ModHost_wiiu.cpp` is inert); a mod runs on PC.
- Networking between two machines uses the custom payload packet on either protocol; the Reborn connection's hello carries the mod list, but channels are not yet negotiated in it.
