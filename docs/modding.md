# Mods

Mods add behaviour to a running game or server: chat commands, reactions to chat,
joins and leaves, and work every tick. The loader is `source/mod` (library
`lce_mod`, C++98, no game code; test `lce_mod_test`, run by `ctest`); the game
reaches it through `ModHost` (`source/game/core/ModHost.hpp`), which has an inert
Wii U backend and a PC backend (`platform/pc/PcModHost.cpp`). Game code calls
`ModHost` and never asks whether a mod is loaded.

## A mod

Start from a template: `python3 tools/sdk/new_mod.py mymod` makes `mods/mymod` with a hook, a command and a setting (`--kind wasm` makes a WebAssembly mod with its `lce.h` and a Makefile).
ctest makes the template and loads it, so it cannot drift from the API.

A mod is a folder, or a `.zip` archive, in the mods folder (`--mods-dir`, default `mods`
in the working folder, for the game and the dedicated server). It has `mod.txt` and an
entry file at its top level. Folders and archives are read through `archive::Archive`
(`source/archive`), the layer the shader packs use too:

```
id=hello
name=Hello
version=1.0
api=1
entry=main.lua
```

`id` is 3 to 32 characters of `a-z 0-9 _ -` and must be unique; `api` must be 1;
`depends=lib>=1.0,other` lists the mods this one needs, each with an optional version rule
(`>=`, `>`, `<=`, `<`, `=` or `^`, which keeps the major number); `memory=<MB>` and
`budget=<millions of instructions per call>` ask for more or less than the host's default,
within the host's maximum (below);
`entry` is a `.lua` or `.wasm` file in the folder. `docs/examples/mods/hello` is a
working Lua mod.

What `api=1` promises: every function and hook listed in `api/stable.txt` keeps its name, its
arguments and their types, its result, and its number for WebAssembly (`LCE_FN_*`, `LCE_HOOK_*` in
`tools/sdk/lce.h`), for as long as the API version is 1. The names follow the game's own methods
(`entity:getHandSlots` is `Entity::getHandSlots`), so a change to the game that would rename or remove
a promised one is caught: `hookgen.py check`, run by `ctest`, fails and names it, and the fix is to keep
the old one working or to start API version 2. What is new since the last `hookgen.py freeze` works
the same, but is promised only once frozen, which is done when a build is released. What a
function does is the game's behaviour, which is not part of the promise.

Mods load at start, in folder order. They reload while the game runs: every second
the loader checks the modification time and size of `mod.txt` and the entry file,
reloads a mod whose files changed, loads a new folder and unloads a removed one.
`harness.py mods reload` reloads all of them at once. A file a mod `require`s is
not watched; touch `mod.txt` or the entry file to reload.

A mod that fails to load shows in `harness.py mods` with its error and nothing
else changes. A mod whose callbacks fail 10 times in a row is switched off until it
is reloaded.

## Libraries, order and events between mods

A mod can depend on others. The loader starts them in dependency order, refuses a mod whose
dependency is missing or the wrong version (naming which), and reloads the dependents of a
mod that changes. A library is a mod with files and no behaviour: a mod that depends on
`lib` can `require("lib:util")` to load `util.lua` from `lib`'s folder, and only then; a
module that library loads in turn finds its own siblings.

Hooks run in priority order, lowest first, then in load order:
`mod.hook("entity_hurt", fn, {priority = -5})`. Mods can also talk to each other without
knowing each other: `mod.listen("shop:buy", fn)` and `mod.fire("shop:buy", player, 5)`; the
arguments are booleans, numbers, strings and handles, and a listener that returns `false`
cancels the event, which `fire` reports by returning `false`. Name an event `modid:name` by
convention. WebAssembly has the same through `lce_listen`, `lce_fire` and an exported
`lce_custom`, and `lce_hook_register(hook, priority)`.

## Files and the overlay

A mod never changes the game's folders. It has one folder it may write in, its data
folder: `modData/` inside a plain-folder mod, and `<name>_modData/` beside an archive
mod (`mods/foo.zip` writes in `mods/foo_modData/`). That folder is not mistaken for a mod.

The game folder `assets/` is overlaid by the data folder's `assets/`. When the game
opens a file for reading, or asks whether it exists, it first looks for the same path
under `<data folder>/assets/`. So `foo_modData/assets/Common/res/gui/logo.png` is used
instead of `assets/Common/res/gui/logo.png`. With several mods the one loaded last wins.
Removing the mod, or the file, brings the original back; the original was never touched.
Paths match exactly, with the case the game uses.

```lua
file.write("assets/Common/res/gui/logo.png", bytes)   -- true, or false if refused
file.read("notes.txt")                                  -- text, or nil
file.exists("notes.txt")
file.read_game("Common/res/gui/logo.png")               -- the original game file, read only
file.read_mod("textures/ruby.png")                      -- a file packaged in the mod itself, read only
```

`file.read`, `file.write` and `file.exists` act on the data folder only; `file.read_mod` (WebAssembly `mod_read`) reads from the mod's own folder or archive, which is where a mod ships its pictures and sounds. A name is refused
if it is empty, absolute, longer than 240 characters, or has `..`, `.`, a backslash, a
colon or an empty part. A file may be as large as `max-file-mb` (16). The WebAssembly
imports are `file_read`, `file_write`, `file_exists` and `game_read`. The overlay serves
files and existence checks, not folder listings, and a texture the game has already loaded
is replaced at the next start, not at once.

## The light engine

Light is the first engine behind an interface. `LightEngine` (`game/world/light`) is
what `Level::checkLight` asks; `VanillaLightEngine` is the game's own, moved there
unchanged, and `LightEngine::Create` picks it. When a mod listens to the `check_light`
provider hook, `ScriptedLightEngine` is used instead and the mod does the work: read
with `std.world.block_at(level, x, y, z)` and `level:getBrightness(layer, x, y, z)`, store with `std.world.set_light(level, layer, x, y, z, value)`. The layer is
the game's own number everywhere in the API: 15 for sky light and 0 for block light. In a test a light engine of a mod stored 9 on both
layers with `std.world.set_light` and read 9 back with `getBrightness`.
The engine is chosen when a world's levels are made, so the mod must be loaded then.

## Mob AI

