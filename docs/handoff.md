# Handoff

Written on 2026-11-10 for the agent that takes over from Claude Sonnet 5.5. The ordered list of
planned work is `todo.md` and stays the only one; `plan.md` is the approved execution order over it and over what was discussed (start there for what to do next); this file says where the work stands, what to
start with, how each large piece should be approached, and what to watch for. Read `AGENTS.md`
first (the method, the evidence rules, the source rules), then this file, then the `todo.md`
section of the piece you take. A claim in this file is as old as its date: check it against the
source before relying on it.

The last code commit of this stretch is `26dc73f4` on `harness-and-nbt-filters`. `ctest` passed 8 of 8
in `build/pc-debug` there. The Release build (`build/pc`) was last built and tested at `39d4c39d`,
before the server access work, so build it first. The working tree was clean at `26dc73f4`; this
file was added after it.

## What the project owner asked for, in their words

- "The modding API still has to be finished."
- "The server binary still has to be worked on, to be a competent server, with safety feats."
- The Reborn protocol and the save format are the next large pieces.
- The interface must stay faithful to the original Legacy Edition. New features and buttons go
  in the proper place in it, with good taste. Rethink how to integrate Identities, a Friends
  system, chat and voice chat.
- Take from LegacyEX what is worth taking. Its code and its quality are not checked, and the
  owner thinks it is far inferior to this codebase.

## Start here

1. Build and test both configurations. A header, `api/lce.api` or generator change is also built
   in Release, because debug-only code sits under `#if LCE_DEBUG_BUILD` and a generated call to it
   once broke the Release build unseen (fixed in `39d4c39d`, rule in `AGENTS.md`).

   ```sh
   cmake --build build/pc-debug -j 3 && ctest --test-dir build/pc-debug
   cmake --build build/pc -j 3 && ctest --test-dir build/pc          # Release, about 25 minutes cold
   ```

2. Learn the harness (`harness.md`). Its commands are what every claim of "checked in a running
   game" in the docs rests on: `up --headless --seed N`, `cmd`, `state`, `screen`, `shot`, `key`,
   `type`, `check <scenario>`, `down`. Things learned the hard way:
   - A key press is `key NAME down` then `key NAME up` at once. Held for 0.3 s it repeats and
     moves two menu rows. `key MouseLeft|MouseRight|MouseMiddle` clicks at the pointer's place.
   - The pause menu is not ready right after the world loads. Wait, then read `screen`.
   - A client only suggests operator commands when it is an operator. In a new world, start it with
     `--launch-arg=--cheats --launch-arg=on`; an existing world keeps its setting.
   - A build is not finished when the command returns a tail line. Read the build log, then run the
     game. Two checks of mine ran against a binary that was still linking.
   - Scenarios (`tools/harness/scenarios/*.json`) reuse their world between runs. A step that fails
     when the block is already there makes the scenario pass once and fail after. Make every
     scenario re-runnable, and run it twice before trusting it.
   - `harness.py up --size WxH` draws smaller (a debug option, `--render-size`); 640x360 took a
     headless client from about 3.3 cores to 2.0.
   - Never commit before reading the check's output. I committed after a printed "0 of 1 passed"
     twice.
3. Two-process testing works and is worth using: `LCERebornServer --save-dir D --log-dir D --mods-dir M`
   in the background, then `harness.py up --headless --launch-arg=--join --launch-arg=127.0.0.1:25570`.
   The server listens on port 25570; a stray game process holding it makes the server fail with
   "Port 25570 is already in use", so check `ss -ltnp | grep 25570` first.
4. Test worlds `harness-12345-creative` and `harness-777-creative` were renamed `*.stale-pre-palette`
   in `saves/games`: they held mod block ids from before chunks had a mod palette and crashed on load.
   Since 2026-11-12 they load, with those blocks as air (`saves.md`); they are kept as the test case.

## Since then (2026-11-11 and 2026-11-12, Claude Opus 5.5)

