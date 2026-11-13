# Bedrock interface

The Bedrock interface is the phone-first interface, selected by
`eGameSetting_UIMode` or by `--ui-mode bedrock` for one run. It is built as
native screens under `source/platform/pc/`, registered through
`source/game/ui/BedrockUI.hpp`, and drawn over the game's UI buffer.

The layout and colours follow Cinnabar's OreUI screens
(`crates/oreui/src/home.rs`, `pause.rs`, `settings.rs` and
`crates/ui/src/oreui_theme/mod.rs`). Cinnabar is the reference for geometry
and palette; the game's own data supplies the actions and the settings.

## Implemented screens

| Screen | Scene | State |
| --- | --- | --- |
| Home | `EUIScene_MainMenu` | title, Play, Servers, Settings, Skins, character panel |
| Pause | `EUIScene_PauseMenu` | Resume game, Settings, Save & Quit, character panel |
| Settings | `EUIScene_SettingsListMenu`, `EUIScene_InGameSettingsListMenu` | General, Video, Audio, Controls, Touch |

The factory in `PcBedrockUI::CreateScene` returns one of these for the scene
type, and returns null for every other screen so `UILayer` falls back to the
Legacy implementation. A screen without a Bedrock version therefore stays
reachable.

## Shared paint layer

`PcOreUI` owns the palette, the rem scale, the text renderer and the widgets.
One rem is five GUI pixels, so the scale is the same as Cinnabar's. The
widgets are the ones the reference screens use: a dimming overlay, a
`NEUTRAL80` panel with a dark border, raised buttons in the hero, secondary,
neutral and destructive variants, list rows, switches, sliders, a back
chevron and the header bar.

The title artwork is the game's single-line wordmark at
`assets/Common/res/gui/logo.png`, drawn through the same batcher as the text.

## Input

Every screen accepts a pointer, touch and the menu actions:

- Up and Down move through the screen; Left and Right change a value or the
  selected section; Return activates; Escape goes back.
- The pointer and touch use the menu pointer the game already maintains, so
  the same code works with a mouse, a real touchscreen and the harness
  `touch` operation.
- Hover only lights a control. It does not move the keyboard selection, so a
  pointer resting over the screen cannot steal the selection.

Settings are written through `GameSettings::SetGameSettings` and flushed with
`CheckGameSettingsChanged`, which is the same path the Legacy settings use.

## Disclosed divergences

- Text is drawn with the game's bitmap font, not the OreUI font.
- The character panel draws a flat front-view silhouette, not a 3D skin
  preview, and it does not show the profile's selected skin.
- The home screen has no Realms entry because the game has no Realms system.
  The Skins entry takes its place.
- Play, Servers and Skins open the Legacy world list, server list and skin
  picker until their Bedrock screens exist.
- Settings covers a subset of the game's options and does not scroll; each
  section is sized to fit the viewport.
- Save & Quit reuses the Legacy exit dialog and save flow rather than
  duplicating it.

## Checking a screen

The harness opens any registered screen without the button that would
normally reach it:

```sh
python3 tools/harness/harness.py up --headless --seed 12345 --ui-mode bedrock
python3 tools/harness/harness.py screen main-menu
python3 tools/harness/harness.py shot /tmp/home.png
python3 tools/harness/harness.py screen settings
python3 tools/harness/harness.py touch 0.89 0.67 down
python3 tools/harness/harness.py touch 0.89 0.67 up
python3 tools/harness/harness.py state --json
```

`state` reports `uiMode` and `pauseMenu`. A settings change survives a
restart because the profile is written before the client exits.

## Files

- `source/platform/pc/PcOreUI.{hpp,cpp}`: palette, scale, text and widgets.
- `source/platform/pc/PcBedrockHome.{hpp,cpp}`: home screen.
- `source/platform/pc/PcBedrockPauseMenu.{hpp,cpp}`: pause screen.
- `source/platform/pc/PcBedrockSettings.{hpp,cpp}`: settings screen.
- `source/platform/pc/PcBedrockUI.{hpp,cpp}`: scene factory.
- `source/game/ui/BedrockUI.{hpp,cpp}`: the platform seam.