The game's goal system (`Goal`, `GoalSelector`) is the interface. A mod adds a goal of its
own to a mob with `std.entity.add_goal(mob, name, priority, flags)` (or the same with `targets` true for the list
that picks whom the mob attacks) and removes it with `std.entity.remove_goal(mob, name)`. The goal
is a `ScriptedGoal` in the mob's own selector, so priorities, control flags (1 movement,
2 looking, 4 jumping) and interruption work as for the game's goals. Its behaviour is the
hooks `ScriptedGoal_canUse`, `_canContinueToUse`, `_start`, `_tick` and `_stop` (the goal's own methods, woven), each given the goal as `self`:
`self:get_mName()` is the name and `self:GetEntity()` the mob; set `result` and `cancel` in the first two to answer. `std.entity.move_to` and
`look_at` (in `lcestd`) act on the mob; its target is `entity:getTarget()` and `entity:setTarget(other)`, and the nearest player `level:getNearestPlayer_3(entity, distance, false, -1)`, the game's own methods. In a test, a pig
given a goal that called `std.entity.move_to` toward the nearest player walked from 8 blocks away to
within 1 block of the player.

To replace a mob's AI instead of adding to it, call `std.entity.clear_goals(mob)` from a `Level_addEntity` hook
(it empties both lists, then add what you want). The pathfinder's costs are per mob and per
kind of terrain: `entity:setPathfindingMalus(BlockPathTypes.get_WATER(), cost)` (negative is impassable) and
`entity:getPathfindingMalus(type)`; the terrains are the game's `BlockPathTypes` members. In a test a pig
spawned, lost its goals, got water cost -1, and ran only the goal the mod gave it. Each path search
is a hook as it begins, with the level, the mob and the target: `PathFinder_findPath_2` (a point),
`_3` (a block) and `_4` (an entity), each with `_after`; setting `call.result = nil` and cancelling answers
with no path. The `path-search` scenario checks it with `docs/examples/mods/pathwatch`: cows' searches
were seen with the cow and its target, and with every search answered with no path the cows stood
still once the paths they were already walking ended. Not done: building a `Path` from nodes of the
mod's own (no conversion makes one), a whole new movement control, and AI for mobs the mod defines.

## The network protocol

`PacketCodec` is the interface between the game's packets and the wire, and each `Connection` asks `CreateCodec()` for
its own when it is made, and deletes it. The game's own is `LegacyPacketCodec`, or the Reborn codec when the `protocols`
settings choose it (`reborn-protocol.md`); a packet mod's codec takes the place of both, so a server with a packet mod speaks
only that mod's frames. When a mod listens to the `packet_encode`
provider hook, `ScriptedPacketCodec` is used: it writes each packet in the game's format into memory, hands the
bytes to the mod (`packet_encode(server, loopback, id, data)`), and sends the mod's bytes with a length in front; on the
other end `packet_decode(server, loopback, data)` turns them back. A mod can compress, encrypt, rewrite or reframe
every packet, or change only some by id. In a test a mod xored every byte of every packet and the game
loaded a world, spawned the player and ran, over that wire. Both peers must run the same mod. Each frame starts with a four byte marker and a fingerprint of the id and version of every mod that listens to `packet_encode`
(`ModHost::HookOwners`, read when the connection is made), so a peer with no packet mod, or another one, is named in the log ("the peer's first bytes are not a packet mod frame" or
"the peer runs a different packet mod") and the connection is treated as broken, instead of its bytes being read as a length. The fingerprint covers the mod's id and version, not its
settings. A local game with the xor mod above ran 1664 frames with the marker and fingerprint and loaded a world; a dedicated server running the xor mod and a client without it (`LCEReborn --join`) were tried, and the server logged "the peer's first bytes are not a packet mod frame" when the client connected. Two clients with different packet mods, and a modded client joining an unmodded server, were not tried.

A mod has channels of its own: `std.net.broadcast(channel, bytes)` sends a custom payload packet to every player, `std.net.send_to_server(channel, bytes)` sends one
from a client, and the receiving mod hooks `ClientPacketListener_handleCustomPayload` or `ServerGamePacketListenerImpl_handleCustomPayload` and reads
`packet:getIdentifier()` and `packet:getData()` (binary safe); a hook has the listener as `self`, so it can answer with `self:send(packet)`. In a test a server mod
broadcast seven bytes, a client mod read them and answered, and the server mod read the answer (`docs/examples/mods/network`). The codec is chosen per connection in one respect: `loopback` is true for the host's connection to its own player, which is the same on both ends, so a mod can pass those bytes through and
change only a remote player's (a local game with the xor mod passing loopback bytes through ran 1818 frames, all loopback, and loaded a world; a remote connection was not tried, because it needs two game processes). One mod's
codec still applies to every remote connection; two different codecs at once are not supported. `bytes` is a binary-safe string type in the API.

## The chunk and save format

`ChunkStorage` is what a dimension asks to load and save a chunk. The game's own is `McRegionChunkStorage`, over the
region files in the save container. When a mod listens to the `chunk_store` provider hook,
`McRegionLevelStorage::createChunkStorage` makes a `ScriptedChunkStorage` instead: it serialises each chunk in the
game's own form into memory, gives the bytes to `chunk_store(world, dimension, x, z, data)`, and asks
`chunk_fetch(...)` for them back, so the mod keeps chunks wherever it likes (a file per chunk with `file.write`, a single
archive, another encoding, compressed, encrypted) and no region file is written. Because the bytes are in a defined
format, a mod that wants a different layout converts to and from it itself. In a test a mod kept every chunk in a file
of its own, the world was closed, and the same world loaded 761 chunks back through `chunk_fetch`.

Not done: the rest of the save (level data, players, the container itself) is still the game's; the container must stay
compatible with the other clients, so it is meant to be extended by a mod, not replaced. A mod that changes how a chunk
is laid out (not just where it goes) needs the chunk's own fields exposed, which the generated classes now allow for
`LevelChunk` once it is listed in `api/lce.api`.

## The standard library: the friendly layer, in Lua

The calls a mod finds easy to use (`block_at`, hiding the HUD, a flat-coloured quad) are not C++. `docs/examples/mods/lcestd` is a
library mod, `require("lcestd:std")`, that builds them in a few lines each from the generated calls (`std.world.block_at`,
`std.world.surface`, `std.ui.hide_hud`, `std.draw.flat`, `std.draw.quad`). A mod that depends on it hid the HUD and read the block and
the surface height in a test. This is where new convenience belongs: when something is wanted, it is written here against the game's
own methods, and nothing in the engine changes. The convenience calls that used to be C++ (`block_at`, `set_block`,
the state, data, redstone, light, goal and sound helpers) are here now. Nothing is written by hand for the API any more: every call is a method of
a game class or of a loader class, generated, and `docs/todo.md` ("Hand-declared functions audit") says how that was reached.

A mod instance does not pay for the whole API: a module's functions and a handle kind's methods are put in their Lua table the first time the mod looks one
up, and a derived kind answers its bases through a chain instead of a copy, so a mod that only hooks and logs starts small.

## Interception: any game function, nothing placed by hand

The rule for hooks is Fabric's: nobody goes into the game's code and adds one. A class is opted in with one line in `api/lce.api`,
`intercept Class from game/path/Class.cpp`, and the build weaves the interception into a copy of that source file (`tools/hooks/hookgen.py
weave`, run by CMake for each file in `tools/hooks/woven.txt`; the original is never edited and the copy lives in the build folder). For every
method the file defines, private and non-virtual ones included, a mod gets two hooks named for the symbol:

- `Class_method(self, args..., result)`, called as the function begins; set `result` and `cancel` to return that instead of running it, or `cancel` to skip it;
- `Class_method_after(self, args...)`, called as the function returns, whichever way it leaves.

and every data member of the class, private ones too, becomes `self:get_member()` and, for numbers, `self:set_member(value)`, reached by explicit
template instantiation (the standard way to name a private member) inside the woven copy. `intercept` is for the game's own objects; a
mod's own objects come from `override` (the `Mod<Class>` templates), and a class is in one or the other.

`docs/examples/mods/worldgen` changes the overworld with nothing but these symbols: `OverworldLevelSource_prepareHeights_after` puts a
stripe into every chunk the game shapes, and `OverworldLevelSource_postProcess_after` takes the generator's private `mLevel`
(`call.self:get_mLevel()`) and builds a gold tower in some chunks. The same is open in the nether and the end (`NetherLevelSource`,
`TheEndLevelSource` are intercepted), and in any class the next `intercept` line names.

A parameter the game leaves unnamed is named in the woven copy (`lceArg1`, ...), and a static method is hooked with no `self`. Skipped by the weave:
constructors and destructors, and parameters with no conversion yet (arrays other than bytes, and by-value objects), so the surface stage
`buildSurfaces`, which takes a biome array, is not hookable until those are.

## Worldgen: biomes, terrain, structures, points of interest

Numeric arrays cross the boundary as packed little-endian strings (`string.unpack("<i4", ...)`). `intercept BiomeInitLayer ... only getArea` therefore lets an `_after` hook rewrite the biome numbers the game's layers produce: set `call.result` to a packed string of the same length and the chunks adopt it. Create the biome first (`PlainsBiome.new(id, false, properties)`, free ids 48-127); `Biome.getId(level:getBiome(x, y, z))` reads it back.

None of it has an API of its own; it is the generated symbols. `docs/examples/mods/worldgen` makes a biome, "Mirror Plains", with
`BiomeProperties.new` and `PlainsBiome.new` (constructors are generated as `Class.new`; a game class nested in another is named
`Owner_Name` for the mod and the class itself is reached as `Name`), puts it into every chunk the game generates with
`OverworldLevelSource_lightChunk` and `chunk:setBiomes` (a byte array travels as a string), and the game ran and drew it. Terrain is changed by
intercepting the generator, `OverworldLevelSource_prepareHeights_after`, and structures and points of interest are placed after population with
`OverworldLevelSource_postProcess_after` and the `level` functions; `std.gen.structure` in `lcestd` places a template given as rows of text.
The same in the nether and end through `NetherLevelSource_*` and `TheEndLevelSource_*`, and in a mod's own dimension (its generator is the
overworld's unless `generate_terrain` replaces it).

Biome ids 48 to 127 are free; the game reads an id from 128 up as a variant of the biome 128 below it. Two game bugs came out of this and are fixed:
the biome statistics table was indexed past its end for any id it did not know, and `ServerPlayer::tick` awarded a statistic that did not exist.
Not done: telling the game's biome layers to choose a new biome by itself (a mod assigns biomes to chunks it is shown, which is what the example does),
and biome colours and decoration (the biome's own fields are reachable only if `Biome` is intercepted with an `intercept` line).

## Game classes as the API

`level:game_time()` and `entity:health()` were removed because the generated `level:getGameTime()` and `entity:getHealth()` do the same, and `entity:level()`, `x()`, `y()` and `z()` because (`get_mLevel()` for the level) the generated field accessors `entity:get_mX()`, `get_mY()` and `get_mZ()` (any data member of an intercepted class, `intercept Entity from ... only -` for the fields alone) do; the other hand-written calls stay until a generated one can replace each without changing what it does.

A mod does not get a hand-written function for each thing a game class can do. `api/lce.api` names a game
class with `override Block from <header> to <output> self block`, and `tools/hooks/hookgen.py` reads the
virtual methods of that class from its header and writes two things: one hook per method the mod can
answer (`Block_use`, `Block_getLightEmission`, ...) and a template, `ModBlock<Base>`, that derives from any
game class and asks the hook before each method. Adding a method to the game's class adds the hook the
next time the generator runs; nothing is written by hand. The same is done for `Item`, and for `Entity`, `LivingEntity` and `Mob` together (`Entity_tick`, `Mob_...`: some 270 hooks), where `self` is the entity.

The override layer uses the same type conversions as the generated calls (one table in `hookgen.py`), so a parameter that is a class pointer or reference arrives as a typed handle (`Entity_hurt(self, source, damage)` gets a `DamageSource` handle that the `DamageSource` methods accept) and an enum as its number. In a test `Entity_hurt` fired for a mod creeper with the damage source and the amount.

A hook gets the object as `self` (its registered name), the method's parameters by the names the header
gives them (a position becomes `x`, `y`, `z`; a block state becomes its data value), and for a method that
returns something, `result`. Set `result` and `cancel` to answer instead of the game; do neither and the
game's own method runs. A mod object is made from a game class with `registry.block_of(name, kind, ...)`,
kinds `door`, `trapdoor`, `button`, `pressure_plate`, `lever`, `ladder` and `fence`, and then is that
class: it opens, powers and connects the way the game's does, with its own name, number and save. One
line in `ModBlockKinds.cpp` makes another game class available as a kind.

`docs/examples/mods/kinds` makes all seven and answers `Block_getLightEmission` for one so it glows. In a test
the light at that plate was 15 and 13 two blocks away. The hooks are filtered by `self` in Lua because
one hook covers every object of the class; a library can wrap that as `on_block(name, method, fn)`.

`registry.entity(name, base, ...)` takes `pig`, `cow`, `chicken`, `sheep`, `zombie`, `skeleton`, `spider` or `creeper`; a mod mob is a `ModEntity` of that class, so every `Entity_*` hook reaches it. In a test the hook fired for a mod creeper and not for the game's own mobs.

`registry.item_of(name, kind, title, icon, a, b, c, d)` does the same for items: `food` (nutrition, saturation, meat), and `sword`, `pickaxe`, `axe`, `shovel`, `hoe` with a tier of the mod's own (mining level, uses, speed, damage bonus). The item is a `ModItem` of the game's class, so `Item_*` hooks change it. In a test the three were registered, named, given with `/give` and held in the inventory by their names; eating and swinging them is not tested.

A method that returns a pointer to a game class is answerable too: the hook's `result` is a handle the mod was given, and the game uses that
object (compiled and generated; a mod cannot yet make a new `SoundType` or `AABB` to return, only pass one it was handed).

Not done yet: methods whose parameters or result are not yet convertible (shapes, textures, render
and the item-stack methods; the generated headers list them), armour, bows, buckets and potions as item kinds, and block entities as kinds. A kind looks like the game's own block
until a mod draws it differently, because textures are the game's. Scripted blocks (`registry.block`) are
still written by hand in C++ and are the next thing to rebuild on top of this layer.

## The game's own methods, generated

`expose Class from header` in `api/lce.api` makes every public method of a game class callable from a
mod, under the game's own name: `level:getSeaLevel()`, `level:getBlockState(x, y, z)`,
`state:getBlock()`, `block:toString()`. The generator reads the header, skips what the game declares but never
defines (a call to it would not link, and it found about a dozen on `Level` alone), and writes the binding
(`ModExposed_generated.cpp`), the Lua and WebAssembly tables, the SDK header and the docs. A pointer to any
class in a signature becomes a typed handle of that class, so `AABB`, `Biome` or `Vec3` appear as
handles without being listed and a mod cannot hand a `Block` to something that wants a `Level`. Overloads
are named `name`, `name_2`, `name_3`; a position parameter is three numbers. `Level`, `Block`, `BlockState` and
`Item` are exposed (about 480 functions); to expose another class is one line.

Static methods are in a table named for the class (`Minecraft.GetInstance()`, `GlStateManager.enableBlend()`), an enum travels as its number, and
a handle of a derived class works wherever a base's is wanted and answers the base's methods: `Minecraft.GetInstance():getLevel(0)` is a
`MultiPlayerLevel` and takes every `level` method. About 1900 functions across 36 classes are generated, including the main `Minecraft`
object, the renderers, `Gui`, the tesselator and `GlStateManager`. A class hierarchy is read from the headers, so adding a class adds its upcasts.

Not covered: parameters that are not a scalar, a string, a position, an enum, an entity or a pointer or reference to a class
(arrays, vectors, callbacks, by-value objects), classes nested in another class, and returning a position or a stack.
The handles last for the call or hook that made them, as before. The calls that once
overlapped with this layer (`set_state`, `block_at`, ...) were removed or moved to `lcestd`.

## Replacing the rendering pipeline

The renderer's classes are in the generated call layer, so a mod can draw a frame the way the game does. Cancel the renderers' own methods that draw the world (`LevelRenderer_renderSky`, `_render`,
`_renderEntities`, `_renderDestroyAnimation`, `ParticleEngine_render`, `_renderLit`, `GameRenderer_renderSnowAndRain`, `_prepareAndRenderClouds`) and draw in `world_render` with `GlStateManager.*`, `Tesselator.getInstance():getBuilder()`
and the `BufferBuilder` calls (`begin_2(0)` starts triangles; a method named like a Lua keyword gets an underscore, so `tess:end_()`).
Coordinates are relative to the camera. `docs/examples/mods/pipeline` skips the whole world and draws two quads of its own: a
screenshot showed sky, the hand and the HUD, no terrain, and the red and green quads in front of the player. `LevelRenderer`,
`GameRenderer`, `ParticleEngine`, `EntityRenderDispatcher` and `Gui` are exposed too, so a mod can also run their own
methods (draw entities, particles, the HUD) in an order and a way of its own.

`GameRenderer_render` is the whole frame: cancel it and the game draws nothing, and the mod composes a frame from the generated calls (`renderer:renderLevel`, `Gui`, `Tesselator`, `GlStateManager`). `Gui_render` is the native display: cancel it to draw none of the game's own. The old `render_stage`, `frame_render` and `hud_draw` hooks are these methods now; the stage numbers became method names. A drawing context travels as a parameter of the game's own type `ModHandles::GuiInfo` (or `WorldInfo`) reaches a hook as a `gui` (or `canvas`) handle, so the old `hud_render` is `Gui_renderOverlay(self, hud, playerIndex)`, a method the game now has for drawing its overlay; cancel it to replace the overlay, or draw in it before the overlay is drawn.

The console personality's scenes are reachable too: `ConsoleUIController.instance()` (a `singleton` line in `api/lce.api` makes a global object available) gives `GetHUD(player)`, a `UIScene_HUD`, and `UIScene`, `UILayer`, `UIGroup` and `UIControl` are exposed (about 230 functions). `UIScene::setVisible` had been a flag that nothing read; the scene now honours it when it draws. A mod that calls `ui:GetHUD(player):setVisible(false)` every tick (a `MinecraftServer_tick` hook, since no handle is needed) hid the hotbar and crosshair in a screenshot; with `ui:SetEnableTooltips(player, false)` and `ui:ShowTooltip(player, i, false)` as well the button hints went too (`docs/examples/mods/hidehud`: a screenshot showed the world with no hotbar, crosshair or hints). Replacing the console UI is therefore a matter of hiding scenes and drawing others (`Gui_renderOverlay`, `canvas`, mod screens), and the open work is a mod building a console scene of its own. `GetHUD` was declared in the header and never defined, which the generator caught by skipping it; it is defined now.

Not done: a mod cannot yet build a console scene of its own, and the chunk meshes the game builds
are not reachable (a mod that wants terrain draws from `Level` and `BlockState` calls each frame, which is slow).

## Dimensions

`DimensionType` is a registry now: the game's three come first (index 0 overworld, 1 nether, 2 end) and a mod adds its own
with `registry.dimension(name, id)` from `Item_staticCtor_after` (id 2 to 100, at most 8 in all; `DimensionType::Register`). The server makes one
level for each type (`MinecraftServer::mLevels` is sized by the registry, `getLevel` and `setLevel` go through the type's index, the
block-update slots of `ServerLevel` are per index), saves a mod's chunks under `DIM<id>`, and builds its `Dimension` as a
`ScriptedDimension`: the overworld's behaviour over a generated `ModDimension<NormalDimension>`, so every `Dimension_*` hook can
change sky, light, fog, spawn and the rest, and `generate_terrain` (given the level, so it can tell dimensions apart) makes the land.
A player enters one with the generated `entity:changeDimension(id)`; a mod dimension keeps the coordinates, has no portal, and puts
the player on the surface at that column.

The client keeps one level for all mod dimensions, in the overworld's slot, because a player is in one dimension at a time; the packets that
name a dimension carry that slot (`DimensionType::getClientSlot`) and the respawn packet makes a fresh level when the slot holds another
dimension's. In a test a mod dimension was entered from the overworld: the client reported the dimension's name and drew the terrain. Seven trips there and back ran without a crash, and a world that had been in the mod dimension reloaded. Two clients joined one dedicated server (each with its own `--identity-dir` and `--username`) and one was moved to the mod dimension while the other stayed in the overworld: nobody dropped and nothing crashed in 70 seconds. Finding the mod dimension's chunks again after a reload was not checked.

Leaving a mod dimension crashed about one run in six: `PlayerList::removePlayerFromReceiving` used the dimension's number as the index of `mReceivingPlayers`, which holds the game's three, and a mod dimension's number (2 to 100) indexed past it. It now puts any other dimension in the overworld's group, as the other functions of `PlayerList` do (`// Parity Not Checked`, until it is compared with the executable). The `mod-examples` scenario enters and leaves the mod dimension, which is how it was found.

Known problems: entering the nether with `changeDimension` crashes, with or without this change (it crashed on the build before it): first in
`PortalForcer::findPortal`, which assumed the entity came through a portal (guarded now), then in `PortalForcer::createPortal` with a corrupt
object, which I did not run down; a client that lacks a dimension its server has is refused: a client with no mods joined a dedicated server that registered one and logged "The server has mod content this game does not have: dims_mirror" before leaving (the client is started directly, not through the harness, which makes a world of its own: `xvfb-run build/pc-debug/LCEReborn --join HOST:PORT --mods-dir EMPTY`); and the client does not free the overworld level it replaces.

## Handles in a command, and the game's own containers

A handle is valid for one call into the game, and a call that runs a hook inside it (a command that reads a chest, whose loot table fills through `LootTable_fill`) used to lose the
handles it had made: the host held no handle scope around a command, a timer, a load or a reload, so the scope of the inner hook cleared the command's own. `PcModHost` holds one
around each now. The same object may also be asked for as another class (`shared_cast`): a game chest is a `Container` through `LockableContainer`, and a call on its `BlockEntity`
handle reaches the `Container` methods, as a mod block's does; the `BlockEntity` copies of those methods that only answered for a mod block are gone, since the cast answers for both
(checked: a mod read the 27 slots of a chest, and `mod-examples` reads one with `std.world.get_item_count`).

## Block behaviour: the game class's own methods

A mod block reports what happens to it through the hooks of the game class, with no declaring: `Block_neighborChanged`, `Block_onPlace`,
`Block_onRemove`, `Block_stepOn`, `Block_entityInside`, `Block_tick` and the rest of `Block`'s virtual methods, each given the block's name as
`self` (`ScriptedBlock` is a generated `ModBlock<Block>`, so it needs no code of its own for them). They fire on the client too, so a mod
that wants the server only checks the level's `mIsClientSide`. `registry.block_random_ticks(name)` gives a block the game's random ticks;
`std.world.schedule_tick(level, x, y, z, delay)` asks for a `Block_tick` after a delay. This replaced `registry.block_behaviour`, `block_event` and
`block_touch`, which listed what a block wanted reported and called a hand-placed hook for it. A block entity's per-tick hook is `ScriptedBlockEntity_tick`, with the place read from its fields (`self:get_mPos_x()`, `get_mPos_y()`, `get_mPos_z()`,
`get_mLevel()`; a position field or result is three functions). `registry.block_entity(name, slots)` lost its `ticks` argument, since the hook
needs no declaring.

## The redstone engine

Redstone is the second engine behind an interface. `RedstoneEngine` (`game/world/redstone`) is what
`Level::updateWire` asks when a wire is placed, removed or has a neighbour change; `VanillaRedstoneEngine`
is the game's own wire logic, unchanged. When a mod listens to the `wire_update` provider hook,
`ScriptedRedstoneEngine` is used instead and the mod decides the power of the network. The mod gets
`std.redstone.wire_power`, `std.redstone.set_wire_power` (no neighbour update), `std.redstone.signal_at` (what the sources
around a position give, wires not counted), `std.redstone.conducts` and `std.redstone.update_wire_neighbors` (written in `lcestd` over `Blocks.get_redstone_wire()` and the level's own methods).
Updates the mod causes while it runs wait until it returns, so it never re-enters itself, and the
game stops after 4096 nested rounds with a warning instead of hanging. `docs/examples/mods/redstone`
solves the whole connected network once per change. In a test a line of twelve wires read 15 down to 4,
a gap in it left 0 past the gap, and removing the source cleared the line. As with the light engine
the engine is chosen when a world's levels are made, so the mod must be loaded then. Repeaters,
comparators, torches and pistons still use the game's logic; their classes are reachable through `intercept` and
the `Block_*` hooks, not yet through an engine of their own.

## Sound: a mod can be the whole engine

`Minecraft::mSoundEngine` is a `ConsoleSoundEngine`, the interface the game talks to, and
`SoundEngineProvider::Create` picks the implementation when the game starts. The game's own is
`SoundEngine` over the platform mixer. When a mod listens to the `sound_start` provider hook,
`ScriptedSoundEngine` is used instead and nothing is played unless the mod plays it. The mod gets:

- `sound_start(sound, x, y, z, volume, pitch, kind)` for every sound the game asks for
  (kind 0 in the world, 1 on the interface, 2 music or a record), and the
  generated `ScriptedSoundEngine_tick` every frame (the listener is where `canvas:camera_x()`
  and its neighbours in `GameRenderer_renderWeather` say).
- `sound.decode(name)`, the recording behind a sound name as the bytes of a 16-bit WAV
  file. A name with several recordings gives one at random each time.
- `audio.open(sample_rate, channels)`, a channel to the speakers (at most 8 a mod), with
  `stream:queue(pcm)` taking 16-bit little-endian samples, `stream:queued()` and `stream:close()`.

An immersive-audio mod mixes in its own code: it keeps voices, applies distance, direction,
reverb or occlusion to the decoded samples, and queues the result. The channels close when
the mod is unloaded. Not done: the event's own volume and pitch ranges are not applied by
`sound.decode`, a few interface sound names have no recording behind them (`minecraft:press`,
`minecraft:focus`), stopping a looping sound has no call yet, and the engine is chosen at
start, so a mod loaded later does not take over until the next start. A decorating mod that
only changes or silences sounds still uses `sound_play` and `std.world.play_sound`.

`std.audio.play(name, {loop=true, volume=0.5, pan=-1})` plays any sound by name on a channel of its own and returns a handle whose `stop()` ends it: the sound is decoded (`sound.decode`) and fed to an audio stream, so a mod can loop, stop, scale and place a sound without a new engine call (`docs/examples/mods/audio`).

## Surfaces: pictures a mod draws anywhere

One primitive covers images, video and anything a mod renders itself: a `surface`, a
grid of RGBA pixels the mod owns. `surface.create(w, h)` makes one (or `surface.decode(bytes)`
reads a PNG, JPEG, BMP or GIF), `surface:write(x, y, w, h, rgba)` replaces pixels, and
`gui:image(surface, x, y, w, h, u, v, u2, v2)` draws all or part of it on any display
the game hands the mod a `gui` for (today the heads-up display through `Gui_renderOverlay`; screens
and chat get the same `gui`). A video player writes each decoded frame with `write`; a
browser renders a page into a surface and sends the result the same way; nothing in the
game knows which. The game turns the pixels into a texture on its draw thread when the
surface is drawn, and frees it when the mod destroys the surface or is unloaded. A surface
is the one kind of handle that outlives the call that made it, and only the mod that made it
can use it. Limits: 4096 pixels a side, 64 surfaces a mod. `surface:read(x, y, width, height)` reads pixels back as `surface:write` takes them.

In the world, `GameRenderer_renderWeather(self, world, level, partialTick)` is the game's rain and snow stage; its hook runs after the terrain and the clouds are drawn, and `world` is the `canvas` for that view (a parameter of the game's type `ModHandles::WorldInfo` reaches a hook as a `canvas`). `canvas:image(surface, x, y, z, across_x, across_y, across_z,
down_x, down_y, down_z, light)` draws a surface as a flat picture whose top left corner is at
the world position, whose top edge runs by the across vector and whose left edge runs by the
down vector (blocks), seen from both sides, with light 0 to 15 for how bright it looks. That is
the one primitive: a picture on a block face, an item frame or a map is the call with the
numbers for that face, and one over a mob is the call with the mob's position. The picture
hides behind terrain like anything else. It does not take the world's light at that spot (the
mod picks `light`, for instance from `level:getBrightness(layer, x, y, z)`), and it is not part of the shader pack
passes. Not done: letting a mod draw on a block's own texture so it shows through the
chunk mesh, or in an item frame's own model.

