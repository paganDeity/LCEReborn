# LCEReborn agent guide

LCEReborn is a matching decompilation of Minecraft: Wii U Edition, the 4J
Studios Legacy Console Edition codebase, plus a native PC version built from
the same source. `README.md` describes the result and `docs/` describes how it
is built and how it is checked. This file is the working agreement for agents
changing it.

## Document precedence

1. The user's request.
2. This file.
3. The relevant document under `docs/`.
4. The source and the build configuration.
5. `README.md`.

`docs/provenance.md` is the authority on where the source comes from. Do not
treat any status line, including a figure in this file or in `README.md`, as
more current than the source.

`docs/todo.md` is the one list of planned work, in the order it is taken.
Read it before choosing what to work on. Planned work goes there, not into
source comments, other documents or this file; a document may say a gap
exists and point to the list.

## The method

The source is written by reading the original Wii U executable and refined
until it compiles to the same code. That is not optional context, it is the
definition of correct here.

- Every function that changes is read from the original's compiled code
  first, then written by hand in the project's own style.
- A change is verified function by function with `tools/build.py` and
  `tools/measure/parity.py`, against the executable in the update 688 dump.
  `docs/measuring.md` says what parity checks and what it does not.
- Parity is a check of calls, constants, strings and branch counts. It is
  not a check of behaviour. Behaviour claims need a measurement, a save, a
  wire capture, or a disclosed gap.
- Never commit binaries, disassembly, symbol tables, decompiler output or
  game data. Local working files stay outside the repository.
- The Xbox 360 TU75 build is the behavioural cross-check when the Wii U
  evidence leaves a question open. Do not substitute memory of Java Edition
  for it.

State a result as one of these, and say which:

- Matches the checked behaviour of the cited source or measurement.
- Ported with one disclosed divergence: the difference and its reason.
- Not implemented: the missing behaviour or prerequisite.

A plausible result is not evidence. A stub that returns a fixed value is a
gap until its consumers and intended behaviour have been traced.

## Source rules

`docs/conventions.md` governs every file under `source/`. The parts that are
missed most often:

- C++03 for game and shared code: no `auto`, lambdas, range-for, `nullptr`,
  `override`, `constexpr`, rvalue references, or newer standard types.
  `source/platform/pc/` is built as C++17 and may use that library, but keeps
  the same naming, layout and comment rules.
- One class family is one header and one implementation file, named after the
  outermost class.
- No comments, no placeholders, no stubs, no commented-out code, with one
  exception: a fix to original code that was not checked against the
  original keeps the fix and is marked `// Parity Not Checked` on the line
  above it, until parity confirms it (`docs/conventions.md`).
- Includes are `source/`-relative, the family header first.
- Four-space indentation, braces on their own line, 120 columns, LF.

Do not reformat or refactor code outside the change. A behavioural fix and a
tidy-up are separate commits.

## Data-oriented direction

The game model is object oriented, and it stays so where it is. The direction is
toward data-oriented design because it lowers complexity and technical debt, uses
less memory, and keeps hot loops in cache. It is a direction for work already
being done, not a rewrite: every change that touches content definitions, the
mod API or a hot path should leave it closer to this, and none may ship a
half-converted subsystem or add an ECS for its own sake. It does not reach the matched decompiled game
classes (see "The method"): their structure is what parity checks, so they change only for a behavioural
reason, and `docs/todo.md` says where the direction stands.

- Authoring and runtime are separate concerns. Inheritance describes content:
  a definition names a parent and overrides only what it states, and a child of
  `Door` gets the door's behaviour, sounds and texture layout unless it
  overrides them. It must not dictate memory layout or execution.
- Resolve once. Parent defaults and overrides are merged at registration, never
  during play, into an immutable definition that runtime code reads by a
  compact numeric id. An absent property is not the same as one set to its
  default, a cycle or a missing parent is an error with a message, and
  resolution is deterministic at any depth.
- Static data lives in the definition, mutable state in the instance. A
  per-instance copy of something every instance shares (a block's hardness, an
  item's stack size, a mob's attributes) is debt; a virtual getter that only
  returns a constant is the same debt.
- Prefer contiguous, id-indexed storage and structure-of-arrays for a path that
  runs over many things each tick (chunks, light, entity ticking). Keep virtual
  dispatch out of such loops.
- Some properties are computed or have side effects and ordering rules. Find
  them before choosing how a property is stored; do not flatten them blindly.
