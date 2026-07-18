# cna-examples — Plan

## 1. Purpose

`cna-examples` is a single cross-platform application that lets a user browse and run
live demonstrations of every area of the [CNA](https://github.com/openeggbert/cna) framework
(the C++ reimplementation of the XNA 4.0 programming model). It plays the same role for CNA
that [javafx-ensemble8](https://github.com/lusalome/javafx-ensemble8) plays for JavaFX: a
single browsable "sample catalog" app, not a collection of separate executables.

It is conceptually inspired by javafx-ensemble8's structure — a home page of top-level
categories, drilling down into a tree of samples, each sample runnable in place — but it is
not a JavaFX port and shares no code or assets with it. Everything here is CNA-specific and
built from scratch on top of CNA/XNA APIs.

Unlike `../cna-samples` (which ports ~90 *separate* official XNA sample executables, one
binary per sample, for structural/API-fidelity reference), `cna-examples` is **one
application** with in-app navigation. It is the user-facing "show me what CNA can do, and
prove Input/Audio/Devices/etc. actually work" catalog, including demonstrations that need a
human in the loop to validate (e.g. hardware-dependent Input checks that cannot be asserted
by an automated test suite).

## 2. Platform targets

| Tier | Platforms | Status |
|---|---|---|
| Now | Windows, Linux, Web (Emscripten), Android | Primary targets for this skeleton |
| Future | macOS, iPhone (iOS), consoles | Not addressed yet; CNA's own platform support gates this |

CNA's `include/CNA/Entrypoint.hpp` already provides the cross-platform `main()`/`SDL_main`
wiring needed for all of the above (including the Android `SDL_main` rename via
`SDL3/SDL_main.h`), so `cna-examples` needs no custom platform entry-point code — every
platform uses the same `Program.cpp`, exactly like every sample in `../cna-samples` already
does.

Because Android (and, later, iPhone) is a first-class target from day one, the navigation UI
is designed for touch first: large tap targets, vertical list menus, no reliance on a
keyboard or a mouse hover state, though both are also supported (desktop/web) via the same
`InputState`-style abstraction used below.

## 3. Graphics backend strategy

CNA selects its graphics backend at compile time via the `CNA_GRAPHICS_BACKEND` CMake option
(`SDL_RENDERER`, `EASYGL`, `VULKAN`, `BGFX`, `WEBGPU`, …). `cna-examples` follows the same
model `../cna-samples` uses: the top-level `CMakeLists.txt` defaults `CNA_GRAPHICS_BACKEND`
to `EASYGL` (CNA's strongest/most complete backend today) if the user/CI doesn't override it
on the command line.

For now, only `EASYGL` is actively targeted and verified. The app itself must not hard-code
any backend-specific assumption — it only uses public `Microsoft::Xna::Framework` /
`Microsoft::Xna::Framework::Graphics` APIs, exactly like any other XNA-style game — so
switching `CNA_GRAPHICS_BACKEND` to a different backend later (Vulkan, bgfx, WebGPU, ...) is
expected to be a CMake-flag change only, not an app rewrite. Multi-backend CI coverage is
future work, tracked alongside CNA's own backend maturity.

## 4. Navigation architecture

The navigation shell is adapted from the official XNA **Game State Management** pattern,
which is already ported to CNA and proven working at `../cna-samples/samples/GameStateManagement`
(`ScreenManager`, `GameScreen`, `MenuScreen`, `MenuEntry`, `InputState`). `cna-examples` reuses
this pattern (not the sample's own screens) as its navigation substrate:

- **`ScreenManager`** — a `DrawableGameComponent` owning a stack of `GameScreen`s; routes
  `Update`/`Draw`/input to the top of the stack, handles transition-on/off timing.
- **`GameScreen`** — base class for anything that occupies the screen (a menu, a demo, a
  popup). Provides the transition state machine (`TransitionOn` / `Active` / `TransitionOff`
  / `Hidden`) and `ExitScreen()`.
- **`MenuScreen` / `MenuEntry`** — base class for any screen that is just a vertical list of
  selectable items with a `Selected` callback; this is exactly the shape of every menu level
  `cna-examples` needs (Home, Area, Category).
- **`InputState`** — unifies keyboard, gamepad and (for this app) touch/mouse into
  `IsMenuUp` / `IsMenuDown` / `IsMenuSelect` / `IsMenuCancel`, so the same menu code drives
  desktop, gamepad and touch navigation without per-platform branches.

This is a deliberate reuse of a pattern CNA has already ported and exercised, rather than
inventing a new UI framework. `../cna-samples/samples/DynamicMenu` (a richer `Control`/
`Container`/`Transition` widget framework) was also evaluated; it is a good candidate for
*rendering individual demo UIs* later (sliders, buttons, on-screen readouts inside a demo
screen) but is heavier than what the top-level catalog navigation needs. GameStateManagement
is the better fit for the catalog shell itself.

### Screen hierarchy

```
HomeScreen (MenuScreen)
 └─ one MenuEntry per Area: Input, Audio, Devices, Net, Media, 2D Graphics, 3D Graphics, ...
      │  (selecting an Area pushes an AreaScreen)
      ▼
AreaScreen (MenuScreen)
 └─ one MenuEntry per Category within that Area
      │  (selecting a Category pushes a CategoryScreen, or directly a DemoScreen if the
      │   category has only one demo)
      ▼
CategoryScreen (MenuScreen)                      [only when an Area needs a 3rd level]
 └─ one MenuEntry per Demo within that Category
      ▼
DemoScreen (GameScreen, one concrete subclass per demo)
 └─ the actual live demonstration; Back / Cancel pops back to the CategoryScreen/AreaScreen
```

Every level above `DemoScreen` is generic and **data-driven** — a `MenuScreen` built from a
declarative list, not a hand-written screen per Area/Category. Concretely:

```cpp
struct DemoEntry {
    std::string title;
    std::string description;                 // shown while the entry is selected
    std::function<std::shared_ptr<GameScreen>()> create; // factory for the DemoScreen
};

struct CategoryEntry {
    std::string title;
    std::vector<DemoEntry> demos;             // empty for now — filled in later per area
};

struct AreaEntry {
    std::string title;
    std::vector<CategoryEntry> categories;
};
```

`HomeScreen`, `AreaScreen` and `CategoryScreen` are each a thin `MenuScreen` subclass that
builds its `MenuEntry` list from one of these structs and pushes the next level via
`ScreenManager::AddScreen` in the entry's `Selected` callback (`OnCancel` pops back, inherited
unchanged from `MenuScreen`). Adding a new Area, Category or Demo later is a data change, not
a new screen class — this is what keeps the skeleton extensible without re-architecting when
real demo content is added.

## 5. Areas (top-level menu)

The Home screen's entries mirror CNA's own XNA 4.0 namespace areas:

| Area | CNA namespace(s) | Categories (this pass) |
|---|---|---|
| Input | `Microsoft::Xna::Framework::Input`, `CNA::Internal::Input` | Keyboard, Mouse, Gamepad, Touch, Other |
| Audio | `Microsoft::Xna::Framework::Audio`, `Media.SoundEffect` | placeholder only |
| Devices | `CNA` device/platform-capability surface | placeholder only |
| Net | `Microsoft::Xna::Framework::Net`, `GamerServices` | placeholder only |
| Media | `Microsoft::Xna::Framework::Media` | placeholder only |
| 2D Graphics | `Microsoft::Xna::Framework::Graphics` (SpriteBatch, Texture2D, ...) | placeholder only |
| 3D Graphics | `Microsoft::Xna::Framework::Graphics` (Model, Effect, ...) | placeholder only |

More areas may be added later (e.g. Content pipeline, Math) — the `AreaEntry` list is just
appended to; no structural change is needed.

Only **Input** has its category list filled in for this pass, per the current task. All other
areas exist as real, selectable Home-screen entries leading to an `AreaScreen` with zero
categories (rendered as an empty/"coming soon" list) — this proves the navigation shell scales
to the full area set without committing to any area's content yet.

### 5.1 Input area detail

The Input area is the first to be fleshed out. Its category list:

- **Keyboard** — **implemented** (10 demo screens, `src/Demos/Input/Keyboard/`): live
  `IsKeyDown` state on a curated keyboard-shaped grid, `GetPressedKeys()`, an edge-detected
  press/release log, `GetModStateEXT()` modifiers/locks, `TextInputEXT` composed text input,
  scancode-vs-keycode naming (`GetScancodeNameEXT`/`GetKeyNameEXT`/their inverses),
  `KeyboardState::Equals()`/`GetHashCode()`, the 4-slot `GetState(PlayerIndex)` API,
  `IsKeyDown`/`IsKeyUp`/`operator[]`/`getItem` side by side, and a hold-duration/typematic-repeat
  pattern built on `IsKeyDown` + `GameTime`. See `src/Navigation/AreaCatalog.hpp`'s
  `BuildKeyboardDemos()` for the full registry.
- **Mouse** — **implemented** (10 demo screens, `src/Demos/Input/Mouse/`): live position and
  all five button states (`GetState()`), scroll wheel (cumulative value + derived delta,
  vertical and the `EXT` horizontal counterpart), `SetPosition()`'s write side, the
  `MouseCursor` stock-shape gallery (`EXT`), the event-driven `ClickedEXT` (vs. polling),
  relative-mouse-mode (`EXT`), desktop-global position and warp (`EXT`), capture (`EXT`),
  `MouseState::Equals()`/`GetHashCode()`/`operator==`/`operator!=`, and an edge-detected
  button press/release log. See `BuildMouseDemos()` in `AreaCatalog.hpp`.
- **Gamepad** — **implemented** (10 demo screens, `src/Demos/Input/Gamepad/`): connection +
  capabilities across all 4 `PlayerIndex` slots, a live digital-button grid, `DPad`+`ThumbSticks`
  (with a marker-in-a-box visualization for each analog stick), `Triggers` (text progress bars +
  `GamePad::TriggerThreshold`), a `None`/`IndependentAxes`/`Circular` dead-zone-mode comparison,
  `SetVibration()`/`SetTriggerVibrationEXT()`, battery/power state (`EXT`), the player-number LED
  (`GetPlayerIndexEXT`/`SetPlayerIndexEXT`, `EXT`), a combined device-info sheet (GUID/path/
  serial/firmware/Steam handle/connection medium/touchpad count/button-label, all `EXT`), and
  `GamePadState::Equals()`/`GetHashCode()`/`operator==`/`operator!=`. See `BuildGamepadDemos()`
  in `AreaCatalog.hpp`. **Not yet verified against a real controller** (none available in the
  dev/CI environment this was built in) — every screen was confirmed to render correctly and not
  crash with zero controllers connected (`IsConnected` false, `GetCapabilities()` all-false,
  string getters empty, `GetPowerInfoEXT` returning `Error`, etc.), which is the realistic
  default state, but live button/stick/trigger/vibration/LED behavior needs a hands-on pass with
  actual hardware before it can be called fully proven.
- **Touch** — `TouchPanel` state, `TouchLocation`, `GestureSample` recognition.
- **Other** — anything Input-related that doesn't fit the above four (sensors, power,
  joystick-as-distinct-from-gamepad, haptics, global mouse state, and similar `EXT`
  extensions already present in CNA's Input backend).

This area is where the `plan_input.md` Phase 11 hardware-validation tasks (the 15 tasks that
require a human with real keyboards/mice/gamepads/touchscreens attached, and could not be
automated in CI) will eventually be turned into interactive, on-screen demonstrations — e.g.
"press any key and see it echoed", "move the mouse and see the cursor/position tracked",
"connect a gamepad and see live button/stick state", "touch the screen and see gesture
recognition". That is not limited to only the Phase 11 items, though — the Input area is meant
to demonstrate the whole Input API surface, automatable or not.

## 6. Project layout

```
cna-examples/
├── .gitignore
├── LICENSE                  (MIT)
├── README.md
├── plan.md                  (this file)
├── CMakeLists.txt           (top-level; sibling add_subdirectory of ../cna)
├── cmake/
│   └── ExamplesHelpers.cmake
└── src/
    ├── Program.cpp                    (entry point: #include "CNA/Entrypoint.hpp")
    ├── CnaExamplesGame.hpp            (Game subclass; owns GraphicsDeviceManager + ScreenManager)
    ├── Navigation/
    │   ├── HomeScreen.hpp
    │   ├── AreaScreen.hpp
    │   ├── CategoryScreen.hpp
    │   └── AreaCatalog.hpp            (the AreaEntry/CategoryEntry/DemoEntry data)
    ├── GameStateManagement/           (ScreenManager/GameScreen/MenuScreen/MenuEntry/InputState,
    │                                    adapted from ../cna-samples/samples/GameStateManagement)
    └── Demos/
        ├── DemoScreen.hpp             (shared leaf-screen chrome: title, Back, DrawLines() helper)
        └── Input/
            ├── Keyboard/              (10 DemoScreen subclasses -- see section 5.1)
            ├── Mouse/                 (10 DemoScreen subclasses -- see section 5.1)
            └── Gamepad/               (10 DemoScreen subclasses -- see section 5.1)
```

`GameStateManagement/` is a local copy (adapted, not symlinked — `cna-samples` is a sibling
repo, not a dependency `cna-examples` can rely on being present) of the four small header
files listed in section 4, trimmed to only what the menu-driven catalog needs.

## 7. Build system

Follows the same convention as `../cna-samples`:

- `cmake_minimum_required(VERSION 3.20)`, C++23.
- Consumes CNA via `add_subdirectory(../cna CNA_BUILD)` (sibling-directory checkout), which
  transitively pulls in `sharp-runtime`.
- Defaults `CNA_GRAPHICS_BACKEND` to `EASYGL` unless already set on the command line.
- One executable target, `cna_examples`, linked against `CNA` and `SHARP_RUNTIME` (with the
  same `-Wl,--start-group ... -Wl,--end-group` treatment on GCC/Clang Linux for the
  circular CNA/backend reference, and the same Emscripten/`WIN32_EXECUTABLE` special-casing
  `cna-samples/cmake/SampleHelpers.cmake` already uses — reused as `cmake/ExamplesHelpers.cmake`
  rather than re-derived from scratch).
- A `Content/` directory (fonts/textures the navigation shell itself needs, e.g. a menu font)
  is copied next to the built executable, same post-build pattern as `cna-samples`.

## 8. Scope of this pass

This document and the accompanying skeleton are an **architecture pass only**, per explicit
instruction. In scope now:

- This `plan.md`.
- `.gitignore`, `LICENSE` (MIT), `README.md`.
- The CMake project skeleton and `Program.cpp` entry point.
- The generic, data-driven navigation shell (`ScreenManager` + `HomeScreen` + `AreaScreen` +
  `CategoryScreen`), wired to a real `AreaCatalog` listing all seven areas from section 5,
  with Input's five categories present as empty (zero-demo) `CategoryEntry` placeholders.
- Enough of `GameStateManagement/` (adapted from `cna-samples`) to make the above compile and
  run: shows the Home menu, navigates into an Area, navigates into a Category, shows "no demos
  yet" and lets the user back out — proving the architecture end-to-end without real demo
  content.
- **Since extended beyond the initial pass:** the shared `DemoScreen` base (title/Back chrome,
  `DrawLines()` helper) plus all 10 Keyboard, all 10 Mouse, and all 10 Gamepad demo screens (see
  section 5.1), wired into `AreaCatalog.hpp`. `MenuScreen` also gained auto-scroll-to-selection
  (`scrollOffset_`) once Keyboard's 10-demo + Back list overflowed a single screen — any
  category with enough demos to overflow the viewport now scrolls correctly, not just Keyboard.

Explicitly **out of scope** still (future work):

- Touch/Other demo screens for Input, and anything for Audio/Devices/Net/Media/2D/3D.
- A hands-on verification pass for the Gamepad demos against real controller hardware (see
  section 5.1's Gamepad note) — no controller was available while building them.
- The Phase 11 hardware-validation demonstrations that need a human with a real touchscreen
  attached (Keyboard's, Mouse's, and Gamepad's demos above cover keyboard/mouse/controller
  already, modulo the Gamepad hands-on verification note above).
- Search (`javafx-ensemble8`'s `SearchPopover` equivalent).
- Multi-backend CI, non-EasyGL verification.
- macOS/iOS/console targets.
- Touch swipe/drag-to-scroll for browsing a long menu beyond the current selection (today's
  auto-scroll only follows keyboard/gamepad Up/Down; a touch-only user can't yet reach an entry
  further down an overflowing list than what's already on screen without a physical/virtual
  D-pad).

## 9. Relationship to existing repos

- **`../cna`** — the framework being demonstrated; consumed as a sibling `add_subdirectory`,
  never vendored/copied. Builds against whatever branch `../cna` has checked out (`develop` by
  default) -- note that as of this writing the `feature/input` branch's Input-subsystem audit
  fixes/hardening have not yet been merged into `develop`, so a Keyboard demo here reflects
  `develop`'s current Input behavior, not `feature/input`'s. No cna-examples-side change is
  needed once that branch merges; it will pick up the fixes on the next `../cna` update.
- **`../sharp-runtime`** — pulled in transitively through `../cna`.
- **`../cna-samples`** — reference/inspiration only for CMake structure
  (`cmake/SampleHelpers.cmake` → `cmake/ExamplesHelpers.cmake`) and for the
  `GameStateManagement`/`DynamicMenu` sample code adapted into the navigation shell. No build
  or runtime dependency on `cna-samples` — `cna-examples` is fully self-contained once the
  small amount of adapted code is copied in.