Strings that cross to WebAssembly now carry a 32-bit length, so a frame is not limited to
64 KB.

## The world renderer: stages a mod can skip

The game draws the world in stages, each a method of a renderer (sky, clouds, opaque terrain, entities, particles, translucent terrain, block breaking, rain and snow), and a
hook on that method, cancelled, skips that stage; with `GameRenderer_renderWeather` and
`canvas:image` the mod draws its own in its place. In a test, cancelling stages 1, 2 and 5
left only the sky and stars. A mod can now draw geometry of its own. `canvas:triangles(surface, vertices)` draws triangles with a
surface as their picture and `canvas:colored_triangles(vertices)` with colours only; `vertices` is nine
little-endian 32-bit floats per corner (x, y, z, u, v, red, green, blue, alpha, built with
`string.pack("<fffffffff", ...)`), three corners a triangle, in world coordinates, up to 30000 corners a
call; `canvas:camera_x()`, `camera_y()` and `camera_z()` say where the camera is. With those hooks that
is a world renderer of the mod's own: cancel a stage and draw what replaces it. In a test a mod cancelled the
opaque terrain, translucent terrain and clouds stages and drew every exposed top and north face within
7 blocks as flat-coloured quads built from `std.world.block_at`; the game's own sky, entities, items and
hand still drew on top. Costs and limits: it is a plain draw call each time it is made (no caching or
instancing), so a mod that rebuilds a mesh every frame in Lua pays for it in Lua (the test did about 1700
`block_at` calls a frame); there is no way to keep a mesh on the GPU between frames, no lighting (colour only,
the mod works out its own), no custom shaders (shader packs do that) and no access to the game's chunk
meshes. The stages are still the game's, and the shader pack's finer stage list (sun, moon, block entities,
outline) is not exposed.