Both builds, `ctest` (11 tests), `hookgen.py check`, the 14 scenarios, `hostile.py` and
`protocols.py` pass at the last commit. In plan order, with the doc that holds each:

- Server: world backups and log pruning (A1), `ban-ip` of an online player (A3), 50 clean sanitizer
  runs with the defects they showed fixed (A5, `harness.md` has the sanitizer build), and a hostile
  packet check (`tools/harness/hostile.py`, A2) that found a cut-off peer keeping its link; the
  server now closes it and logs the real address and reason (`pc-platform.md`).
- Reborn protocol steps 1 and 2 (B1, B2): codecs owned per connection; preamble, hello, refusal
  and framed packets behind `protocols` and `--protocol reborn`, checked with Reborn and Legacy
  clients on one server (`reborn-protocol.md`, "What is built").
- `interface-design.md`: the identities, friends, chat and voice proposal, waiting on the owner (H).
- Saves: unknown block numbers load as air (C6), mod block palettes recorded as a capability and
  refusals that name what is missing (C1), chunk memory and save cost measured with no layout
  change needed (C4) (`saves.md`, `compatibility.md`).
- Mod API: list results (D2, first conversion), path searches with level, mob and target (D3,
  `pathwatch`, `path-search`), and API version 1 as a checked promise with stable WebAssembly
  numbers (`api/stable.txt`, D9). PC-only and debug-only members are no longer exposed.
- Commands (E1): `/save-all`, `/save-off`, `/save-on`, `/stop` (through `ServerControl`, both
  servers), `/particle`, `/clone`, `/playsound`, `/locate`, `/spreadplayers`, `/bossbar` (saved with
  the world), `/data ... storage`; `/title`, `/worldborder`, `/scoreboard`, `/function`,
  `/trigger`, `/teammsg`, `/schedule` and `/datapack` recorded with what each waits on
  (`commands.md`).
- Menus (G1 to G3, `todo.md`): switches and value rows drawn once in `MenuPainter`; panels after a
  clipped list no longer lose their background (a stale scissor); a touch tap clicks in the shader
  and debug overlay menus; the harness can move the mouse (`pointer`).
- Reborn batching and zstd (part of B3, `reborn-protocol.md`), with `tools/harness/protocols.py`
  running a Reborn and a Legacy client on one server.
- A chat line over 123 characters dropped every remote PC client, because the client refused a
  string longer than its field; only the server refuses now (`pc-platform.md`).
- Scenarios: the world is saved when a scenario's client exits, so the summoning scenarios place
  the player and clear the area themselves, and `server-access` and `capabilities` have worlds of
  their own (`harness.md`).

