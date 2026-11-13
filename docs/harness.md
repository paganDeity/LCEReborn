# The agent harness

The harness answers questions about a running game instead of a rebuilt one.
An agent starts one debug client, asks as many things as it needs, and stops
it. Prefixes, menu clicks and world loads stop being part of every question.

The game side is a small debug protocol in the PC backend; the tool side is
`tools/harness/`.

## Building the debug client

The protocol and the harness flags exist only in a build that defines
`LCE_DEBUG_BUILD`, which CMake sets for `Debug` and `RelWithDebInfo`. A
release build does not contain them.

```sh
cmake -S . -B build/pc-debug -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build/pc-debug --target LCEReborn -j 3
```

The harness looks for `build/pc-debug/LCEReborn`, or the binary named by `LCE_HARNESS_BINARY`.

A sanitizer build runs the same scenarios under AddressSanitizer and UndefinedBehaviorSanitizer:

```sh
cmake -S . -B build/pc-asan -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DLCE_BUILD_LAUNCHER=OFF \
      -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
      -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=bounds" \
      -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build/pc-asan --target LCEReborn -j 3
LCE_HARNESS_BINARY=$PWD/build/pc-asan/LCEReborn \
ASAN_OPTIONS=detect_leaks=0:detect_stack_use_after_return=0:log_path=/tmp/asan/report \
UBSAN_OPTIONS=print_stacktrace=1:log_path=/tmp/asan/ubsan \
python3 tools/harness/harness.py --port 25580 check loot-command
```

Undefined behaviour is reported and the game goes on, except an out-of-bounds index; a memory
error stops the game. `detect_stack_use_after_return` is off because libpng's `setjmp` in
`png_safe_execute` trips it with nothing wrong in the game's code. The game reads a null `Language`
in `TitleScreen::init` (a stub that never touches it), so the null check must stay recoverable.

## Using it

```sh
python tools/harness/harness.py up --seed 12345 --mode creative
python tools/harness/harness.py state
python tools/harness/harness.py cmd "time set day" "give @p stone 64"
python tools/harness/harness.py key w down
python tools/harness/harness.py shot /tmp/view.png
python tools/harness/harness.py key w up
python tools/harness/harness.py down
```

`up` returns as soon as the client answers; the client keeps running for the
next command. Without `--world` it stays in the menus, which is what a menu
question wants.

`--world FOLDER` loads that save, or makes a new world there if it has none.
`--seed` and `--mode` without `--world` name the world after themselves,
`harness-<seed>-<mode>`, so the same arguments give the same world: the
first run generates it, `down` saves it, and later runs load it. Either one
alone defaults the other to seed 12345 or creative. `up` reuses a client
that is already running, and refuses, without stopping it, when that client
was started for a different world. A world that fails to start is reported
with the game's own error as soon as the game exits. `up` returns once the
world has loaded and no menu is flagged as displayed, because the game
refuses to open a screen over one for a few seconds after a load.

The harness is headless by default. A headless client runs on a private Xvfb display
at 1280x720, uses Mesa's software renderer, draws 10 frames a second and is silent: it
selects SDL's dummy audio driver, so nothing is heard. `--sound` keeps the real audio
driver on a headless client. `--windowed` runs on the current display instead, plays sound
and draws at the game's 60. `--frame-rate` overrides either; it passes the debug option of
the same name to the game. `--headless` is accepted and changes nothing.

The harness starts the client with `--skip-intro on` (a debug-build option), so the splash
screen and the save-icon message are not waited for: the splash ends at its first tick and
the message confirms itself, which leaves the first-run notice on a new profile and the main
menu otherwise.

A profile that has never run before shows the first-run notice before the
main menu. `state` reports it as `eula`, and `up --world` presses Return to
accept it, so a fresh profile can boot unattended.