## Chat

The chat window draws each line through `Gui_renderChatLine(self, display, text, left, y, width, height, message, first, indent, colour)`;
the hook runs for every line of chat just before its text is drawn, with the line's position and size in interface pixels,
the number of the message (0 is the newest), and whether this is the first line of a message.
The mod draws with the same `gui` as everywhere (`gui:image` with a `surface` is how a Heads
in Chat mod shows a face) and changes `indent` to the room it wants left of the text; cancel the hook to draw the line itself. A message's
text and who sent it come from `chat_message`; keep what you need by message number.
A mod gets the pointer over the chat from `ChatScreen_Pointer(pad, message, kind, x, y, button)`. `message` is the number of
the line under the pointer, the same one `Gui_renderChatLine` gets (so a mod that drew something on a line knows what was clicked), or -1 when
it is over none. `kind` is `move`, `press`, `release` or `scroll`; x and y are in interface pixels from the line's top left; `move` with message -1
means the pointer left the lines. The chat's own answer to a wheel turn is to scroll, so cancelling the hook takes the event.

The mouse (or a finger) works in every chat mode. With the controller, chat shows a cursor
in the full history view (press the right stick): the Look stick moves it and A presses. The
on-screen keyboard view uses the stick and A for the keyboard, so it has no cursor.