Next in plan order: the rest of B3 (varint fields, per-packet string limits and long chat, which
needs the connection's agreement in the chat box and both chat packets), B4 to B6, then D4 to D8.
E2 and E3 wait on systems recorded in `commands.md`; F and H wait on the owner's answers to
`interface-design.md`.

## Where things stand

Built and checked in a running game this stretch (details in the commit messages and the docs):

- Mod API: command branches with typed arguments (`std.command.tree`); mod commands run as the
  entity under `execute as`; hooks for loot fills, mob drops, fishing, the pathfinder and move
  control, player and level saves, per-connection packet flags (`loopback`); game rules readable;
  scene replacement through `NavigateToScene` (`docs/examples/mods/pausemenu`); entity
  enumeration (`std.world.entities`); a handle scope around every mod entry, so a hook firing inside
  a command no longer drops the command's handles; a game chest answers `Container` calls.
- Commands: `/replaceitem`, `/loot` (give, insert, spawn, replace, `kill <target>`, `fish` as a
  table), chat-box completion of loot tables and slots, red highlighting of an invalid word.
- Crashes found by testing and fixed: leaving a mod dimension (index past `mReceivingPlayers`),
  `/clear` with armour worn, `LootTableContext::Builder` leaving fields unset.
- Packet mods: each frame carries a marker and a fingerprint of the mods that listen to
  `packet_encode`; a client without the mod is named in the server log (checked with a dedicated
  server and a client).
- Harness: mouse buttons, a `key` step, a `mods` step, `--size`, menu driving by keys (path in
  `harness.md`).

Open, and not in this list's reach alone: the `// Parity Not Checked` fixes need the original
`Minecraft.Client.rpx`, which is not on this machine. The `LootingEnchantFunction::apply`
crash (seen twice from `/loot give @p kill`) was not reproduced in 50 runs of `loot-command` under
ASan and UBSan on 2026-11-11 (`harness.md` has the sanitizer build): no memory error and no undefined
behaviour in the loot code. I cannot say the cause is gone, only that nothing points at it now.

The sanitizer runs found, and I fixed (PC-guarded, `// Parity Not Checked` in decompiled code):
memory freed the wrong way (`fuiFile` freed `malloc` memory with `delete`, the PC renderer's PNG
reader now allocates with `malloc` like every other texture decoder; `delete` instead of `delete[]` on
14 `arrayWithLength` buffers and in `EntityTickCache`), and members read before anything set them
(boss bar darkening, music and fog; `CombatTracker`; `Entity` teleport and world border flags;
`Rabbit`; `Textures`; `Tutorial::splitScreen`; the swinging arm; the PvP byte every health packet
sent; one `ResourceLocation` constructor). `FJ_Label` reads alignments 3 and 4 from movie data, which
its enum now names. Left as found: `Tutorial::splitScreen` is never assigned anywhere, so a split
screen change may not reach the tutorial popup (likely lost in decompilation); the 261 FUI factory
functions are called through a pointer of another type (harmless with single inheritance);
`TitleScreen::init` calls the stub `Language` through a null pointer; hash and random arithmetic
overflow signed integers as Java intends.

## Workstream 1: the dedicated server as a competent, safe server

Why first: it is partly done, it is bounded, and the owner asked for it by name.

State of `LCERebornServer` (`source/platform/pc/PcServer.cpp`, `ServerMain.cpp`), measured
2026-11-10 in Release: 57 MB with symbols, 103 MB resident idle with a new world, about a quarter
of a core idle. It already has: `server.properties` (name, motd, port, world, seed, gamemode,
difficulty, world size, cheats, pvp, name tags, view and simulation distance, autosave, LAN
visibility, texture pack), operators in `ops.json` by UUID (with "pending by name" until the player
joins), a console (`stop`, `save-all`, `help`, then game commands), SIGINT and SIGTERM stop
(`PcStopSignals.cpp`), a 30 second socket timeout, chat and drop spam limits
(`ServerGamePacketListenerImpl`), and an in-memory ban of an Xbox UID.

Done since this file was first written (see `pc-platform.md`, "Dedicated server"): the operators,
the whitelist and the bans share `AccessList`; `PcServerAccess` loads and saves `ops.json`,
`whitelist.json` and `banned-players.json` and the `white-list` and `max-players` settings for both
the dedicated server and a game hosted from the client; `whitelist`, `ban`, `pardon` and `banlist`
exist; a refusal is logged and sent as the game's own message. The owner's rule, now in
`AGENTS.md` ("Servers"): the standalone and the built-in server build from the same source and
have the same features.

What is left of this piece, in this order, committing at each step with a scenario or a two-process
check:

1. `max-players` above 8 needs the session's own limit raised (`GameSessionData::MAX_SESSION_PLAYERS`
   and what is sized by it); today the setting only lowers it. Reserve a slot for operators if the
   owner wants it.
2. Done: address bans (`banned-ips.json`, `ban-ip`, `pardon-ip`, `banlist ips`), a limit of connections per
   address and of connections that have not joined (`pc-platform.md`); the login timeout and the unjoined
   timeout already existed. `ban-ip` also takes an online player's name (the address comes from the PC network
   manager, `GetPlayerAddress`, so the decompiled `INetworkPlayer` is unchanged).
