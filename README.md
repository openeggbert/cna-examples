# CNA Examples

A single, cross-platform, in-app catalog of live demonstrations for every area of
[CNA](https://github.com/openeggbert/cna) — a C++ reimplementation of the XNA 4.0 programming
model built on SDL3. Conceptually inspired by
[javafx-ensemble8](https://github.com/lusalome/javafx-ensemble8) (a browsable sample catalog with
in-app navigation), but entirely CNA-specific — no JavaFX code, assets, or dependency involved.

Pick an area from the home menu, drill into a category, and run the demo — all inside one
application, on desktop, web or mobile.

See [plan.md](plan.md) for the architecture, the CNA API coverage analysis, and the roadmap.
[plan20260727.md](plan20260727.md) is the archived previous plan, kept as the record of how the
first seven areas were built and verified.

## Status

**12 areas, 75 categories, 232 demo screens**, every one of them exercising a real
`Microsoft::Xna::Framework` / `CNA::*` API call rather than a mock.

| Area | Categories | Screens |
|---|---|---:|
| Framework | Game Loop, Game Components, Services & Dispatcher, Window, Device Manager | 17 |
| Math | Vectors, Matrix & Quaternion, Geometry, Curves, Color & Packed Vectors | 16 |
| Content | ContentManager Basics, Manifest, CNJ Format, XNB Format, Errors | 7 |
| Storage | StorageDevice, StorageContainer | 2 |
| Diagnostics | Logging, Platform & Build, Backend & Capabilities, Adapter & Display | 4 |
| Input | Keyboard, Mouse, Gamepad, Touch, Other | 52 |
| Audio | SoundEffect, SoundEffectInstance, 3D Audio, DynamicSoundEffectInstance, Microphone, XACT | 12 |
| Devices | Sensors, Vibration, Camera, System & Display, Power, Desktop Integration | 15 |
| Net | NetworkSession, NetworkGamer, GamerServices, Leaderboards | 15 |
| Media | Song, Video, MediaLibrary, Pictures | 17 |
| 2D Graphics | 4 groups, 13 categories | 38 |
| 3D Graphics | 5 groups, 16 categories | 37 |

Run `./build/cna_examples --list-demos` for the full, authoritative list.

`tools/check_catalog.py` enforces that this table, `plan.md`, the screen files on disk and the
registrations in `src/Navigation/AreaCatalog.hpp` all agree.

### Verification status

- **Verified against real hardware on the dev machine:** Keyboard, Mouse, most of Input's "Other"
  category, and Audio.
- **Verified headlessly (rendering + behaviour, under Xvfb):** the Media and Framework areas,
  via `tools/sweep.sh` and `tools/check_shots.py`. Framework additionally verifies that every
  screen restores the global state it changes: after the resolution demo changes the back buffer
  to 800x600 and leaves, the buffer is measurably back to 960x640. The Math area's on-screen
  claims are additionally asserted by `tools/checks/math_claims.cpp`, which caught three
  confidently-wrong statements before they shipped. The Content and Audio areas have the same
  treatment in `tools/checks/cnj_claims.cpp` and `tools/checks/xact_claims.cpp` — necessary
  because those screens catch their own exceptions, so a total failure still renders a clean
  screenshot and passes a sweep.
- **Self-checking screens:** the 3D area's Volume & Cube Textures screens compute their own
  pass/fail (a `GetData` round trip, an occluded-vs-visible pixel count) and draw the verdict as
  a coloured swatch, so a sweep can assert correctness by pixel rather than by "it rendered".
  That is how `RenderTargetCube::GetData` was found to return zeros silently on EASYGL.
- **Renders correctly and degrades gracefully, but never exercised with the real device:**
  Gamepad, Touch, and Input's joystick/haptics screens (no controller, touchscreen, raw joystick
  or haptic device available); Devices' mobile-only Sensors/Vibration screens; Camera
  (no webcam); MessageBox/FileDialog (need a human).
- **Backends:** `EASYGL` and `SDL_RENDERER` are both verified — 232/232 demos render on each.
  `SDL_RENDERER` is 2D-only by design, so the 3D Graphics area is gated on
  `GraphicsDevice::SupportsCapability(ThreeD)` and those demos explain themselves rather than
  throwing. See `plan.md` §4.

## Navigating the app

- **Keyboard/gamepad:** Up/Down (or D-pad/left stick) to move the selection, Enter/Space/gamepad
  A to select, Esc/gamepad B to go back.
- **Mouse/touch:** click or tap an entry to select it; click/tap the "< Back" hint at the
  bottom-left to go back.
- A category with more entries than fit on screen scrolls automatically to keep the selected
  entry visible as you navigate with Up/Down.

## Running a single demo headlessly

The app can open straight into one demo and capture it, with no window manager and no synthetic
X11 input — this is what the verification sweeps use:

```bash
./cna_examples --list-demos
./cna_examples --list-demos --search "fromstream"   # same matcher the search screen uses
./cna_examples --demo "Media/Pictures/Browse" --frames 90 --screenshot /tmp/browse.png
./cna_examples --demo "Album/Artist/Genre" --keys select,down,select --frames 120
./cna_examples --search "occlusion" --frames 70     # open search with a query pre-filled
./cna_examples --keys down,select,select --pointer 480,560,300 --frames 200   # drag gesture
```