## Screens

A mod can own the whole screen. `ModScreen.Open(player, name)` opens one for a local player (0 to 3) and `ModScreen.Close(player)` closes it;
`ModScreen.Name(player)` says which is open. While it is open the world takes none of that player's input, the mod gets `ModScreen_Draw(pad, gui,
width, height)` every frame (the pointer is drawn after the hook, over what it drew) and draws everything with the game's painter (`gui:painter()`: fills, text, panels, controls)
and `gui:image` with a surface, and gets `ModScreen_Input(pad, kind, x, y, button, text)` for each event: the game's own answer is to close on
Escape, so cancelling the hook takes the event. Pointer events carry x and y in the interface pixels of `ModScreen_Draw`. A keyboard also sends
`key` events (the key's name in `text`, `Shift+` in front when held) and `text` events (what was typed, so an input field or a browser gets real
text). A controller sends `Up`, `Down`, `Left`, `Right`, `Escape` (B) and `Enter` (A, while the Look stick cursor is not showing) as keys, moves a
cursor with the Look stick and presses with A. A screen closes by itself when the player dies or no mod listens to `ModScreen_Draw` any more.

Two kinds of screen exist, and a mod picks by what it needs. The game's console menu scene (`UIScene_CustomMenu`) has controls, descriptions,
tooltips, controller and mouse navigation and the pause menu stack. A menu is a page of rows: `UIMenuPage.new(name)`, filled with `page:AddButton`,
`AddCheckbox`, `AddSlider`, `AddValue` and `AddLabel`, and handed to the scene with `ConsoleUIController.instance():NavigateToMenuPage(player, page)`,
which the scene owns from then on. The mod listens to the scene's own handlers (`UIScene_CustomMenu_handlePress`, `handleCheckboxElementToggled`,
`handleSliderElementMove`), and tells its menu from another by `self:GetPage():GetName()`. The game's own pages are pages too, so a mod can add a
row to one with the hook on its `Build` (`UIMoreOptionsPage_Build_after(self, menu)` and `menu:AddButton(...)`). `docs/examples/mods/menu` is one.
`ModScreen.Open` is the other: a screen the mod draws entirely itself. It draws with the game's own menu kit: `gui:painter()` returns the `MenuPainter` the shader and debug menus use (`Panel`, `Button`, `Box`,
`Slider`, `SwitchSegment`, `Scrollbar`, `Chip`, `Text`, `Trim` ...; the list is generated from `UIMenuPainter.hpp`, in `docs/modding-api.md`), so a mod screen
looks like the game's and a change to the kit reaches every screen. `docs/examples/mods/widgets` is a widget set built on it.

Chat and mod screens share two pieces of the game: `HudOverlay` (the layers drawn over the
HUD, one scale and one pointer space for all of them) and `UiPointer` (mouse, finger and
Look stick turned into move, press, release and scroll). Anything else that wants to draw
over the HUD and take a pointer is a `HudOverlayLayer`.

Widgets are not part of the game: a screen is one rectangle, and the toolkit on top of it is
shared Lua code, `docs/examples/mods/widgets` (a library mod; copy it and `docs/examples/mods/uidemo`
into the mods folder). A mod that lists `depends=widgets` writes

```lua
local ui = require("widgets:widgets")
local menu = ui.menu("my_screen", {title = "Settings"})
menu:label("Pick")
menu:toggle("Sound", true, function(on) end)
menu:slider("Volume", 0, 10, 5, function(value) end)
menu:input("Name", "Steve", function(text) end, 16)
menu:button("Close", function(self) self:close() end)
menu:open(0)
```

and gets a centred panel with labels, buttons, toggles, sliders and a text field, drawn with the
`gui` calls, with focus kept for the keyboard and the controller (Up and Down move it, Left and Right
change a slider or toggle, Enter activates), hover and click for the mouse and a finger, and typing
into the focused field. This is the console interface's own design for menus (a focused row, a
d-pad), built from the primitives, so a mod can restyle it or write another toolkit; the game has no
widget set of its own to replace. In a test, keys moved focus, a slider went from 5 to 8, typing and
Backspace edited a field, Enter toggled a switch and a click on Close closed the screen. Not done: a
mod screen is not one of the console scenes (`UIScene`, the SWF movies), so it does not take part in
scene navigation, tooltips or the pause menu stack; making one would be a game change, not a mod one.
It is not tied to the Bedrock toolkit. A mod's screens are not closed when only that mod unloads while another
still listens to `ModScreen_Draw`.

## Limits

Nothing is fixed in the code. The host's defaults, and the most a mod may ask for, are in
`mods/limits.txt` (`key=value`): `memory-mb` (16) and `max-memory-mb` (256) for the memory
of one mod, `budget-millions` (5) and `max-budget-millions` (500) for the instructions one
call may run, `failure-limit` (10) failed calls in a row before a mod is switched off,
`max-mods` (256), `max-file-mb` (16), `wasm-stack-kb` (128) and `poll-ms` (1000) between checks for changed
files. Command limits (`execute-forks` 4096, `compare-blocks` 32768, `command-chain` 65536)
are set with `--command-limits name=number,...`.

## Lua API

Scripts are Lua 5.4. The sandbox has `base` (without `dofile`, `loadfile`, `load`
and `collectgarbage`), `table`, `string`, `math`, `utf8` and `coroutine`; of `os`
only `os.time` and `os.clock`; there is no `io`, `debug` or `package`. `print`
writes to the log. `require("name")` loads `name.lua` from the mod's own folder.

```lua
mod.id, mod.version
mod.hook("MinecraftServer_tick", function(call) end)      -- any symbol of an intercepted class, and the hooks listed in modding-api.md
mod.command("name", function(sender, args) return "text" end, "<usage>", 2, "targets number") -- the reply goes to the caller; usage, permission (0 anyone, 2 operators, 4 console) and the kinds of its arguments, in the words the game's own commands use (targets, coord, number, item, mode, text), are optional
std.command.tree("shop", { buy = { run = function(sender, rest) return "bought " .. rest end }, list = { run = function() return "nothing" end } }) -- lcestd: the first word picks a branch, and the box offers the branch names, then the kinds a branch declares with `arguments` (the command itself is declared as one signature per branch, split by `;`)