3. Done for the readers a client can reach: of the 57 packet types a server accepts, the 19 that read a
   string, loop or allocate were read one by one; the unbounded ones are refused (`PacketRead`), and a
   packet rate limit cuts a flood (`pc-platform.md`). NBT inside a packet is read under a 2 MB allocation budget
   (`NbtBudget`). Not done: no hostile packet was sent over a real connection. A test
   client that speaks the link protocol and sends malformed packets is the missing tool.
4. Done: the watchdog (`ServerWatchdog`, `max-tick-seconds`), for both programs. `stop` and the signals
   already saved and waited for the save. World backups on load and log pruning are done too (`pc-platform.md`).
5. The binary: it carries the renderer, UI and SDL code (`todo.md`, "Standalone server": the object
   library links every game object and `-rdynamic` makes every symbol a root). Leaving the
   client-only sources out of the server target means the shared game code must stop reaching them;
   `nm -C build/pc/LCERebornServer` shows how far that is. This is real refactoring of what the
   server includes, not a flag. Do it after the safety items, and keep Legacy behaviour byte for
   byte (`ctest` `legacy_stream`).

Write the settings and the behaviour into `docs/pc-platform.md` (find the dedicated server text there, or add a section) and
add a two-process check for each refusal (banned, not whitelisted, full) to `harness.md`.

## Workstream 2: the Reborn protocol

Read `reborn-protocol.md` and `compatibility.md` ("On the wire"). Steps 1 and 2 are built
(2026-11-11): each `Connection` owns its codec, and `RebornCodec.cpp` adds the preamble, the hello,
the refusal and the framed packets, opted into by `protocols` (both servers) and `--protocol reborn`
(a client). What was checked with two processes, and what is left of step 2 (a game-side round trip
of every packet, retrying as Legacy, the registry hash), is in `reborn-protocol.md`, "What is built".
A packet mod still takes the place of either protocol; two codecs at once is D8.

The remaining steps (3 to 7) are in `reborn-protocol.md` and `todo.md`. The number map of mod
content moves into the Reborn hello when it exists (`todo.md`, "Mod interfaces still to do").

## Workstream 3: the save format

Read `saves.md`. The native container (appended journal, zstd chunks) is built and PC writes it;
export converts back (`LCERebornSaveTool to-legacy`). Open: the capability recorded in `level.dat`
and the refusal by an older build, "Export for Wii U" in the save's options, the Wii U byte order
and LZX, the export running inside the game.

A trained zstd dictionary was measured (`saves.md`, 2026-11-10): 7 to 11 percent of chunk bytes on
one world, with the caveats written there. It is a format capability (every reader needs the same
dictionary), so it waits for a measurement over several worlds and seeds. Do not build it on the
strength of that number.

Rules that bite here: data that needs a capability is never written where a reader has not
declared it, and a world that uses none is saved exactly as the original saves it. Check actual
bytes, not behaviour, and keep `lce_savecontainer_test` round-tripping a real world.

## Workstream 4: finish the modding API

"Finished" needs a definition, because the API is generated and grows with the game. Proposal for
the owner to confirm: every row of `fabric-comparison.md` is either checked in a running game or
carries a reason it cannot be, every documented limit in "Where it is narrower than Fabric" is
either closed or a recorded decision, and a mod author can build, run and publish a mod from
`docs/modding.md` and `tools/sdk` without reading the game source.

Gaps I know of, each with what it needs:

- Rows built but not seen firing: the interaction and entity events, the message API, particles
  (spawning only), `ModScreen` input, key bindings in a scenario, the fishing cancel
  (`FishingHook_retrieve` is called from more places than the rod; read `Player::mFishing`, now
  exposed as `entity:get_mFishing()`, with a state test that does not use the mouse).
- Generator conversions still missing (`todo.md`, "Mod API: where it stands", item 4): arrays and
  vectors of anything but entities, by-value objects, callbacks, nested classes, positions as
  results, item stack types. `getEntitiesOfClass` needs a `type_info` a mod cannot pass; replacing a
  path needs a `Path` built from nodes.