`--demo` accepts a full `Area/Category/Demo` path or any unambiguous substring. `--keys` scripts
menu actions (`up`, `down`, `select`, `cancel`), one every `--key-interval` frames. Any run with
`--frames` ignores real input devices entirely, so a sweep cannot be perturbed by a stray event.

**Use `tools/headless.sh` rather than plain `xvfb-run`.** SDL3 picks the Wayland video driver
whenever `WAYLAND_DISPLAY` is set and then ignores the `DISPLAY` that `xvfb-run` exports — so a
command that looks headless still opens a window on your real desktop. The wrapper forces
`SDL_VIDEODRIVER=x11` and unsets `WAYLAND_DISPLAY`:

```bash
tools/headless.sh --demo "Media/Song/Visualization" --frames 150 --screenshot /tmp/vis.png
tools/sweep.sh                 # screenshot every demo
tools/sweep.sh Media           # ...or just the ones matching a filter
tools/check_shots.py build/screenshots   # flag blank/overflowing screens

# A second backend, in its own build tree
cmake -S . -B build-sdlrenderer -DCNA_GRAPHICS_BACKEND=SDL_RENDERER \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build build-sdlrenderer -j4 --target cna_examples
tools/sweep_backend.sh build-sdlrenderer
```

## Platforms

| Tier | Platforms |
|---|---|
| Now | Windows, Linux |
| Targeted | Web (Emscripten) |
| Later | Android, macOS, iPhone, consoles |

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
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build build -j4 --target cna_examples
```

Run it:

```bash
./build/cna_examples
```

Keep the `-j4` ceiling and `ccache` — see `../CLAUDE.md` for why (unbounded parallel builds have
driven this machine into swap).

The graphics backend defaults to `EASYGL`; override with
`-DCNA_GRAPHICS_BACKEND=<SDL_RENDERER|EASYGL|VULKAN|BGFX|WEBGPU>` if needed (subject to CNA's
own backend maturity — see CNA's `CLAUDE.md`).

## Project structure

```
cna-examples/
├── plan.md                        Architecture, CNA coverage analysis, roadmap
├── plan20260727.md                Archived previous plan
├── CMakeLists.txt                 Top-level build (sibling add_subdirectory of ../cna)
├── Content/                       Menu font + UI textures + demo media
│   ├── MediaDemo/                 ffmpeg-generated tones and a test video clip
│   └── MediaLibraryDemo/          A synthetic music/picture library (see tools/)
├── tools/
│   ├── gen_menu_font.py           Regenerates the menu SpriteFont from a system TTF
│   ├── gen_media_library.sh       Regenerates Content/MediaLibraryDemo/
│   ├── headless.sh                Run one demo on a virtual display
│   ├── sweep.sh                   Screenshot every demo
│   ├── sweep_backend.sh           ...from a non-default build tree (a second backend)
│   ├── check_shots.py             Flag blank or overflowing screenshots
│   ├── check_layout.py            Flag hardcoded bottom-of-window draw positions
│   ├── check_catalog.py           Screens vs registrations vs docs consistency
│   └── checks/                    Programs asserting what demos claim on screen
│       ├── math_claims.cpp        (caught three wrong statements before release)
│       └── cnj_claims.cpp         (.cnj loaders, envelope and fail-fast rules)
└── src/
    ├── Program.cpp                 Entry point + CLI
    ├── CnaExamplesGame.hpp         Game subclass; GraphicsDeviceManager + ScreenManager
    ├── Harness/                    Headless driver: option parsing, flat demo index
    ├── GameStateManagement/        Screen-stack navigation (adapted from the XNA
    │                                "Game State Management" sample)
    ├── Navigation/                 HomeScreen/AreaScreen/GroupScreen/CategoryScreen +
    │                                AreaCatalog.hpp (the Area→Group→Category→Demo registry)
    └── Demos/                      One subfolder per Area, one file per demo screen
```

Adding a demo means: write a `DemoScreen` subclass under `Demos/<Area>/<Category>/`, then
register it with `MakeDemo<YourScreen>(title, description)` in `AreaCatalog.hpp`'s
`Build<Category>Demos()` function. Run `tools/check_catalog.py` afterwards.

All bundled media is synthetic — generated by `ffmpeg` from `lavfi` sources, or built
procedurally at runtime. No third-party audio, video, image or metadata is shipped.

The one exception is *borrowed, not bundled*: the Content area's `.xnb` demos need real
MonoGame-produced files, which CNA can read but never write. Those are copied out of
`../cna/tests/assets/xnb` into the build output at build time and are **not** in version
control — they are Ms-PL, and this repository is MIT. `FontCalibri14.xnb` is excluded even
from that copy, because it embeds a rasterised Calibri glyph atlas. Build without `../cna`
and the XNB demos report the fixtures as unavailable instead of failing.

## Development

Active development happens on the `develop` branch; `master` tracks the latest stable state.

## License

MIT — see [LICENSE](LICENSE). This is original CNA-specific work, not a port of Microsoft's
XNA sample collection (compare `../cna-samples`, which is Ms-PL for that reason).