game.log(text)
game.say(text)                 -- to every player
game.run(line)                 -- run a command as the server: ok, reply
game.players()                 -- a table of names
game.time_ms()
```

A command name is 1 to 32 characters of `a-z 0-9 _`. A mod command is a command of the game's own (`ModCommand`): `/help` lists it with its usage, the command box
completes it, and only a sender of its permission level runs it. A name a built-in command has is not taken. It receives the caller's name (`Server` for the
console) and the rest of the line, and what it returns is sent back to the caller. It goes when its mod is unloaded.

Limits per mod: 16 MB of memory (an allocation past it fails), and about 5 million
instructions for one callback (past that the call stops with an error). Handlers run
on the server thread, in the order the mods loaded.

## The generated API: hooks and handles

`api/lce.api` is the one description of what a mod can call and hook. Edit it, then run
`python tools/hooks/hookgen.py generate` (it writes the bindings, the game's dispatch, the
WebAssembly SDK header `tools/sdk/lce.h` and `docs/modding-api.md`, which lists every
call and hook) and write the host function. Each hook becomes a typed function in
`ModEvents` (`game/core/ModEvents_generated.hpp`); the game calls it in one line at the
`target` named in the description (`if (!ModEvents::EntityHurt(shared_from_this(), damage, source)) return false;`).
A member declared under `#if LCE_DEBUG_BUILD` or `#if LCE_PLATFORM_PC` is not part of the API, so the API is the same in every build and on every platform.
`hookgen.py check`, run by `ctest`, fails when a generated file is stale or a target
function does not call its hook.