- No tag system (`Tags`), no way to add a particle type, no mod renderer for a fluid, no line in the
  crash report, a mod cannot change a game rule (`GameRules::set` is empty in the game).
- The Wii U build has no mod host (`ModHost_wiiu.cpp` is inert). Only start this with a Wii U
  toolchain and hardware.
- Per-connection codec: one mod's codec applies to every remote connection; two codecs at once are
  not supported.
- Hot reload, dependency ranges and sandbox limits are built; a mod author documentation pass and
  a stability promise for the generated names (a rename of a game method renames its hook) are not.
  Decide what the API version covers and write it down.

Keep the rules: no C++ written only for the API to call; reshape the system instead. No hand
declared function in `api/lce.api`. The `ctest` `mod_api` test checks the generated files are
current and loads every example mod; run `python3 tools/hooks/hookgen.py generate` after any header
change and commit the generated files.

## Workstream 5: the interface, faithful to the Legacy Edition

This is the piece that needs taste and the project owner. Stop and design with them before writing
a screen, as `AGENTS.md` already says for the command block screen. What exists, so the design
starts from it and not from a blank page:

- The Legacy interface is the default and the reference. The game's screens are the console scenes
  (`UIScene_*`, movies for the retail layouts, `UIMenuPage` and `ModScreen` for pages and free
  drawing). New work in Legacy mode reuses a scene's layout, its tooltip bar (button glyphs at the
  bottom), its focus and navigation, controller first and then mouse and keyboard, in single and
  split screen. Strings go through `IDS_LCE_*` in `ExtraStrings.hpp` and `L10N.cpp`.
- Bedrock mode (`bedrock-ui.md`) is a second personality, being built a slice at a time; Java mode
  is reserved. Do not mix a Bedrock layout into a Legacy screen.
- Built and not reviewed by a person (`todo.md`): the Identities screen (Help and Options, under
  Change Skin; `identity.md`) and the Friends screen with the Friends row in Join Game and the
  Invite flow (`p2p.md`). Neither has been seen on a controller or in split screen.
- Chat: the retail console chat is minimal; this project has a Java-style chat box with
  suggestions and Tab completion (`ChatScreen.cpp`, command completion and red highlighting).
  Decide where that stops being faithful and say so in a disclosed divergence.
- Voice chat: nothing exists in this tree beyond the mixer. The console games had it per player
  with a speaker icon over the player in the list; the PC has no retail reference for it.

Questions to put to the owner before design, with a recommendation for each:

1. Where do Identities live? Today: Help and Options > Identities. Recommendation: keep it there
   (it is account-level, like Change Skin and Controls), and show the active identity's name and
   skin on the main menu where retail shows the signed-in gamertag.
2. Friends: today a row in Join Game, and the Friends screen from there or from Players/Invite.
   Recommendation: the same two doors only, no new top-level menu; a Friends entry in the pause
   menu's Players screen for inviting; presence shown as the retail "joinable" marker in the lobby
   list, not as a new panel.
3. Chat: keep the console's chat opening and tooltips, add the Java-style box as the PC keyboard
   behaviour, and put history, mute and block (per identity) in the Players screen.
4. Voice chat: a push-to-talk key by default (and a controller button), a speaker icon in the
   Players screen and over the player's name in the HUD list, input and output device choice and
   volume in the Audio settings, proximity as a host option in Host Options beside the existing
   ones, mute and block in the Players screen. Never send a peer's address (`p2p.md` threat model);
   voice rides the same connection as the game, so a Reborn feature when Reborn exists (a Legacy
   connection cannot carry it without a new custom payload, and Legacy bytes are frozen).
5. Moderation to ship with voice and friends, not after: block lists by identity, a report
   path is out of scope, host kick and mute, no voice from an unauthenticated peer.

Deliver the design as a short document with mock layouts drawn from the existing scenes, reviewed
by the owner, then build one slice at a time with the harness `screen` and `shot` checks, as the
Bedrock slices were.