- Behaviour a mod changes goes through the generated symbols, and definitions a
  mod makes use the same resolution as the game's own, so one mechanism serves
  both. `docs/todo.md` ("Content definitions") has the plan.

## Platform boundaries

The PC backend lives under `source/platform/pc/` and answers the same
interfaces as the Wii U backend. Game code may not branch on the operating
system. A PC-only feature belongs in the PC backend behind `LCE_PLATFORM_PC`,
or in a narrow seam the game already has.

Debug capabilities are gated by intent, not by platform: a hook, protocol
handler or string that exists for debugging sits under `#if
LCE_DEBUG_BUILD` even inside game code, so it never reads as PC game
behaviour and can gain a backend per platform later.

The Wii U image is built by `tools/build.py` with RedHills. PC-only code must
not reach that image through a shared translation unit without a guard, and a
PC-only flag must never change how the Wii U build behaves.

## Commands

Chat commands are a project choice, not a fidelity claim. The target syntax is
Java Edition 1.14, including NBT arguments (`dataTag`) and the 1.13 NBT path
grammar, as recorded in `docs/commands.md`. The retail command surface is
context for what LCE shipped, not a limit on this target.

- Reuse `ChatCommands`, `CommandArguments`, `TargetSelector`, coordinate
  parsing and the existing permission checks. Do not add a second dispatcher.
- Syntax and game capability are separate. If a command needs a system the
  source does not have, leave it missing and say so.
- Append new binary command ids; never renumber a wire value.
- Replies go through the existing chat path. Never send a raw translation key.
- Typing a command, in chat or a command block, behaves as modern Java
  Edition (1.23.6 and 1.24) does: suggestions, argument hints, Tab
  completion and error highlighting. The 1.14 target governs syntax only.
- Command blocks run commands through the same dispatcher as chat. Their
  screen is designed with the project owner before any of it is written;
  stop and ask when the work reaches it.

## The debug protocol and the harness

The PC build has a debug protocol so agents can drive and inspect a running
game instead of rebuilding and clicking through menus for every question.

- The protocol is a localhost TCP line protocol carrying JSON, enabled only in
  debug builds (`LCE_DEBUG_BUILD`), off unless `--debug-port` is given.
- One implementation of the client side lives in `tools/harness/`. A tool
  must import it rather than copy connect, boot or request code.
- The harness keeps one client alive between questions (`up`, `down`), and
  must be able to run headless or against a window.
- Protocol changes are documented in `docs/harness.md` in the same commit.
- Debug-only features must not become a second game loop. Prefer reading
  existing state and injecting ordinary input events.
- The portable-lce harness (`scripts/ui/` in portable-lce) is the reference
  for what this one should offer. Headless runs are muted.
- When driving the interface would cost more than asking, run the client
  windowed and ask the user with `harness.py ask`, offering answers with
  `--option` where they help. The request arrives in the game chat and the
  user answers with `/agent` when ready; read it with
  `harness.py answers --wait`.
- Profiling data (CPU, GPU, memory) is read through the same protocol, not a
  separate channel, and costs nothing when the protocol is off.

## Servers

The dedicated server (`LCERebornServer`) and the server inside the game are one server: they build
from the same source and have the same features. A server feature (a list, a limit, a command, a
protection) is written once, in game code or in a PC module both programs start, never inside
`PcServer.cpp` alone and never behind `IsDedicatedServer()` unless the difference is the absence of
a local player or a window. `docs/pc-platform.md` says how each is configured.

## Network

There are two wire protocols behind one network contract. Game code talks to
the contract and never to either protocol directly.

- Legacy is the wire format as it was before this project changed anything.
  Other projects built on LCERecomp and the Wii U build speak it. Its bytes
  are frozen: it has no capability negotiation and is never extended.
- Reborn is planned and not started (`docs/todo.md`). It is incompatible with
  Legacy and with everything before it, on purpose: lower overhead, lower
  latency, resilient, and no compatibility debt. Negotiation, feature gating
  and play between versions belong to Reborn only. At connect, client and
  server agree capabilities, so a newer game joins an older one and the
  reverse, and a feature the other side has not agreed to is never sent.
- Peer to peer (`docs/p2p.md`) is a transport, not a protocol. It carries
  either one.

Save and format capabilities (`docs/compatibility.md`) are a separate rule
and not part of the wire protocol: each extension is a capability, data that
needs one is never written where a reader has not declared it, and a save
records the capabilities its data uses. A world that uses none is saved
exactly as the original saves it.

## Modding