In Lua: `mod.hook("entity_hurt", function(call) ... end)`. `call` has the hook's
arguments by name; `call.cancel = true` (or returning `false`) stops what the game was
about to do, and an argument marked `mut` can be assigned (`call.amount = 0.5`). A
handle (an entity, a level) is a table whose functions are methods:
`call.entity:getName()`, `call.level:getBlockState(x, y, z)`. A handle is valid only while the
hook or callback that gave it to you runs; a stale one makes the call fail with an error. A call
that returns several objects the game owns, such as `entity:getHandSlots()` (written `list:ItemInstance`
in `modding-api.md`), returns a list: a Lua table of handles in order, nil for an empty slot. A list of
entities still comes as their ids, one per line, as it did before lists existed.

In WebAssembly: `lce_hook_register(hook)` from `lce_init`, then the game calls your
`lce_hook(hook, ptr, length) -> i32` with the arguments packed as values (the codec in
`source/mod/ModValue.cpp`); return 1 to cancel, and `lce_hook_set(index, value, length)`
to change a `mut` argument. Any API function is one import, `lce.call(function, in,
in_length, out, out_capacity)`, with `LCE_FN_*` numbers and the pack helpers in `lce.h`.

Hooks are generated from the game's own symbols (`Class_method`, `Class_method_after`, see "Interception" above), and there is no
separate event mechanism. A hook the game used to carry by hand is now a woven one: `block_break`, `block_use`, `item_use`, `server_tick`,
`player_join`, `player_leave`, `sky_color`, `light_override`, `entity_spawn`, `sound_play`, `chat_message`, `screen_open`, `chunk_generated`,
`entity_hurt`, `entity_death`, `audio_tick`, `hud_render`, `chat_line` and `world_render` no longer exist (see `docs/todo.md` for their
replacements). Registering content is a hook on the game's own item registration, `Item_staticCtor_after`, during which the registries are open; the screens a
mod owns are `ModScreen_draw` and `ModScreen_input` (see Screens).

Engine hooks (`docs/modding-api.md`): `Gui_renderOverlay` (draw on the display with the `gui`
functions), `light_override` (the light level the game reads), `sky_color`, and
`chunk_generated` (change the terrain of a new chunk with the `chunk` functions). A hook
that returns a value (`light_override`, `sky_color`) is asked first and the game keeps its
own value when no mod sets one.

Audio, rendering and the interface: a mod runs inside the client on PC, so a hook on the
client side is possible and `screen_open` and `sound_play` are the first two. What there is
not yet is anything a mod can draw or play itself: no 2D drawing for a HUD, no render
layer or entity renderer, no way to build a screen, no way to play a sound by name for one
player, and no mod resources (models, textures, sounds). Those are the client API in
`modding.md`, Direction, item 7; each is a set of calls and hooks in `lce.api`, plus the
resource loading behind them.

## WebAssembly API

The entry file is a WebAssembly module, run by wasm3 (an interpreter, so one build
runs everywhere). There is no WASI and no file or network access: the module can call
only what the game imports into it, module name `lce`, strings as pointer and length
in the module's memory (UTF-8):

| Import | Signature | Does |
| --- | --- | --- |
| `log` | `(i32, i32)` | write a line to the log |
| `say` | `(i32, i32)` | say a line to every player |
| `run` | `(i32, i32) -> i32` | run a command as the server; 1 if it did not fail |
| `command` | `(i32, i32)` | register a command name (call it from `lce_init`) |
| `reply` | `(i32, i32)` | add text to the reply of the command being run |

and the game calls what the module exports: `lce_alloc(size) -> ptr` (room for the
game to write a string into), `lce_init()` (once, after load) and
`lce_command(ptr, length) -> i32`. The text at `ptr` is the command name, the sender
and the arguments joined by newlines. Everything else a module hears arrives through
`lce_hook` and `lce_custom`.

Limits: 16 MB of memory, a 128 KB stack, and an instruction budget: the vendored wasm3 counts every pass of every loop in a call (`m3_fuel`), and a call that spends the
mod's budget (the same `budget=` setting as a Lua mod's, millions of passes) traps instead of holding the thread. A mod that loops for ever fails like a Lua one.

## Direction: the Fabric class of mod

The API above is the first step, not the goal. The goal is mods like Fabric's: a mod
can change almost any behaviour, add content, depend on other mods, and a library is
just a mod others build on. What that takes, as one design:

1. Mods are packages with a graph. `mod.txt` grows `depends` (with version ranges),
   `breaks`, `provides`, `entrypoints` (`init`, `server`, `client`, `registry`,
   `preload`) and `exports`. A library is a mod with exports and no behaviour. The
   loader resolves the graph, orders the load, reports a missing or conflicting
   dependency before anything runs, and gives each mod only the modules of mods it
   declared.
2. One description of the engine API, in an interface description file, is the single
   source: a generator produces the C++ host bindings, the Lua bindings and the WebAssembly
   import headers (C, and a template for Rust and Zig), and the API docs. A new call is added once.
