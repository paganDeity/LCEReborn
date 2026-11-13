# Source conventions

These are the rules every file under `source/` follows. They exist so the
tree reads as one codebase rather than as thousands of independent efforts.

## Language level

- C++03, the language level the original was written in. No `auto`,
  lambdas, range-for, `nullptr`, `override`, `constexpr`, rvalue references
  or `std::` types newer than C++03.
- boost 1.x for smart pointers, unordered containers and hashing
  (`boost::shared_ptr`, `boost::unordered_map`, `boost::hash`). The standard
  library covers everything else.
- No compiler extensions and no `#pragma pack`, except where the original
  used a processor instruction directly (the time base, byte-reversed loads,
  paired-single maths): that code sits behind a compiler check, with a
  version for each compiler that does the same thing.

## One family, two files

- A class family is one header and one implementation file, both named
  after the outermost class: `Block.hpp` and `Block.cpp`.
- Nested classes, local helper classes and anonymous structs live inside
  their outer class's files.
- A template and all of its instantiations are one family.
- Header-only families (interfaces and enum owners with no symbols of their
  own) have a header and no implementation file.

## Naming

- Classes keep the names the build exposes: Mojang-style names for the game
  core (`LivingEntity`, `BlockPos`), 4J's `C`-prefixed names for the console
  layer (`CConsoleMinecraftApp`, `CStorage`) and `FJ_` / `fui` for the UI.
- Game-core members use the Mojang `mName` style; the 4J console and UI
  layers use `m_name`, with `m_p` for pointers; statics use `s` or `s_`
  to match their layer.
- Parameter, local and field names that the build does not record are
  chosen to read naturally in context. They are provisional and may be
  renamed as understanding improves.

## Content

- Files contain code only: no comments, no headers describing where the
  code came from, no annotations, no TODO or placeholder markers. The one
  exception is the parity marker below.
- The PC is one behavioural target, `LCE_PLATFORM_PC`; Windows and Linux
  are operating system targets within it, and the Wii U is the other
  behavioural target. Behaviour the PC build has is written once for the
  PC; per-OS files only say how each operating system does the same thing.
- Operating system and processor differences live in separate files: one
  interface (`name.hpp`, and `name.cpp` for what every platform shares),
  implemented per platform in `name_os.cpp`, `name_arch.cpp` or
  `name_os_arch.cpp`. The operating system names are `windows`, `linux`,
  `darwin`, `android`, `ios`, `freebsd` and the like, with `wiiu` for the
  Wii U; the architecture names are `amd64`, `arm64`, `386`, `arm`,
  `ppc64le` and the like, with `ppc` for the Wii U. `android` also takes
  `_linux` files and `ios` also takes `_darwin` files. A folder holds
  either double-suffixed files or single-suffixed ones, never both. The
  build defines `LCE_OS` and `LCE_ARCH` as strings for code that needs the
  name itself.
  `tools/build/platform_files.py` is the one rule; CMake and the Wii U build
  both use it. Do not branch on `_WIN32`, `__linux__` or similar macros in
  shared files; code that means "the PC build" uses `LCE_PLATFORM_PC`, and
  code that can be portable is written once.
- Code that exists only for debugging (harness hooks, debug protocol
  support, strings only a debug feature shows) is gated with
  `#if LCE_DEBUG_BUILD`, never with a platform macro, even when only one
  platform has a debug backend today. The gate says why the code exists,
  and a debug contract can later get a backend per platform without the
  hook looking like game behaviour. `LCE_PLATFORM_PC` is for behaviour the
  PC build ships.
- A fix to code that came from the original, made without checking the
  original's compiled code, keeps the fix and carries the line
  `// Parity Not Checked` directly above the changed statement. It marks
  the change as a fix of ours, not a reading of the original, until
  `tools/measure/parity.py` or the disassembly confirms or replaces it;
  the marker is removed then. New code with no counterpart in the
  original does not carry it. `grep -rn "Parity Not Checked" source`
  lists the fixes still waiting.
- No stubs. A method exists with a real body or it does not exist yet.
  A declaration with no definition is used only where the original build
  itself has none.
- No machine-generated idioms: no raw memory loads and stores standing in
  for member access, no hand-built vtables, no decompiler identifiers.

## Layout and includes

- Includes are `source/`-relative and sorted with the family's own header
  first, then project headers, then library and system headers.
- Forward declarations are preferred in headers; implementation files
  include what they use.
- The layout is the one `source/.clang-format` describes: four-space
  indentation, braces on their own line, 120 columns, LF line endings,
  UTF-8 without BOM. It was applied with clang-format 22; the few files
  listed in `source/.clang-format-ignore` keep their layout.