| Command | Does |
| --- | --- |
| `up` | start a client, or reuse the one already running |
| `down` | stop it and the X server it started |
| `pause` | stop the client with SIGSTOP; it uses no CPU and keeps its world in memory |
| `resume` | continue a paused client; `up` and `down` resume it too |
| `state` | world, tick count, frame rate, player position and rotation, level time |
| `cmd` | run console commands and print the replies |
| `console` | print captured console output from a sequence number |
| `shot` | write the current frame as a PNG |
| `key` | press or release a key by name, or a mouse button (`MouseLeft`, `MouseMiddle`, `MouseRight`) at the pointer's place; Ctrl, Shift and Alt held this way apply to later keys |
| `pointer` | move the mouse pointer to `X Y`, each from 0 to 1 across the window; a later mouse button clicks there |

Two `key` commands (down, then up) are two processes, so the button stays down for about a quarter
second, which is long enough for the game's hold-to-repeat (5 ticks) to use the item again. A
scenario `key` step, or both requests on one connection, makes a quick click.
| `type` | send text as typed input, the way an input method delivers it |
| `screen` | with no name, list the top scene of each UI layer; with a name, open it: `pause`, `help`, `controls`, `settings`, `skins`, `main-menu`, `shaders`, `debug-overlay`, `friends`, `identities` |
| `touch` | send a finger down, move, up or cancel at a screen position |
| `p2p` | drive peer-to-peer play: `status`, `add-friend CODE`, `friends`, `lobbies`, `open`, `invite KEY`, `join KEY` |
| `identity` | list and change identities: `list`, `select FILE`, `create NAME`, `rename FILE NAME`, `remove FILE`, `skin FILE PNG` (bundles a skin) |
| `save` | save the hosted world now, as the pause menu's save does (debug builds) |
| `rawpacket` | sends `hex` (a packet number, then its fields) to the server on the client's own connection, `count` times (1 to 100000); for testing what a server does with a packet a hostile client would send. The bytes go through the client's codec like any packet, so on a Legacy connection they reach the server as written |
| `chunkstats` | `memory`: the chunks the hosted world keeps loaded, with the bytes of their block elements, light and fixed parts; and the chunk path's timers (debug builds): chunks serialised, compressed into the region, read, decompressed and deserialised, and the container's encode and flush, each with count, total and slowest milliseconds and bytes in and out; `--reset` zeroes them after reading |
| `mods` | list the loaded mods (folder, id, name, version, state, error, commands); `mods reload` unloads and loads them all again |
| `ask` | send the user a request in the game chat, with optional answers to choose from |
| `answers` | show requests and their answers; `--wait` waits for them |
| `profile` | sample frame rate, section times, server tick time and memory, then print average, min and max |
| `check` | run named scenarios and report each step as `ok` or `FAIL`; exits 1 when any fails |
| `wait` | return once this many server ticks (or, with `--frames`, drawn frames) have passed |
| `hold` / `release` | stop server ticks, or let them run again; the client keeps drawing |
| `step` | run this many server ticks while they are held, and return when they have run |

The session records its pid, display, log and port in
`$TMPDIR/lcereborn-harness/<port>.json`. A second command reads that file,
so nothing has to stay in the foreground between commands.

## Hostile packets