3. Hooks that reach arbitrary game code, the counterpart of Fabric's mixins. C++ cannot
   patch bytecode, so the build generates the patch points: every method of the game
   classes marked hookable (blocks, items, entities, levels, players, the network and
   command layers) gets a generated entry that consults a hook table, with one pointer
   test when nobody hooked it. A hook can run at the head or tail, wrap the call,
   replace it, change an argument or the result, or cancel. Several mods hook one method
   in a declared order. This is wider than Fabric's event API and narrower than a
   free-form patch, on purpose.
4. Events that mods define. A mod creates an event type with its own phases and
   ordering (`before`/`after` another mod's id), fires it, and other mods listen, so
   libraries expose their own extension points the way Fabric API does.
5. Open registries. Blocks, items, entities, block entities, effects, enchantments,
   recipes, biomes, features and commands register under `modid:name`. Mods load
   before registration. A world saves the name to id table and remaps on load, with a
   placeholder for content whose mod is gone; players agree on the table at connect.
   Content is data plus behaviour: a table of properties and a handler object.
6. Mod state: per-world and per-entity data attachments saved with the world, mod
   configuration with a schema, and mod-defined network channels (which need the Reborn
   protocol, `reborn-protocol.md`).
7. Client mods: resources (models, textures, sounds, language), a rendering API (render
   layers, entity and block entity renderers), screens on the Bedrock interface
   toolkit, HUD elements, key bindings and tooltips.
8. Safety and speed. Each mod declares the capabilities it needs (file access is
   never one); limits per mod for memory and time, with fuel metering for WebAssembly
   (a runtime that can be interrupted); trust levels if native mods ever come, behind
   the same interface.
9. Developer tools: an SDK with templates, hot reload, a test harness that runs a mod
   headless against a world, and a stable, versioned API with a deprecation period.

Order: the hook generator and the interface description first (they decide everything
else's shape), then events and the world API on top of them, the dependency graph and
exports, open registries with the save remap, then the Reborn protocol and client
content. Each step is useful alone: after the first two a mod can already change most
server behaviour.

## Replacing a subsystem, and content with its own interface

Hooks patch a point. Two things a mod must also be able to do are not patches: take over a
whole subsystem (render the world its own way, generate terrain its own way, light the
world its own way), and add content that brings its own interface (a block that opens its
own screen). Doing them as piles of hooks would leave the game a tangle, so the design
is this, in the order it has to be built:

1. Subsystems become services. Each engine subsystem (world renderer, light engine,
   terrain generator, audio, screens and menus, input) is one interface with the game's
   implementation behind it, found through one registry, and game code asks the registry
   rather than naming a class. This is also the cleanup the project owes itself: today the
   renderer draws immediate mode from many places, lighting and generation are reached
   directly, and the UI builds scenes in a factory. A subsystem that is not behind an
   interface cannot be replaced, so moving each behind one is the first piece of work and it
   removes debt whether or not a mod ever uses it.
2. A mod can decorate a service (run before or after it, which is what a hook is, now defined
   once at the interface) or replace it (register a provider that outranks the default; two
   replacements of one service are a load error unless one names the other as its base).
   The interface is declared in `api/lce.api` as a `service` and the generator writes the C++
   adapter that forwards each call to the mod, so a Lua or WebAssembly mod can be the world
   generator or the light engine.
3. Rendering is split into passes (sky, terrain, entities, particles, weather, HUD) that a
   mod can add to, reorder or replace, each given a draw API (meshes, materials, shaders,
   2D drawing). A mod that renders on its own replaces the world pass and draws with that API.
   This needs the renderer reorganised into passes first.
4. A block with its own interface is three registered things: the block (properties and a
   behaviour object for use, neighbour change, tick, break), a screen (a provider with init,
   draw, input and close, built on the interface toolkit, drawn on the Bedrock widgets) and,
   when it holds items, a menu (the server side container logic with slot synchronisation).
   `use` on the block asks for the screen by name. The menu and the screen travel over a mod
   channel, which is a Reborn protocol feature, so such content works for Reborn players;
   a Legacy peer, which cannot name it, never sees it.
5. Open registries come before 4, so the block and the screen have names and numbers every
   player agrees on.

What exists now is the hook layer, the first engine hooks above and the first replaceable
subsystem: the terrain generator. `generate_terrain` is a provider hook (`provider` in
`api/lce.api`): when a mod listens, the dimension builds a `ScriptedChunkGenerator`
(`game/world/chunk`) instead of the game's own, hands the mod an empty chunk as a `primer`
and fills it from what the mod sets with `std.gen.primer_set(primer, x, y, z, name)`. The game still lights the
chunk, sets its biomes from the biome source and draws mobs from the biomes. A mod made hills
of grass, dirt and stone this way. What the mod does not get is the game's caves, structures
and decoration (it makes its own), `chunk_populated` (population runs on worker threads) and
a way to ask the game's generator for a base to build on. The registry, the renderer passes
and screens (steps 1, 3, 4 and 5) are the design and are not built.

## What a mod can and cannot do today

It can: hook any method of the game classes that `api/lce.api` intercepts (head and tail, change an argument or the result, cancel), call any method of the classes it
exposes, register items, blocks (from the game's own classes too), mobs, recipes, sounds, dimensions and creative entries, derive new content from content already
registered, give blocks a store and an inventory, replace the world's terrain, biomes, lighting, redstone, sound, rendering stages and chunk storage, own a screen or a
console menu, draw in the interface and the world, define channels of its own between server and client, keep configuration in its data folder, depend on and
call other mods, and be reloaded while the game runs.

Status of the design in "Direction" above: 1 (packages and the dependency graph, with version ranges), 2 (one description of the API, generated), 3 (hooks into any
game method), 4 (events between mods, `mod.listen` and `mod.fire`), 5 (open registries, saved by name) and the network channels and configuration of 6 are done; per-entity
and per-world data (`std.data`: a value on an entity is saved with it as an entity tag, and settings per world live in the mod's data folder under the world's
name) and the block store of 6 are done; key bindings of 7 (`std.input.bind("G", fn)`; a binding is not listed in the controls screen), language files of 7 (`std.lang.load`; a title is a key, translated for the player's language), and mob and block models of 7 (`registry.entity_model` and `registry.block_model`: boxes in text, drawn with the game's own model parts), and an SDK with templates in 9 (`tools/sdk/new_mod.py`) are done. A capability list per mod (8) is not planned: Fabric mods are trusted code too, so what limits a mod is its memory and instruction budget.

## Not done

By decision, not planned: a mod key binding as a row of the controls screen (a mod keeps its keys in its own configuration, `std.input.bind(config.key, fn)`); light and shader passes for world pictures (a shader pack already acts on the whole frame); a native code path (`dlopen`). WebAssembly already runs compiled mods on every platform from one build, and a native backend would be one per operating system and unsandboxed; if it is ever added it goes behind the same `Instance` interface. A ninth creative tab: the creative scene's movie has eight, so `registry.creative` takes the tab 0 to 7.

- A model of an item other than its picture (items are pictures in this game). A block has its shapes (`registry.block_shape`, a picture for each side) and, with a store, a model of boxes drawn on it (`registry.block_model`).
- The Wii U build is inert and was not compiled.

## Adding a mob base

A mod mob is made from one of the game's own mob classes (`registry.entity(name, base, health, speed)`). The bases are the `mobbase name TYPE Class from header` lines of `api/lce.api`; `hookgen.py generate` writes them into `ModEntityBases_generated.hpp`, so a new base is one line and no C++ table is edited.
