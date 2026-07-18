# CNA Examples

A single, cross-platform, in-app catalog of live demonstrations for every area of
[CNA](https://github.com/openeggbert/cna) — a C++ reimplementation of the XNA 4.0 programming
model built on SDL3. Conceptually inspired by
[javafx-ensemble8](https://github.com/lusalome/javafx-ensemble8) (browsable sample catalog with
in-app navigation), but entirely CNA-specific — no JavaFX code, assets, or dependency involved.

Pick an area from the home menu (Input, Audio, Devices, Net, Media, 2D Graphics, 3D Graphics, ...),
drill into a category, and run the demo — all inside one application, on desktop, web or mobile.

See [plan.md](plan.md) for the full architecture and navigation design.

## Status

The **Input** area is complete: all five categories — Keyboard, Mouse, Gamepad, Touch, Other —
have real, working demo screens (50 total), each exercising an actual
`Microsoft::Xna::Framework::Input`/`CNA::Input` API call, not a mock. See [plan.md](plan.md)
§5.1 for the full per-category breakdown.

The other six areas (Audio, Devices, Net, Media, 2D Graphics, 3D Graphics) are registered on the
Home menu but still show an empty "(coming soon)" category list — real demo content for them is
in progress. Only the `EASYGL` graphics backend is targeted for now.

**Hardware-verification note:** Keyboard, Mouse, and most of Other's demos were verified against
real input on the dev machine this app is built on. Gamepad, Touch, and Other's
joystick/haptics-device screens render correctly and handle "nothing connected" gracefully, but
have not yet been exercised with an actual controller, touchscreen, joystick, or haptic device —
see [plan.md](plan.md) §5.1 for exactly which screens still need that pass.

## Navigating the app

- **Keyboard/gamepad:** Up/Down (or D-pad/left stick) to move the selection, Enter/Space/gamepad
  A to select, Esc/gamepad B to go back.
- **Mouse/touch:** click or tap an entry to select it; click/tap the "< Back" hint at the
  bottom-left to go back.
- A category with more entries than fit on screen scrolls automatically to keep the selected
  entry visible as you navigate with Up/Down.

## Platforms

| Tier | Platforms |
|---|---|
| Now | Windows, Linux, Web (Emscripten), Android |
| Future | macOS, iPhone, consoles |

## Prerequisites

| Tool | Version |
|---|---|
| CMake | ≥ 3.20 |
| C++ compiler | C++23 (GCC 13+, Clang 16+, MSVC 19.38+) |
| CNA | sibling directory `../cna` |
| sharp-runtime | sibling directory `../sharp-runtime` |

Clone all three side-by-side:

```
openeggbert/
├── cna/
├── sharp-runtime/
└── cna-examples/       ← this repo
```

## Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run it:

```bash
./build/cna_examples
```

The graphics backend defaults to `EASYGL`; override with
`-DCNA_GRAPHICS_BACKEND=<SDL_RENDERER|EASYGL|VULKAN|BGFX|WEBGPU>` if needed (subject to CNA's
own backend maturity — see CNA's `CLAUDE.md`).

## Project structure

```
cna-examples/
├── plan.md                        Full architecture + per-area/category breakdown
├── CMakeLists.txt                 Top-level build (sibling add_subdirectory of ../cna)
├── Content/                       Menu font (baked via tools/gen_menu_font.py) + UI textures
├── tools/gen_menu_font.py         Regenerates the menu SpriteFont from a system TTF
└── src/
    ├── Program.cpp                 Entry point
    ├── CnaExamplesGame.hpp         Game subclass; owns GraphicsDeviceManager + ScreenManager
    ├── GameStateManagement/        Screen-stack navigation (adapted from the XNA
    │                                "Game State Management" sample)
    ├── Navigation/                 HomeScreen/AreaScreen/CategoryScreen + AreaCatalog.hpp
    │                                (the single data-driven Area → Category → Demo registry)
    └── Demos/                      One subfolder per Area, one file per demo screen
        └── Input/
            ├── Keyboard/, Mouse/, Gamepad/, Touch/, Other/    (10 demo screens each)
```

Adding a new demo means: write a `DemoScreen` subclass under `Demos/<Area>/<Category>/`, then
register it with `MakeDemo<YourScreen>(title, description)` in `AreaCatalog.hpp`'s
`Build<Category>Demos()` function.

## Development

Active development happens on the `develop` branch; `master` tracks the latest stable state.

## License

MIT — see [LICENSE](LICENSE). This is original CNA-specific work, not a port of Microsoft's
XNA sample collection (compare `../cna-samples`, which is Ms-PL for that reason).