`python3 tools/harness/hostile.py` starts a dedicated server in a temporary folder (game port 25590, `max-packets-per-second=500`), joins it with a debug client (harness port
25581), and for each case sends packet bytes on the client's own connection with `rawpacket`: a crafting packet that says it carries 2^31 items, a chat message of 1024 characters
where 256 are allowed, an item tag with an int array of 2^31 entries, and 20000 keep alives. Each case passes when the server logs why it cut the client off ("it sent a packet the
server could not read", or "it sent N packets in 5 seconds"), ends the client's session ("left the game") and still answers its console. `--keep` keeps the server's folder; name
cases to run only those. It needs the debug programs and the game's data, so it is not part of ctest. Not covered yet: the per-address and pending-connection limits, which need a
second local address.


## Both protocols on one server

`python3 tools/harness/protocols.py` starts a dedicated server in a temporary folder (game port 25591,
`protocols=reborn,legacy`), joins it with a Reborn client (`--protocol reborn`, harness port 25581,
named `reborn`) and a Legacy client (harness port 25582, named `legacy`), checks from the server's log
that the Reborn hello agreed batching and zstd, then moves both players twice with `tp` and reads
each client's position back, then has the server say a 250 character line and checks both stay, and has each client type a 599 character message (the Reborn one must arrive whole, the Legacy one cut to 255). It prints `ok` or one `FAIL` line for each problem and exits 1 on any.
## Peer to peer

Start a client with `up --launch-arg=--p2p-server=HOST:PORT
--launch-arg=--p2p-server-key=HEX --launch-arg=--p2p-dir=DIR` (the key is the
one `LCERebornP2pServer` prints; the folder holds the client's identity and
friends). `up --launch-arg=ARG` passes any launch option to the game, and may
be repeated. The debug protocol gains one command, `p2p`, with the request
`{"cmd":"p2p","action":ACTION,"text":TEXT,"nickname":NAME}` and these actions:

| Action | Result |
| --- | --- |
| `status` | `status` (one line), `online` (reached the rendezvous server) and `code` (this client's friend code) |
| `add-friend` | looks `text` up as a friend code and adds the identity, named `nickname`; answers at once, the friend appears in `friends` a moment later |
| `friends` | `friends`: each with `key`, `nickname` and `code` |
| `lobbies` | `lobbies`: each with `owner` (the key to join), `name`, `players`, `max`, `protocol` and `version`; asks the server again, so read it twice to see a new lobby |
| `open` | opens the hosted world to the network, which publishes it to friends; the result is `open` |
| `invite` | invites the key in `text` to the hosted world |
| `join` | joins the lobby whose owner key is `text`, as Join Game does; the result is the join result code |

Every action is an error when peer to peer is off. A host and a guest on one
machine need two ports (`--port`) and two `--p2p-dir` folders; the guest can
stay at the main menu until it joins.

## Identities

`--cheats on` lets the host use commands in a world started from the command line (the host is an operator only when cheats are on, so the chat box suggests only the commands everyone may use otherwise). `--mods-dir` points the game or server at its mods folder (`modding.md`).

The game keeps the player's name, UUID, skin and key in an identity file (`identities.md`). A client's identity folder is `--identity-dir`, or `identities` inside `--p2p-dir`, or `identities` in the saves folder. `--username` only names the first identity made in an empty folder; after that the active identity decides the name. Two clients that must be different players need different folders: the same folder means the same identity, which cannot be online twice. `harness.py identity` answers with the identities (`stem`, `name`, `uuid`, `skin`, `bundledSkin`, `usable`, `active`) and whether the identity is online; `select` and `rename` apply to the running game at once.

## Waiting and stepping

`wait` replaces polling: `wait 40` returns after 40 server ticks, two
seconds of game time, and `wait 10 --frames` after ten drawn frames. The
reply comes from the game when the count is reached, or as an error if it
is not reached within twice the expected time (at least ten seconds).

`hold` stops the server's ticks through a PC seam in `MinecraftServer`,
separate from the game's own server pause, so saving or closing the pause
menu does not undo it. `step N` then runs exactly N ticks, each the same
work a normal tick does, and returns when they have run. `wait` in ticks is
refused while ticks are held, and `step` is refused while they are not. The
client keeps drawing and keeps its own predicted day-time clock running,
so `level.dayTime` in `state` moves while held and snaps back to the
server's value after `release`; `serverTicks` is the held clock.

## Profiling

`profile --seconds 10` samples `state` twice a second and prints the
average, minimum and maximum of each figure; `--json` prints the same as
JSON for a tool to compare runs.

- `frame ms` and the section times come from the game's own timing points
  (`InfoBarManager`), which the original leaves empty and the debug build
  fills in. Frame time is main-thread work up to presenting the frame, so
  it excludes the frame-rate wait. Terrain, entities, particles and the UI
  are sections of it, summed over every viewport in split screen.
- The `gpu ms` rows time the same sections on the GPU, with OpenGL
  timestamp queries placed where each section starts and ends. Results are
  read four frames later so the CPU never waits for them; a frame whose
  results are not ready by then is skipped. The debug contract is
  `platform/DebugGpuTiming.hpp` and the PC backend is
  `PcDebugGpuTiming.cpp`; a driver without timestamp queries reports no
  GPU rows. On llvmpipe the GPU is the CPU, so the two sets of figures
  overlap.
- `server tick ms` is the server's own running average.
- `resident MiB` is the process's resident memory.

Every figure is an exponential average over about 20 frames or ticks, so a
single slow frame shows as a bump, not a spike. Sections are the game's own
timing points, not render passes; a pass inside a section is not split
out.

## Scenarios

A scenario is a feature check that can be rerun alone. Each is a JSON file
in `tools/harness/scenarios/`, and `tools/harness/scenario.py` documents the
format and is the only code that reads it:

```sh
python tools/harness/harness.py check --list
python tools/harness/harness.py check nbt-path-filters
python tools/harness/harness.py check
```

A scenario names its world by seed and mode, and `check` brings that world
up the way `up --seed --mode` does, reusing a client already in it. When
the next scenario needs a different world, `check` saves and restarts the
client and says so. Steps
run console commands and match their replies (`expect`, or `expect_error`
for a reply in the chat error colour), wait, hold and step ticks, or compare
a value from `state`, or read the client player's own data (`client`) to
check what reached the client rather than what the server holds. Each step
prints `ok` or `FAIL` with the reason, every
step runs even after a failure, and held ticks are released at the end.

Add a scenario when a feature is finished, from the checks that proved it.

## Asking the user

Some checks are cheaper for a person than for the harness: a screen with no
`screen` name yet, a flow that needs many inputs, or a judgement about what
the frame looks like. `ask` sends the request into the game chat, the way
`/tell` does, and returns at once:

```sh
python tools/harness/harness.py up --windowed --seed 12345
python tools/harness/harness.py ask "Does the sky look blue?" --option "Yes" --option "No"
python tools/harness/harness.py ask "Open a furnace, put coal in it, and close it."
python tools/harness/harness.py answers --wait
```

The player sees "Agent asks (request N): ..." with the options numbered,
and answers whenever they are ready, with debug-only chat commands:

| Command | Does |
| --- | --- |
| `/agent` | list the requests still waiting, with their options |
| `/agent answer <request> <answer>` | answer with an option number or any text |
| `/agent done <request>` | mark a task finished; the answer is `done` |

`answers` prints every request and its answer, or only one with its id;
`--wait` blocks until the requests it shows are answered, and `--timeout`
limits the wait. Polling counts as activity, so a long wait does not trip
the idle limit. Requests need a hosted world, since they travel through
the chat, and every player in the world sees them. On a headless client
`ask` still works but warns that no person can see it.

## Lifetime

The game quits itself when no debug request has arrived for
`--debug-idle-quit` seconds. The harness passes `--idle` (default 120) and
the number is part of the session. A forgotten client, or a harness that
died, cannot leave a game running.

`down` asks the protocol to quit, waits, then terminates the process if it
must, and stops the Xvfb it started. It is safe to run when nothing is up.

`pause` stops the process, so a client can wait between questions without
holding the CPU or paying for another boot. Other commands refuse while it
is paused rather than hang. Time spent stopped does not count toward the
idle limit: the game treats a gap of more than two seconds between frames as
a stop and moves its idle clock forward by it.

## The protocol

TCP on `127.0.0.1`, one JSON object per line in each direction. The port is
chosen with `--debug-port` and is off when the flag is absent. One client is
served at a time.

Request:

```json
{"id": 7, "cmd": "state"}
```

Reply:

```json
{"id": 7, "ok": true, "result": {"inWorld": true, "ticks": 1204, "...": "..."}}
```

Failure:

```json
{"id": 7, "ok": false, "error": "commands need a hosted world"}
```

The id is echoed, so a client can match replies. Unknown commands and malformed
requests are errors, never silence.

| `cmd` | Parameters | Result |
| --- | --- | --- |
| `state` | | `inWorld`, `eula`, `uiMode`, `menuDisplayed`, `pauseMenu`, `ticks`, `frameRate` (frames drawn in the last second), `scenes` (`group`, `layer`, `name` for the top scene of each layer; `name` is the scene's movie, or `scene type N` for a native screen without one), `tasks` (every `ask` request and its answer), `profile` (`sectionsMs`, `gpuSectionsMs` with null for a section not yet measured, `serverTickMs`, `residentBytes`), `ticksHeld`, `serverTicks` (while a world is hosted), and while in a world `player` (`x`, `y`, `z`, `yaw`, `pitch`, `dimension`) and `level` (`name`, `dir`, `seed`, `dayTime`, `raining`, `thundering`, `host`) |
| `console` | `since` sequence number | `lines` (`seq`, `text`) and `next` |
| `command` | `line` | `lines` written by the command |
| `capture` | `path` | `path`, `width`, `height` once the frame is written |
| `key` | `name`, `down` (1 or 0) | `queued` |
| `pointer` | `x` and `y` from 0 to 1 | `queued` |
| `type` | `text` | `queued` |
| `client-nbt` | optional `path` | `nbt`: the client player's own data as SNBT, as the client received it, whole or at the path |
| `touch` | `x` and `y` from 0 to 1, `phase` (0 down, 1 move, 2 up, 3 cancel), optional `id` | `queued` |
| `screen` | `name` | `opened` |
| `hold` | `hold` (1 or 0) | `held` |
| `wait` | `count`, `unit` (`ticks` or `frames`) | `reached`, sent once the count has passed |
| `step` | `count` | `reached` (steps run so far), sent once the ticks have run |
| `ask` | `text`, optional `options` (array of strings) | `id`; the request and its answer appear in `state` under `tasks` (`id`, `text`, `options`, `answer`, null until answered) |
| `quit` | | `quitting`, then the game exits |

`command` runs on the server thread through the console command source and
answers only after the command has run. Its reply carries the console lines it
produced, so a caller never has to guess when output has stopped.

`key` queues an ordinary SDL key event, so the game sees the same thing a
keyboard would send. It is not a gamepad and does not bypass focus handling.
The debug server remembers which modifier keys the harness holds down and
sets them on later key events, as SDL does for a real keyboard, so
`key "Left Ctrl" down`, `key a down`, `key a up`, `key "Left Ctrl" up` is
Ctrl+A. `type` queues a text input event; key events alone produce no text.

`screen` opens a screen through the same path the game uses, on the primary
player, without needing the button that would normally open it. It is how a
screen is checked before the flow that reaches it exists. `settings` opens
the settings list with the init data the Settings menu passes it, in a world
or not. The pause menu's own settings entry is the mini game host settings,
which is not this.

`up --ui-mode bedrock` starts the client in the Bedrock interface; `state`
reports it as `uiMode` (0 legacy, 1 bedrock) and reports `pauseMenu` for
whether a pause screen is on the stack.

`touch` sends the finger straight to the touch layer, because a headless
client has no touchscreen for SDL to report. It runs the same gesture code a
real screen does, so it can drive the touch controls and the menu pointer. A
finger that has been down once is enough for the client to report a
touchscreen, which shows the touch rows in the controls menu.

## Driving the menus with keys

A press is `key NAME down` then `key NAME up` at once; a key held for 0.3 seconds repeats and
moves two rows. The scene is read with `screen` (the debug overlay and the shader menu both say
`MultilistMenu`, so look at a screenshot to tell). From a world that has finished loading, with the
game's own keys: `Escape` (PauseMenu; `Up` from Resume wraps to Exit Game, so go `Down`), `Down`
then `Return` (Help & Options), `Down` three times then `Return` (Settings), `Down` twice then
`Return` (More Options), then `Down` nine times and `Return` for the Debug Overlay, or `Down` ten
times and `Return` for Shaders. Checked on screen: in the Debug Overlay `Return` changes the
focused switch and `R` changes the preset ("Default" to "Minimal"); in Shaders `R` and `Q` switch
tabs, skipping the greyed-out Shader Pack Settings tab and wrapping around. A touch tap clicks in
both (checked 2026-11-12: a tap on a debug overlay switch segment sets it, a tap on a shader menu
tab opens it), and so does `pointer X Y` followed by `key MouseLeft down` and `up`.

## Limits

- `down` saves the world before the client exits, using the same action the
  pause menu's save uses. It waits up to 90 seconds for the client to exit on
  its own, because an interrupted save can lose a world. A client that joined
  someone else's game has no world of its own to save.

- `command` needs a hosted world. A client that joined someone else's game
  has no console command source and answers with the error above.
- `up --size WIDTHxHEIGHT` (at least 320x180) draws at that size, and the frame grab has it too; 640x360 took a headless client from about 3.3 cores to 2.0.
- A headless frame costs about four cores of llvmpipe at 1280x720. The
  10 fps cap holds, but llvmpipe alone reaches only about 11, so the cap
  saves little on a machine like the one this was measured on; `pause` is
  what frees the CPU.
- `capture` reads the back buffer, so it must be requested while the game is
  drawing; a paused client still draws.
- The protocol is a debug feature. It must not become the way the game does
  anything, and a release build must be unaffected by its existence.

## Adding to it

Add a command to `PcDebugServer.cpp`, an entry to the table above, and a
method to `tools/harness/protocol.py` if it needs one. Do not put a second
JSON reader or socket wrapper in a tool; `PcJson` and `lcenet::TcpSocket` are
already there, and `tools/harness/session.py` is the only place that starts or
stops a client.

## Scenarios that run example mods

A scenario may name `"mods"`: a list of folders of `docs/examples/mods`. The check copies them (and `lcestd`) to a temporary folder and starts the client with `--mods-dir` on it, so
the scenario proves the examples work in a running game, not only that they load. `python3 tools/harness/harness.py check mod-examples` does this for the registry, derived content,
mod block stores, the network channel, translations, a model, saved data, commands and a mod dimension. A step `{"mods": "list", "expect": REGEX}` matches the loaded mods as the `mods` request reports them, `{"key": "MouseRight", "ticks": 5}` holds a key or mouse button for that many ticks, `{"type": "text"}` types text, `{"save": true}` queues a save of the hosted world (follow it with a wait; the other scenarios share worlds that are never saved: `check` turns automatic saves off with `/save-off` before each scenario, but the world is still saved when a scenario's client exits, so a scenario cannot assume where the player stands (the summoning ones move the player into open air first); a scenario that saves on purpose names a seed of its own), a scenario that summons starts with a wait of 100 ticks (a world made moments ago is still loading chunks, whose animals would fill the cap again), `kill @e[type=!player]` and a wait of a few ticks, because a killed entity counts toward the cap until the tick that removes it (the console caps how many of each mob a world holds, a fresh world's own animals can fill the cap, and a summon run through `execute` tells the player, not the harness, that it failed), and `"allow_error": true` on a command step accepts an error (for housekeeping such as clearing an empty inventory); `check` starts the client with `--hints off`, so a hint never takes a key or a click, (the scenario opens a chest and casts a rod this way). A click is `"ticks": 1`, and the rod is used in survival with `lootcancel on` around the checks that need a bobber out:
the game reads the mouse once a frame, so a headless frame that runs late covers several ticks with the button down, and in creative any quick click, or in survival a frame of five
ticks or more, uses the item twice (cast, then reel in). With retrieves cancelled the bobber stays out either way. The check deletes the temporary mods folder after each scenario. It needs the game's data, so it is a harness check and not part of ctest, which loads every example.