The modding API is `docs/modding.md`. Mods load at start and reload while the game
runs. Scripts are Lua 5.4. Native mods are WebAssembly (wasm3), so one build runs on
every platform; `dlopen()` may be added later behind the same interface, not as a
second one. Mods reach the game through `ModHost` (`source/game/core/ModHost.hpp`,
with a PC backend and an inert Wii U one), and game code does not branch on whether a
mod is loaded. The API is generated (`tools/hooks/hookgen.py` from `api/lce.api` and the
headers), which keeps the Lua and WebAssembly runtimes and the docs in step; `ctest` checks it, and loads every
example mod. A call is a documented method of a game class or of a loader class, exposed with one `expose` line
(and `alias` to name it for Lua); a hook is a woven game method (`intercept`). There is no hand-declared function
and no host code, so adding to the API is writing the method. What API version 1 promises is `api/stable.txt`
(`modding.md`): when `hookgen.py check` says a promised function or hook is gone or changed, keep the old one working
rather than editing the file; a line of it is never removed or changed within an API version.

Never write a C++ function or class only so the generated API can hook or call it. A hook goes on code that does the work, and the game's own
default behaviour lives in that code, so cancelling the hook takes the event. When the system has no such seam, change the system (a typed
call instead of `void*`, a page held as data instead of a class per menu) so the symbol exists naturally for the game and for mods alike.

## Tooling mandate

Tooling is held to the same standard as the game code, and this is a hard
requirement, not a preference:

- No technical debt. A tool that is known to be wrong, approximate or
  unfinished is fixed or removed, not documented as a quirk.
- Orthogonal. One tool has one job, one module owns one concern. Parsing,
  transport, boot and reporting are separate modules, not copies.
- Reproducible. Same inputs, same output. Pin versions, hash what is
  downloaded, make hidden state explicit (pid files, ports, temp folders),
  and clean it up.
- DRY. One source of truth for a format, a protocol, a constant or an
  algorithm. If two tools need it, it moves into a shared module.
- Simple. The smallest design that does the job. No speculative abstraction,
  no framework, no generated code for a hypothetical need.
- Honest. Fail loudly with a clear message, validate input, never report
  success for something that did not happen, and never touch saves or
  profiles without saying so.

Python tools: Python 3.8 or newer, standard library only unless
`docs/building.md` already pins the dependency.

## Change discipline

Before editing, read the relevant source, the build files, the nearby
callers, and recent history. Trace producers and consumers of anything called
incomplete. State assumptions and what remains unverified.

After editing, run the narrowest check first, then the build that covers the
change:

```sh
cmake --build build/pc --target LCEReborn -j 3
```

A change to a header, to `api/lce.api` or to the generator is also built in the Release folder
(`cmake --build build/pc`), because debug-only code sits under `#if LCE_DEBUG_BUILD` and a
generated call to it compiles in a debug build and fails in a release one; the generator leaves
those blocks out, and this build is what shows that it did.

For a Wii U change, the covering check is `tools/build.py` followed by
`tools/measure/parity.py`; a change that cannot be run through it is not
verified and must say so.

Never interrupt a build, never run two builds in the same folder, and never
report a result that was not observed.

## Documentation map

- `docs/overview.md`: what is here and the current status.
- `docs/todo.md`: planned work, in order.
- `docs/provenance.md`: where the source comes from and what is excluded.
- `docs/measuring.md`: parity, and what it does and does not prove.
- `docs/conventions.md`: the rules every source file follows.
- `docs/architecture.md`: the directory layout and the platform seam.
- `docs/building.md`: preparing game files and building the RPX.
- `docs/pc-platform.md`: building and running the PC version.
- `docs/pc-input.md`: keyboard, mouse, controllers and touch on PC.
- `docs/commands.md`: the command target, the inventory and the gaps.
- `docs/harness.md`: the debug protocol and the agent harness.
- `docs/handoff.md`: where the work stands and how to approach the large pieces, dated.
- `docs/interface-design.md`: the proposal for identities, friends, chat, voice and the Bedrock slice order, waiting on the owner (plan decisions D7 to D11).
- `docs/plan.md`: the execution plan over everything pending, in order, with the check for each piece and the decisions that are the owner's (approved 2026-11-11).
- `docs/modding.md`: the mod API; `docs/fabric-comparison.md`: it next to Fabric's, with status.
- `docs/compatibility.md`: format capabilities on the wire and in saves.
- `docs/bedrock-ui.md`: the Bedrock interface, its screens and its gaps.
