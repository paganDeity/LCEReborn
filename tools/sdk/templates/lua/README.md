# {{name}}

A Lua mod for LCEReborn. Put this folder in the game's `mods` folder (or pass `--mods-dir`), start the game, and run `/{{id}}`.

- `mod.txt` names the mod and what it needs (`depends=lcestd` is the standard library mod, from `docs/examples/mods/lcestd`; copy it beside this folder).
- `main.lua` is the entry file. Edit it while the game runs: mods reload when a file changes.
- The API is `docs/modding-api.md`; the guide is `docs/modding.md`; `docs/examples/mods` has a small mod for most things.