## Workstream 6: what to take from LegacyEX

The directory is `../LegacyEX-PC` (47 MB, 3829 `.cpp` files, three commits). It is a TU74 PC port
made from the same decompilation this project started from; its README says "the edited version is
also known as LCEReborn". It is GPL-3, as this project is, so a licence is not the obstacle. The
obstacles are these: its quality is not checked; its README says it was made with AI and was
"touched by Generative AI by default"; and this project's rule is that behaviour claims need
evidence, so a LegacyEX change is a lead, not a source. Read it to find what is worth doing, then
do it here, in this project's structure, with a test.

Candidates from its feature list, with my view (the owner should rank them):

- Worth investigating, because they are bug reports: "patched a retail duplication bug", "patched
  a retail world crashing bug", Battle minigame chests filled with only apples, minigame crashes
  (a guest leaving crashes the host; maps loading too long; Tumble function names), spawn egg
  crash, Glide ghost starting ten seconds ahead, friends spawning under the floor in a Battle map.
  For each: reproduce in this tree first; if it reproduces, fix it with `// Parity Not Checked`
  and a note; if it does not, drop it.
- Worth taking as ideas: 64-bit and larger world sizes (a "Massive" 10240 world; this project has
  classic to large already), more than 8 players (they say 24; compare with `max-players` here),
  in-game key binding, voice chat with proximity and host force or disable (opus), persistent
  split-screen player identities (this project's identities already cover it), a soft anticheat
  (compare with Workstream 1 item 5), faster saving (this project's native container already
  measured 32 ms for 515 chunks).
- Worth a look, and cosmetic: the "Golden Age" options (old textures, models, water, lava, sounds,
  title logos, historical panoramas and tutorial worlds). They need assets that are not in either
  repository, so they are a settings design problem first and an asset problem second.
- Not worth taking: its central online service (an `mclegacy.space` server with Discord code
  authentication), its mod support (this project's is far larger), "Minecraft Store replaced by
  Online Multiplayer", and its in-game keyboard. This project's peer-to-peer design (`p2p.md`) is
  authenticated key-based and does not depend on a central service or Discord.

Method for each candidate: write what it claims, find the code in `LegacyEX-PC` that does it,
decide whether this project already has it, reproduce the bug or measure the claim, and only then
implement it here. Record the result (taken, not taken, why) in `todo.md`, so it is not
re-investigated. Other clones sit beside it (`neoLegacy` for the network protocol and formats,
`LCE-UI`, `nbcraft`); `AGENTS.md` says which is an authority for what.

## Suggested order and sizes

1. Workstream 1 items 2 to 4 (connection limits, packet limits, run safety): medium.
2. Workstream 1 item 1 (more than 8 players), if the owner wants it: it reaches the session structures.
3. The Reborn step 2 design agreed, then built with the two-process check: large.
4. The interface design document with the owner (Workstream 5): no code, but the longest wait; ask
   early.
5. The modding API gaps that are checks (cheap) before the ones that are generator work.
6. Save capability and "Export for Wii U".
7. Workstream 1 item 5 (the server binary without the renderer): large refactor; last.
8. LegacyEX bug leads, interleaved wherever a reproduction is cheap.

Do not start: the Wii U mod host, a dictionary for chunks, a central online service, a rewrite of
the matched decompiled classes toward data-oriented storage (`AGENTS.md` says why), or the Java
interface mode.

## Traps

- The matched decompilation is the definition of correct for game code; a fix to it that is not
  checked against the original keeps the fix and carries `// Parity Not Checked` above it. There are
  five today (`grep -rn "Parity Not Checked" source`).
- Comments in game code are forbidden except that marker. The game and shared code is C++03; only
  `source/platform/pc/` is C++17.
- Generated files are checked in. After a header or `api/lce.api` change run
  `python3 tools/hooks/hookgen.py generate`, build both configurations, and commit the result.
- Do not report a result that was not seen. Where a check was not possible, the docs say so in the
  same sentence; keep doing that.
