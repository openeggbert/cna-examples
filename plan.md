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
 └─ one MenuEntry per Group within that Area (if any), OR one MenuEntry per Category
      │  (an Area populates exactly one of `groups`/`categories` — see below)
      ▼
GroupScreen (MenuScreen)                         [only when an Area needs a Group level]
 └─ one MenuEntry per Category within that Group
      │  (selecting a Category pushes a CategoryScreen, or directly a DemoScreen if the
      │   category has only one demo)
      ▼
CategoryScreen (MenuScreen)                      [only when an Area/Group needs one more level]
 └─ one MenuEntry per Demo within that Category
      ▼
DemoScreen (GameScreen, one concrete subclass per demo)
 └─ the actual live demonstration; Back / Cancel pops back to the CategoryScreen/GroupScreen/AreaScreen
```

Every level above `DemoScreen` is generic and **data-driven** — a `MenuScreen` built from a
declarative list, not a hand-written screen per Area/Group/Category. Concretely:

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

struct GroupEntry {
    std::string title;
    std::vector<CategoryEntry> categories;
};

struct AreaEntry {
    std::string title;
    std::vector<CategoryEntry> categories;    // populated when the Area needs no Group level
    std::vector<GroupEntry> groups;            // populated when the Area needs one (see below)
};
```

`HomeScreen`, `AreaScreen`, `GroupScreen` and `CategoryScreen` are each a thin `MenuScreen`
subclass that builds its `MenuEntry` list from one of these structs and pushes the next level
via `ScreenManager::AddScreen` in the entry's `Selected` callback (`OnCancel` pops back,
inherited unchanged from `MenuScreen`). Adding a new Area, Group, Category or Demo later is a
data change, not a new screen class — this is what keeps the skeleton extensible without
re-architecting when real demo content is added.

**The Group level** was added when the 2D Graphics area's demo count grew large enough (13
categories) that a flat Area→Category→Demo list would have made the Area screen itself an
unwieldy 13+-entry menu. `AreaEntry` gained an optional `groups` field: an Area populates
*either* `categories` directly (every Area through Media — Input, Audio, Devices, Net, Media)
*or* `groups` (2D Graphics, so far the only one) but never both. `AreaScreen`'s constructor
checks `groups` first and builds `MenuEntry`s that navigate to a `GroupScreen` if non-empty,
falling back to the original `categories`-direct behavior otherwise (and to a "(coming soon)"
placeholder only when both are empty, e.g. 3D Graphics today). This is a fully backward-
compatible generalization — every existing Area's navigation is unchanged, verified via Xvfb
that Media (still `categories`-only) and 2D Graphics (`groups`-only) both navigate correctly
side by side. `GroupScreen.hpp` is structurally identical to `AreaScreen.hpp`/
`CategoryScreen.hpp` (a `MenuScreen` over a declarative list, pushing the next level on
selection) — no new navigation concept was invented, just one more optional link in the same
chain. Any future Area with a large, naturally-grouped category set can reuse `groups` the same
way; Areas with a handful of categories should keep using `categories` directly rather than
introducing a Group level for its own sake.

## 5. Areas (top-level menu)

The Home screen's entries mirror CNA's own XNA 4.0 namespace areas:

| Area | CNA namespace(s) | Categories (this pass) |
|---|---|---|
| Input | `Microsoft::Xna::Framework::Input`, `CNA::Internal::Input` | Keyboard, Mouse, Gamepad, Touch, Other |
| Audio | `Microsoft::Xna::Framework::Audio` | SoundEffect, SoundEffectInstance, 3D Audio, DynamicSoundEffectInstance, Microphone |
| Devices | `Microsoft::Devices::Sensors`, `Microsoft::Devices::VibrateController`, `CNA::Devices` | Sensors, Vibration, Camera, System & Display, Power, Desktop Integration |
| Net | `Microsoft::Xna::Framework::Net`, `Microsoft::Xna::Framework::GamerServices` | NetworkSession, NetworkGamer, GamerServices, Leaderboards |
| Media | `Microsoft::Xna::Framework::Media` | Song, Video, MediaLibrary |
| 2D Graphics | `Microsoft::Xna::Framework::Graphics` (SpriteBatch, Texture2D, SpriteFont, ...) | 4 Groups, 13 categories — see 5.6 |
| 3D Graphics | `Microsoft::Xna::Framework::Graphics` (Model, Effect, ...) | placeholder only |

More areas may be added later (e.g. Content pipeline, Math) — the `AreaEntry` list is just
appended to; no structural change is needed.

**Input**, **Audio**, **Devices**, **Net**, **Media**, and **2D Graphics** have their category
lists filled in as of this pass. The remaining area (3D Graphics) exists as a real, selectable
Home-screen entry leading to an `AreaScreen` with zero categories and zero groups (rendered as
an empty/"coming soon" list) — this proves the navigation shell scales to the full area set
without committing to any area's content yet.

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
- **Touch** — **implemented** (10 demo screens, `src/Demos/Input/Touch/`): raw per-finger state
  from `GetState()` (up to `MAX_TOUCHES`, with position markers), `GetCapabilities()` + display
  geometry, the discrete gestures (Tap/DoubleTap, Hold/Flick) and continuous ones (Drag family,
  Pinch's two-finger `Position2`/`Delta2`), an "every `GestureType` at once" log, the
  `IsGestureAvailable`/`ReadGesture()` drain-the-queue contract made explicit, `TouchLocation`
  `Equals()`/`GetHashCode()`/`ToString()`, and `TouchDeviceExistsEXT` + a derived peak-
  simultaneous-touches metric. Each gesture-recognizing screen saves/restores
  `TouchPanel::EnabledGestures` in `LoadContent()`/`UnloadContent()` since it is process-wide
  state, not per-screen. See `BuildTouchDemos()` in `AreaCatalog.hpp`. **Not verified against
  real touch/gesture input** — no touchscreen was available in the dev/CI environment this was
  built in, and synthetic X11 mouse events (the only input this environment can inject) do not
  reliably become SDL touch events, unlike keyboard/mouse which do have working synthetic-input
  paths. Every screen was confirmed to render correctly and not crash with zero active touches
  (the actual default state reached), including the gesture-enable/restore lifecycle
  (`LoadContent`/`UnloadContent`) not crashing on entry/exit — but live gesture recognition
  itself needs a hands-on pass on a real touchscreen or Android device.
- **Other** — **implemented** (10 demo screens, `src/Demos/Input/Other/`): everything
  Input-adjacent that doesn't fit the four device categories above, all `NOXNA`/`EXT`
  extensions with no XNA 4.0 precedent. Raw joysticks (`CNA::Input::Joysticks` —
  enumeration+capabilities, live unmapped axes/buttons/hats/trackballs, `JoystickStateEXT`'s
  `operator==`/`!=`, and `ConnectedEXT`/`DisconnectedEXT` hot-plug events), host device motion
  sensors (`CNA::Input::Sensors`, distinct from a gamepad's own gyro/accelerometer and from the
  official `Microsoft::Devices::Sensors` XNA namespace, which belongs to the separate Devices
  Area), host battery/power (`CNA::Input::Power` — XNA has no power API at all), force-feedback
  haptics (`CNA::Input::Haptics` — richer than `GamePad::SetVibration`'s simple rumble),
  mouse/keyboard/touch device enumeration and hot-plug events (`CNA::Input::InputDevices`), and
  a one-screen "Summary" dashboard tying every subsystem above together. See
  `BuildOtherDemos()` in `AreaCatalog.hpp`. **Verified against real host hardware**, unlike the
  Gamepad/Touch caveats above — `CNA::Input::Power`/`InputDevices` read genuine data from the
  dev machine this was built on (actual battery charge %, real enumerated mice/keyboards), since
  no external peripheral was needed to exercise those APIs; only the joystick/haptics screens'
  "device connected" paths remain unverified for the same no-hardware-available reason as
  Gamepad.

This closes out every category in Input (Keyboard/Mouse/Gamepad/Touch/Other all have real demo
screens now). This area is where the `plan_input.md` Phase 11 hardware-validation tasks (the 15 tasks that
require a human with real keyboards/mice/gamepads/touchscreens attached, and could not be
automated in CI) will eventually be turned into interactive, on-screen demonstrations — e.g.
"press any key and see it echoed", "move the mouse and see the cursor/position tracked",
"connect a gamepad and see live button/stick state", "touch the screen and see gesture
recognition". That is not limited to only the Phase 11 items, though — the Input area is meant
to demonstrate the whole Input API surface, automatable or not.

### 5.2 Audio area detail

Unlike Input, `Microsoft::Xna::Framework::Audio`'s own category boundaries aren't dictated by
anything in XNA itself, so this app's five categories are a judgment call sized to the real API
surface (smaller than Input's — 12 demo screens total, not padded to a fixed count per
category):

- **SoundEffect** — **implemented** (2 demo screens, `src/Demos/Audio/SoundEffect/`): the
  simplest one-shot path, `Play()`/`Play(volume, pitch, pan)`, built on a procedurally generated
  sine-wave PCM buffer (this app ships no WAV asset — see `AudioDemoHelpers.hpp`'s
  `GenerateSineWavePcm16()`), plus the static, process-wide settings (`MasterVolume`/
  `DistanceScale`/`DopplerScale`/`SpeedOfSound`) and the byte-count/duration math helpers
  (`GetSampleDuration`/`GetSampleSizeInBytes`).
- **SoundEffectInstance** — **implemented** (3 demo screens): the long-lived, controllable
  counterpart to `SoundEffect::Play()` — transport controls (`Play`/`Pause`/`Resume`/`Stop` +
  `State`), live-adjustable `Volume`/`Pitch`/`Pan` on a continuously looping tone, and
  `IsLooped`.
- **3D Audio** — **implemented** (2 demo screens): `SoundEffectInstance::Apply3D(listener,
  emitter)` with a movable `AudioEmitter` (position-driven Volume/Pan), and a second screen
  isolating the Doppler effect specifically (`AudioEmitter::Velocity` + `SoundEffect::
  DopplerScale`/`SpeedOfSound` → a real closed-form pitch shift, not a native mixer feature).
- **DynamicSoundEffectInstance** — **implemented** (1 demo screen): real-time audio synthesis —
  the app supplies short PCM chunks on demand via the `BufferNeeded` event and `SubmitBuffer()`,
  with a live-retunable frequency, rather than playing one fixed pre-generated buffer.
- **Microphone** — **implemented** (2 demo screens): device enumeration
  (`Microphone::All`/`Default` + per-device properties) and capture control
  (`Start`/`Stop`/`GetData()` + the `BufferReady` event).

See `BuildSoundEffectDemos()`/`BuildSoundEffectInstanceDemos()`/`BuildAudio3DDemos()`/
`BuildDynamicSoundEffectInstanceDemos()`/`BuildMicrophoneDemos()` in `AreaCatalog.hpp`.

**Verified against real audio hardware**, not just the "gracefully handle absence" bar Gamepad/
Touch were held to: on the dev machine this was built on, `SoundEffect::Play()` actually
returned `true` and played the generated tone, `SoundEffectInstance`'s Stopped→Playing state
transition was observed live, `DynamicSoundEffectInstance`'s `BufferNeeded` event fired
correctly on the real-time audio thread (26 chunks submitted and consumed with no crash or
threading issue over a short session), and `Microphone::All` enumerated the machine's two real
capture devices ("Ryzen HD Audio Controller Digital/Stereo Microphone") with `Start()` +
`GetData()` reading genuine captured bytes (139264 bytes in one short test). `NoAudioHardwareException`
is caught and shown honestly wherever construction/playback could throw it, for environments
that truly have no audio device at all.

Deliberately **not** covered: the XACT project-based audio system (`AudioEngine`/`SoundBank`/
`WaveBank`/`Cue`/`AudioCategory`). Demonstrating it honestly would need real compiled XACT
project binaries (`.xgs`/`.xsb`/`.xwb`), which require Microsoft's XACT authoring tool to
produce and this app cannot generate or fake — unlike `SoundEffect`, which has a raw-PCM-buffer
constructor that sidesteps needing any asset file at all.

### 5.3 Devices area detail

Devices is the first area that mixes two distinct API surfaces: the official (but WP7-only,
never-ported-to-desktop-XNA) `Microsoft::Devices::Sensors`/`VibrateController` namespace, and
`CNA::Devices`, a CNA-original extension namespace with no XNA/WP7 precedent at all (gated
behind the `CNA_DEVICES` CMake option, `OFF` by default in `CNA` itself since it's an
opt-in extension surface — this app's top-level `CMakeLists.txt` forces it `ON` before
`add_subdirectory(../cna CNA_BUILD)`). 15 demo screens across 6 categories:

- **Sensors** — **implemented** (4 demo screens, `src/Demos/Devices/Sensors/`): Accelerometer,
  Gyroscope, Compass, and Motion (the fused sensor), all built on the shared
  `Microsoft::Devices::Sensors::SensorBase<T>` pattern (`getIsSupportedProperty()`, a
  constructor that may throw a sensor-specific `...FailedException`, `Start()`/`Stop()`,
  `getStateProperty()` over the 6-value `SensorState` enum, `getCurrentValueProperty()`,
  `getIsDataValidProperty()`, `getTimeBetweenUpdatesProperty()`, and the `CurrentValueChanged`
  event); Compass additionally demonstrates the `Calibrate` event. **Not verified against real
  sensor hardware** — this is a WP7-era mobile sensor API with no desktop backend; every screen
  was confirmed to render and navigate correctly with `IsSupported: false` (the honest, correct
  desktop answer) and no crash. Per explicit instruction, these are meant to be tested on
  Android, not desktop.
- **Vibration** — **implemented** (1 demo screen): `Microsoft::Devices::VibrateController`
  (`EXT`-suffixed members reflect this being a CNA cross-platform extension of a WP7-only API) —
  `IsSupportedEXT`, `DeviceNameEXT`, `Start(duration)`/`StartLeftRightEXT(left, right,
  duration)`. **Not verified against real vibration hardware** — confirmed to render correctly
  and silently no-op (`IsSupportedEXT: false`) on this desktop machine, consistent with
  `VibrateController` being meaningful mainly on a phone/gamepad-with-rumble; try on Android.
- **Camera** — **implemented** (1 demo screen): `CNA::Devices::Camera`'s live-capture path —
  enumerates available capture devices, opens the first one, lazily allocates a `Texture2D`
  once the real frame size is known, and calls `TryAcquireFrame()` every `OnDemoUpdate` to draw
  a live feed via `SpriteBatch::Draw()`. **Verified on real desktop hardware, not just graceful
  absence**: `Camera::getIsSupportedProperty()` is genuinely `true` on this Linux dev machine
  (the backend itself works), and with 0 physical cameras attached it correctly reports 0
  available devices and shows a "no camera opened" message with no crash — the actual
  live-frame-draw path is implemented and will engage on any machine with a webcam, but wasn't
  exercised with a physical feed here.
- **System & Display** — **implemented** (3 demo screens): `CNA::Devices::SystemInfo` (logical
  CPU core count, system RAM), `CNA::Devices::DisplayInfo` (content scale, safe-area rectangle
  in window client coordinates), and `CNA::Devices::Locale` (the ordered list of preferred
  locales). **Verified against real desktop data** — all three screens showed genuine values
  from the dev machine (16 logical cores, ~30 GB RAM, a 960x640 safe area matching the actual
  window, and `en-US`/`en-US`/`en` as the real preferred-locale list) rather than placeholders.
  Safe-area insets are most interesting on a real phone with a notch/curved edge, which this
  desktop obviously doesn't have.
- **Power** — **implemented** (1 demo screen): `CNA::Devices::PowerInfo` — `PowerState` (the
  6-value enum: `Error`/`Unknown`/`OnBattery`/`NoBattery`/`Charging`/`Charged`), battery
  percentage, and seconds-remaining estimate. **Verified against real hardware** — showed the
  dev laptop's genuine state (`Charged`, 78%), consistent with the equivalent reading already
  seen from `CNA::Input::Power` in the Input area's Other category.
- **Desktop Integration** — **implemented** (5 demo screens): `CNA::Devices::Clipboard`
  (`getHasTextProperty`/`getTextProperty`/`setTextProperty`), `CNA::Devices::MessageBox` (native
  blocking dialog — `ShowSimple()` and `Show()` with Yes/No), `CNA::Devices::FileDialog` (native
  async open-file/open-folder dialogs via a result callback, backed by a real dialog helper
  process such as `zenity` on Linux), `CNA::Devices::UrlLauncher` (`Open()` hands a URL to the
  system's default handler), and `CNA::Devices::SystemTray` (a real notification-area icon with
  a checkable menu entry, created on `LoadContent()` and destroyed on `UnloadContent()`).
  **Clipboard, UrlLauncher, and SystemTray verified against real desktop behavior**: a real
  round-trip write+read against the actual X11 clipboard was performed (`HasText` went
  false→true, `setTextProperty()` returned `true`), and SystemTray's real tray-icon
  creation/entry-registration path ran with no crash (its `AddEntry` click callback is
  `std::function<void()>`, not index-parameterized — corrected during implementation).
  **MessageBox and FileDialog were deliberately not triggered** during this app's own automated
  verification pass: `MessageBox::Show`/`ShowSimple` are genuinely thread-blocking (real SDL3
  behavior, and the API's own doc comment says an automated test must never call it directly),
  and `FileDialog`, though asynchronous at the call level, still launches a real native dialog
  process that waits indefinitely for a human — both screens were confirmed to render and
  navigate correctly (including showing `IsSupported: true` for both) without pressing their
  trigger inputs; try them interactively.

See `BuildSensorsDemos()`/`BuildVibrationDemos()`/`BuildCameraDemos()`/
`BuildSystemAndDisplayDemos()`/`BuildPowerDemos()`/`BuildDesktopIntegrationDemos()` in
`AreaCatalog.hpp`.

### 5.4 Net area detail

Net is the first area whose `.cpp` implementation lives in two separate static-library
targets (`CNA_Net`/`CNA_GamerServices`) rather than the umbrella `CNA` target — this app's
`cmake/ExamplesHelpers.cmake` needed a new `if(TARGET CNA_Net) target_link_libraries(...
CNA_Net)` line (the same fix `../cna-samples` already carries for its `ClientServerSample`)
before any Net/GamerServices symbol would link. `CnaExamplesGame.hpp` also now registers a
`GamerServicesComponent` in `Game::Components` at app startup (not per-screen) — required
before any `Microsoft::Xna::Framework::Net`/`GamerServices` call, and what populates the 4
stub `SignedInGamer`s every screen below reads. 14 demo screens across 4 categories:

- **NetworkSession** — **implemented** (5 demo screens, `src/Demos/Net/NetworkSession/`):
  `Local Lifecycle` (`StartGame`/`EndGame`/`RemoveGamer` driving real `NetworkSessionState`
  Lobby→Playing→Ended transitions, verified end-to-end including the correct `SessionEnded`
  reason and `HasLeftSession` flip); `SystemLink Host` (a genuinely real, UDP-socket-backed
  `NetworkSessionType::SystemLink` session — see the hardware-verification note below);
  `Discover & Join` (`NetworkSession::Find()` — real blocking (~150 ms) LAN/loopback UDP
  discovery, not a stub); `Packet Round-Trip` (`PacketWriter`/`PacketReader` over every XNA
  math type, including a deliberate demonstration of the real, documented upstream FNA quirk
  that `Write(Color)` writes 4 bytes while `ReadColor()` always reads 4 floats — this screen
  triggers it live and shows the resulting thrown exception, not just a comment); `Properties
  & Events` (`NetworkSessionProperties`' `int?` slots + the `GamerJoined`/`GamerLeft`/
  `HostChanged` event trio, including the documented "call `Update()` once right after
  subscribing to receive the replayed initial-join event(s)" pattern).
- **NetworkGamer** — **implemented** (2 demo screens): `Roster` (`AllGamers`/`LocalGamers`/
  `RemoteGamers`/`PreviousGamers` + per-gamer `Id`/`IsHost`/`IsLocal`/`RoundtripTime`) and
  `NetworkMachine` (`Gamer::Machine` + `RemoveFromSession()`, which always throws
  `NotImplementedException`, matching FNA's own stub — verified live, not just asserted).
- **GamerServices** — **implemented** (5 demo screens): `Signed-In Gamers` (`Gamer::
  SignedInGamers` + the real, publicly-documented but never-raised `SignedInGamer::SignedIn`/
  `SignedOut` static events), `Profile/Presence/Privileges` (`GetProfile()` + a live-mutable
  `GamerPresence` + read-only `GamerPrivileges`), `Achievements` (`AwardAchievement`/
  `GetAchievements` — both real, functional, no-persistence-backend stubs — plus
  `Achievement::GetPicture()`, which always throws `NotImplementedException`), `Friends &
  GamerCard` (`GetFriends()`, always genuinely empty, alongside a manually-built demo roster
  via `FriendGamer::CreateInternal` since that's the only way to see `FriendGamer`'s
  `IsOnline`/`IsPlaying`/`IsAway` fields populated on this platform), and `Guide Overlay` (the
  static `Guide` surface's three-way split personality: `BeginShowKeyboardInput`/
  `EndShowKeyboardInput` complete synchronously and never throw but always return `""`;
  `BeginShowMessageBox` always throws `NotSupportedException` outright; every `Show*` UI
  launcher is a real, silent no-op).
- **Leaderboards** — **implemented** (2 demo screens): `LeaderboardReader` (`LeaderboardIdentity`
  as a real, inspectable value type, alongside `Read()`/`BeginRead()`, which unconditionally
  throw `NotSupportedException` — leaderboards were never implemented upstream in FNA either,
  a stub API surface, not a CNA gap) and `LeaderboardWriter` (`LeaderboardEntry`'s
  `Columns`/`Gamer`/`Rating`/`RankingEXT` as a real, working value type, alongside
  `Gamer::LeaderboardWriter.GetLeaderboard()`, which always throws the same way).

See `BuildNetworkSessionDemos()`/`BuildNetworkGamerDemos()`/`BuildGamerServicesDemos()`/
`BuildLeaderboardsDemos()` in `AreaCatalog.hpp`.

**Verified against real hardware/transport, not just graceful-absence handling**: the
`SystemLink Host` screen's `NetworkSession::Create(SystemLink, ...)` genuinely binds a real
ENet UDP socket (confirmed via `AllGamers`/`Host` reflecting real state, and via
`Discover & Join`'s `Find()` performing real blocking UDP discovery I/O that correctly
returns zero results when no other host is reachable — the realistic single-machine desktop
default, the same verification bar already established for Gamepad/Touch/Sensors in earlier
areas). A genuine two-process host+discover+join round trip was **not** completed in this
session: two `cna_examples` instances launched side-by-side under the same unmanaged Xvfb
display (no window manager) rendered at the identical default screen position with no way
found to route synthetic keyboard input to one specific window over the other — a test-harness
limitation of this verification environment, not a code path left unverified by design (unlike
Gamepad/Touch/Sensors' "no hardware available" caveats). The single-process paths that make up
both screens (real session creation, real bound socket, real discovery query/reply I/O) are
fully verified; the actual cross-process join is not, and is flagged here rather than silently
assumed to work.

**A real, verified upstream FNA quirk surfaced during verification, not a bug in this app**:
`NetworkSession::Create()`'s `maxGamers` argument is accepted (and used only for a
`privateGamerSlots > maxGamers` range check) but never actually stored — `EndCreate()`
unconditionally hardcodes `MaxGamers` to `69` regardless of what any `Create()` overload was
asked for, confirmed by reading `NetworkSession.cpp`'s own `// FNA hardcodes 69 here...`
comment. The `SystemLink Host` screen displays this openly (`MaxGamers: 69 (always 69 -- real
upstream FNA quirk, not a bug)`) rather than hiding it. A second, related consequence
discovered the same way: `GamerServicesDispatcher::Initialize()`'s 4 stub `SignedInGamer`s are
`IsGuest=false` only for `SignedInGamers[0]` — the other 3 are all guests — and
`NetworkSession`'s implicit-local-gamer constructor path skips guests, so requesting
`maxLocalGamers=2` (as `Roster`/`Properties & Events` both do) genuinely yields only 1 local
gamer in practice; both screens' doc comments and on-screen `LocalGamers`/`AllGamers` counts
reflect the real, measured value rather than the requested one.

Deliberately **not** covered: Xbox Live/Games-for-Windows-Live-era session types
(`PlayerMatch`/`Ranked`) beyond their real, honest stub behavior (no synthetic offline
approximation exists for them, matching `plan_net.md`'s own documented scope decision), and the
Avatar sub-namespace (`AvatarRenderer`/`AvatarDescription`/...) — a large, separate API surface
already covered by 8 existing demo executables in `../cna/examples/` and cross-cutting enough
(spanning both `GamerServices` and `Graphics::SkinnedModelEXT`) to warrant its own pass later
rather than folding into this Area's four categories.

### 5.5 Media area detail

Media is built against `develop`'s actual implementation, not `feature/media`'s much larger,
still-unmerged `plan_media.md` rewrite (Phases 0–16, `MEDIA-1..215`) — that branch adds real
`Song.Album/Artist/Genre`, a working `MediaLibrary`, and real visualization data, none of which
exist on `develop` yet. This app's own demo screens and their "hard stub, not gracefully empty"
framing describe `develop` as it stands today; revisit this section if/when `feature/media`
merges. 11 demo screens across 3 categories, all built on fully synthetic content generated by
`ffmpeg` from `lavfi` sources (`sine`/`testsrc`) — no third-party audio or video asset is
bundled with this app:

```bash
ffmpeg -f lavfi -i "sine=frequency=440:duration=3" -ar 44100 -ac 1 -c:a pcm_s16le tone_a.wav
ffmpeg -f lavfi -i "sine=frequency=554:duration=3" -ar 44100 -ac 1 -c:a pcm_s16le tone_b.wav
ffmpeg -f lavfi -i "sine=frequency=659:duration=3" -ar 44100 -ac 1 -c:a pcm_s16le tone_c.wav
ffmpeg -f lavfi -i "sine=frequency=440:duration=2" -c:a libopus unsupported.opus
ffmpeg -f lavfi -i "testsrc=size=320x240:rate=24:duration=4" -f lavfi -i "sine=frequency=330:duration=4" \
  -c:v libx264 -pix_fmt yuv420p -c:a aac -shortest test_clip.mp4
```

- **Song** — **implemented** (6 demo screens, `src/Demos/Media/Song/`): `Load & Play` (`Song`'s
  NOXNA direct-from-file constructor — real XNA's `Song` has no public constructor at all, only
  reachable via `MediaLibrary`, which is a hard stub here — + `MediaPlayer`'s transport
  controls), `Volume/Mute/Repeat/Shuffle` (live-adjustable playback settings, reset to their
  defaults on exit since `MediaPlayer` is a static/global singleton — same discipline as
  `TouchPanel::EnabledGestures` in the Input area), `Queue Navigation`
  (`Play(SongCollection, index)` + `MoveNext()`/`MovePrevious()` over 3 differently-pitched
  tones), `MediaPlayer Events` (`ActiveSongChanged`/`MediaStateChanged` — real events, deferred
  one frame through `FrameworkDispatcher`, which `Game::Update()` already pumps automatically
  every frame with no manual wiring needed), `Unsupported Format` (a real, validly-encoded
  `.opus` file — `Song` construction succeeds since it only checks the file exists, but
  `MediaPlayer::Play()` silently stays `Stopped`: CNA's vendored SDL3_mixer has Opus/WavPack
  decoding fully compiled out, with no built-in fallback decoder the way MP3/OGG/FLAC have via
  bundled `dr_mp3`/`stb_vorbis`/`dr_flac`), and `Visualization`
  (`GetVisualizationData()`/`IsVisualizationEnabled` — a confirmed, permanent stub; the setter
  is a real no-op and the buffer is never written to, verified live by summing both arrays every
  frame during real playback and confirming the sum stays exactly zero).
- **Video** — **implemented** (3 demo screens, `src/Demos/Media/Video/`): `Load & Play` (a real
  FFmpeg-decoded clip — `Video`'s constructor probes real `Width`/`Height`/`FramesPerSecond`/
  `Duration`, and `VideoPlayer::GetTexture()` decodes real frames on demand into a live
  `Texture2D` drawn every frame), `Playback Control` (`Play`/`Pause`/`Resume`/`Stop` +
  `IsLooped` + live `Volume`/`IsMuted`), and `Multi-Track (EXT)`
  (`SetAudioTrackEXT`/`SetVideoTrackEXT` — CNA extensions with no stock XNA equivalent; this
  app's own test clip only has 1 video + 1 audio stream, so selecting track 0 is a real,
  no-throw call with no visible change, honestly noted as such rather than staged to look more
  dramatic).
- **MediaLibrary** — **implemented** (2 demo screens): `Catalog Access`
  (`MediaLibrary`'s default constructor succeeds — the only member that doesn't — while every
  accessor, `Songs`/`Albums`/`Artists`/`Genres`/`Playlists`, and even `Dispose()` itself, throws
  `std::runtime_error("not implemented")`; `MediaSource::GetAvailableMediaSources()` is real and
  never throws, just always empty) and `Album/Artist/Genre/Playlist`
  (a narrated screen: `Album`/`Artist`/`Genre`/`Playlist`/`Picture` all have private
  constructors and `MediaLibrary` — their only possible producer — throws on every accessor
  that could ever hand one back, so there is no live instance of any of these types reachable on
  this platform at all; the screen's one live, callable demonstration is
  `MediaLibrary(nullptr)`, which really does throw immediately, plus the `MediaSourceType` enum
  values as the one piece of real, inspectable data in this whole stub cluster).

See `BuildSongDemos()`/`BuildVideoDemos()`/`BuildMediaLibraryDemos()` in `AreaCatalog.hpp`.

**Genuinely verified against real decode, not just graceful-absence handling** (same bar as
Audio/Net's SystemLink transport): `Load & Play`'s tone actually played with `PlayPosition`
advancing and `Duration` auto-populating to the real encoded length; `Video`'s `Load & Play`
screen showed the real ffmpeg `testsrc` color-bar pattern animating frame-to-frame (confirmed
via two screenshots a second apart showing the gradient bar and clock digit both having moved),
correctly reached `MediaState::Stopped` at the clip's natural end, and `Playback Control`'s
`IsLooped` was confirmed by resuming and waiting past the 4-second clip duration without the
state ever leaving `Playing`. One genuine implementation detail found while verifying
`MediaLibraryScreen`, not initially anticipated: `MediaLibrary::Dispose()` throws exactly like
every other accessor (confirmed by reading `MediaLibrary.cpp`) — this screen's own
`UnloadContent()` deliberately does **not** call it (since screen teardown must never throw),
noted directly in the source rather than silently worked around.

Deliberately **not** covered: `Picture`/`PictureAlbum`(+collections) beyond the same narrated
"unreachable" treatment `Album`/`Artist`/`Genre`/`Playlist` already get (a WP7-era
`MediaLibrary` sub-surface with no additional live behavior to demonstrate over what
`Album`/`Artist` already cover), and `feature/media`'s in-progress rewrite (see this section's
opening note) — this Area describes `develop`'s current, real behavior only.

### 5.6 2D Graphics area detail

2D Graphics is the first Area to use the Group navigation level (section 4): 4 Groups, 13
categories, 38 demo screens total, all built against `develop`'s EasyGL backend (this app's
default). Scope was deliberately fixed up front via two decisions: a Group level rather than a
flat 13-category list, and raw vertex/primitive drawing (`VertexPositionColor`,
`GraphicsDevice::DrawUserPrimitives`, `PrimitiveType`) left entirely for a future 3D Graphics
area — this Area covers only `SpriteBatch`/`Texture2D`/`SpriteFont`/2D-relevant
`GraphicsDevice` state. Every texture used across all 38 screens is generated procedurally at
runtime via `TextureDemoHelpers.hpp` (`CreateCheckerboardTexture`/`CreateGradientTexture`/
`CreateGridTexture`, all built on `Texture2D::CreateFromPixels`) — no PNG/image asset is
bundled, the same "no third-party asset" discipline as Audio's `GenerateSineWavePcm16()` and
Media's ffmpeg-generated clips.

**A recurring implementation pattern worth calling out**: `DemoScreen::Draw()` wraps every
`OnDemoDraw()` call in a default-parameter `sb.Begin()`/`sb.End()` pair (for the title and Back
hint). Since `SpriteBatch::Begin()` throws if called while already begun, any demo needing a
custom `SpriteSortMode`/`BlendState`/`SamplerState`/`RasterizerState`/transform-`Matrix`, or
that switches the active render target, must inside its own `OnDemoDraw`: call `sb.End()`
first (flushing whatever the base class already queued into the still-open default batch —
critically, this must happen **before** any direct `GraphicsDevice` state mutation, e.g.
`ScissorRectangle` or `SetRenderTarget`, since `SpriteBatch` with the default `Deferred` sort
mode only rasterizes queued sprites at `End()` time — changing device state first retroactively
corrupts anything already queued but not yet flushed), then `sb.Begin(customParams...)`, do the
custom draws, `sb.End()` again, restore default state, then `sb.Begin()` once more before
returning so the base class's trailing Back-hint text has a valid open batch. This dance is
demonstrated at length in `SortModes/DeferredSortScreen.hpp`'s code comment and reused across
roughly a third of the screens in this Area.

- **SpriteBatch** (4 categories, 18 demo screens, `src/Demos/Graphics2D/{DrawingBasics,
  SortModes,DrawString,BeginEndState}/`) — **implemented**. *Drawing Basics* (5 screens):
  position-only `Draw`, destination-rectangle scaling, source-rectangle sub-sprite selection,
  rotation/origin, and `SpriteEffects` flip + scale, all pixel-verified including a
  quadrant-colored (red/green/blue/yellow) test texture specifically so flips/rotation can be
  checked for exact pixel correctness rather than "looks about right". *Sort Modes* (5 screens):
  `Deferred`/`Immediate` (visually identical submission-order result, confirmed side by side),
  `Texture` (honestly shown as pointer-dependent/nondeterministic, not given false precision),
  and `BackToFront`/`FrontToBack` proven as exact visual opposites via two comparable
  screenshots. *DrawString* (4 screens): basic text, a transformed/orbiting text demo,
  `SpriteEffects` on `DrawString` (confirmed the **whole string mirrors as a block**, e.g.
  "ABC 123" flips to read backwards, not each glyph individually in place) plus the
  `MeasureString` trailing-newline gotcha (a trailing `\n` adds one full empty line's height, not
  a fraction), and a `StringBuilder` overload screen. *Begin/End & State* (4 screens): a real
  nested-`Begin()` exception, a real draw-outside-`Begin()` exception (app remains stable after
  both), a live device-state readback proving `BlendState` set inside a batch leaks past `End()`
  until explicitly reset, and the 7-argument `Begin()` transform-`Matrix` overload driving an
  orbiting-camera effect over 3 fixed-position markers.
- **Textures & Fonts** (3 categories, 9 demo screens, `src/Demos/Graphics2D/{Texture2DBasics,
  SaveAsReload,SpriteFont}/`) — **implemented**. *Texture2D Basics* (3 screens): the NOXNA
  `CreateFromPixels` one-liner vs. the XNA-idiomatic `Texture2D(device,w,h)` + `SetData`
  construction paths side by side, a real `SetData`/`GetData` round trip confirmed byte-for-byte
  (`MATCH (all 64 pixels)`), and `IsDisposed`/`HasBackend()` lifecycle checks after `Dispose()`.
  *SaveAs & Reload* (2 screens): a genuine on-disk round trip for both `SaveAsPng` (a real
  393-byte PNG, confirmed via the `file` command and reloaded pixel-exact) and `SaveAsJpeg` (a
  real 1965-byte JPEG, reloaded closely matching per JPEG's lossy nature, noted honestly rather
  than hidden). *SpriteFont* (4 screens): `MeasureString` on short/long/multi-line samples with
  real measured dimensions; `DefaultCharacter` fallback, confirming both the real fallback
  substitution and the real `std::invalid_argument("Text contains characters that cannot be
  resolved by this SpriteFont.")` throw when unset (the font's actual missing-glyph search is
  dynamic, not a hardcoded assumed codepoint); live-adjustable `LineSpacing`/`Spacing`; and a
  brand-new `SpriteFont` built entirely from scratch via the NOXNA public constructor (10
  flat-color digit glyphs on a hand-built atlas) — this last screen caught a genuine bug during
  verification (below).
- **Device State & Blending** (3 categories, 7 demo screens, `src/Demos/Graphics2D/{BlendState,
  SamplerState,ViewportScissor}/`) — **implemented**. *BlendState* (2 screens): `Opaque`/
  `AlphaBlend`/`Additive`/`NonPremultiplied` cycled live over two overlapping squares, plus a
  dedicated Premultiplied-Alpha-gotcha screen showing the same orange color built two ways
  (correct pairing vs. a deliberately swapped/wrong pairing) to explain exactly why `Additive`/
  `NonPremultiplied` look dimmer than `AlphaBlend`/`Opaque` when combined with the wrong kind of
  source color. *SamplerState* (2 screens): `PointClamp` (crisp) vs. `LinearClamp` (blurry)
  filtering on the same 8×8-upscaled-to-200×200 texture, and `PointWrap` (tiles 4×4) vs.
  `PointClamp` (edge-smear) addressing on a 4×-oversized source rectangle — both real,
  EasyGL-pixel-verified differences (the Mirror address mode was deliberately skipped as
  optional/redundant with Wrap for demonstration purposes). *Viewport & Scissor* (3 screens): a
  live `Viewport` inspector that shrinks/restores a 300×200 sub-viewport (confirmed real —
  shrinking genuinely clips all rendering, including `Clear()`, to the sub-rect, leaving stale
  pixels outside it until the viewport is restored and a full-screen `Clear()` runs again — an
  authentic GPU behavior, not a bug); real `ScissorRectangle` clipping (confirmed a real bug
  during verification, below); and a confirmed gotcha that `SetRenderTarget()` **silently resets
  Viewport and ScissorRectangle** back to the new target's full bounds every time it's called,
  even switching to the same backbuffer via `SetRenderTarget(nullptr)`.
- **Render Targets** (3 categories, 4 demo screens, `src/Demos/Graphics2D/{RenderToTextureBasics,
  ScreenTransition,DisposeSafety}/`) — **implemented**. *Render-to-Texture Basics* (2 screens): a
  source texture drawn once into a `RenderTarget2D` in `LoadContent()` then sampled every frame
  like any ordinary `Texture2D` (it is one), and two independent `RenderTarget2D` instances
  switched to and from multiple times, confirming each keeps its own distinct content through a
  round trip via the backbuffer. *Screen Transition* (1 screen): a live post-effect pattern —
  three drifting quads rendered into a `RenderTarget2D` every frame, then drawn back over the
  backbuffer with a continuously pulsing alpha, confirmed genuinely animating (not a static
  frame) via two screenshots showing both the quad positions and the alpha level changed.
  *Dispose Safety* (1 screen): `RenderTarget2D::Dispose()` confirmed to really throw
  `System::InvalidOperationException("Disposing target that is still bound")` when the target is
  still the device's active render target, matching FNA's own "disposing a bound render target
  is a programming error" behavior — the app remains fully stable and navigable after the
  exception.

See `BuildDrawingBasicsDemos()`/`BuildSortModesDemos()`/`BuildDrawStringDemos()`/
`BuildBeginEndStateDemos()`/`BuildTexture2DBasicsDemos()`/`BuildSaveAsReloadDemos()`/
`BuildSpriteFontDemos()`/`BuildBlendStateDemos()`/`BuildSamplerStateDemos()`/
`BuildViewportScissorDemos()`/`BuildRenderToTextureBasicsDemos()`/`BuildScreenTransitionDemos()`/
`BuildDisposeSafetyDemos()` in `AreaCatalog.hpp`.

**Real defects found and fixed during Xvfb verification** (all rebuilt and reverified after
fixing, not just reasoned about): the hand-built `SpriteFont` screen reused its atlas-cell
rectangle for both `glyphBounds` (correct) and `cropping` (wrong — cropping is a per-glyph
*rendering offset relative to the pen*, not the glyph's atlas position; reusing it doubled up
with the pen's own advance and reversed left-to-right glyph order for any non-monotonic string,
e.g. drawing "42" rendered as "24" — confirmed by tracing `SpriteBatch::DrawString`'s actual
layout math against the bug before fixing it); the Scissor Clipping screen set
`GraphicsDevice::ScissorRectangle` to its narrow demo rect **before** calling the first
`sb.End()`, so the base class's already-queued title text was clipped along with the intended
texture once `SpriteBatch`'s deferred flush ran under the new (narrow) scissor state — fixed by
moving the state change to after that `End()` call, matching the pattern already used correctly
elsewhere in this Area (e.g. `ScreenTransition/FadeTransitionScreen.hpp`). Several smaller
text-layout/label-collision fixes (side-by-side labels overlapping at this app's 960px window
width) were also found and fixed the same way as in prior Areas.

Deliberately **not** covered: raw vertex/primitive drawing (`VertexPositionColor`,
`GraphicsDevice::DrawUserPrimitives`, `PrimitiveType`) — reserved for a future 3D Graphics area
per explicit direction; the `SamplerState` Mirror address mode (redundant with Wrap for this
Area's demonstration purposes); and hands-on verification against any GPU/driver other than this
dev machine's EasyGL/Mesa software path.

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
    │   ├── GroupScreen.hpp            (optional 5th level -- see section 4; 2D Graphics only so far)
    │   ├── CategoryScreen.hpp
    │   └── AreaCatalog.hpp            (the AreaEntry/GroupEntry/CategoryEntry/DemoEntry data)
    ├── GameStateManagement/           (ScreenManager/GameScreen/MenuScreen/MenuEntry/InputState,
    │                                    adapted from ../cna-samples/samples/GameStateManagement)
    └── Demos/
        ├── DemoScreen.hpp             (shared leaf-screen chrome: title, Back, DrawLines() helper)
        ├── Input/
        │   ├── Keyboard/              (10 DemoScreen subclasses -- see section 5.1)
        │   ├── Mouse/                 (10 DemoScreen subclasses -- see section 5.1)
        │   ├── Gamepad/               (10 DemoScreen subclasses -- see section 5.1)
        │   ├── Touch/                 (10 DemoScreen subclasses -- see section 5.1)
        │   └── Other/                 (10 DemoScreen subclasses -- see section 5.1)
        ├── Audio/
        │   ├── AudioDemoHelpers.hpp             (GenerateSineWavePcm16() -- no WAV asset needed)
        │   ├── SoundEffect/                     (2 DemoScreen subclasses -- see section 5.2)
        │   ├── SoundEffectInstance/             (3 DemoScreen subclasses -- see section 5.2)
        │   ├── Audio3D/                         (2 DemoScreen subclasses -- see section 5.2)
        │   ├── DynamicSoundEffectInstance/      (1 DemoScreen subclass -- see section 5.2)
        │   └── Microphone/                      (2 DemoScreen subclasses -- see section 5.2)
        ├── Devices/
        │   ├── Sensors/                         (4 DemoScreen subclasses -- see section 5.3)
        │   ├── Vibration/                       (1 DemoScreen subclass -- see section 5.3)
        │   ├── Camera/                          (1 DemoScreen subclass -- see section 5.3)
        │   ├── SystemAndDisplay/                (3 DemoScreen subclasses -- see section 5.3)
        │   ├── Power/                           (1 DemoScreen subclass -- see section 5.3)
        │   └── DesktopIntegration/              (5 DemoScreen subclasses -- see section 5.3)
        ├── Net/
        │   ├── NetDemoHelpers.hpp               (EndSession() + enum-to-string helpers)
        │   ├── NetworkSession/                  (5 DemoScreen subclasses -- see section 5.4)
        │   ├── NetworkGamer/                    (2 DemoScreen subclasses -- see section 5.4)
        │   ├── GamerServices/                   (5 DemoScreen subclasses -- see section 5.4)
        │   └── Leaderboards/                    (2 DemoScreen subclasses -- see section 5.4)
        ├── Media/
        │   ├── MediaDemoHelpers.hpp             (content paths + MediaStateName())
        │   ├── Song/                            (6 DemoScreen subclasses -- see section 5.5)
        │   ├── Video/                           (3 DemoScreen subclasses -- see section 5.5)
        │   └── MediaLibrary/                    (2 DemoScreen subclasses -- see section 5.5)
        └── Graphics2D/
            ├── TextureDemoHelpers.hpp           (procedural checkerboard/gradient/grid textures)
            ├── DrawingBasics/                   (5 DemoScreen subclasses -- see section 5.6)
            ├── SortModes/                       (5 DemoScreen subclasses -- see section 5.6)
            ├── DrawString/                      (4 DemoScreen subclasses -- see section 5.6)
            ├── BeginEndState/                   (4 DemoScreen subclasses -- see section 5.6)
            ├── Texture2DBasics/                 (3 DemoScreen subclasses -- see section 5.6)
            ├── SaveAsReload/                     (2 DemoScreen subclasses -- see section 5.6)
            ├── SpriteFont/                       (4 DemoScreen subclasses -- see section 5.6)
            ├── BlendState/                       (2 DemoScreen subclasses -- see section 5.6)
            ├── SamplerState/                     (2 DemoScreen subclasses -- see section 5.6)
            ├── ViewportScissor/                  (3 DemoScreen subclasses -- see section 5.6)
            ├── RenderToTextureBasics/            (2 DemoScreen subclasses -- see section 5.6)
            ├── ScreenTransition/                 (1 DemoScreen subclass -- see section 5.6)
            └── DisposeSafety/                    (1 DemoScreen subclass -- see section 5.6)
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
- A `Content/` directory (fonts/textures the navigation shell itself needs, e.g. a menu font,
  plus `Content/MediaDemo/` — 5 fully synthetic ffmpeg-generated audio/video assets, see section
  5.5) is copied next to the built executable, same post-build pattern as `cna-samples`.

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
  `DrawLines()` helper) plus all 10 Keyboard, all 10 Mouse, all 10 Gamepad, all 10 Touch, and all
  10 Other demo screens (see section 5.1) — every category in the Input area now has real demo
  content, wired into `AreaCatalog.hpp`. `MenuScreen` also gained auto-scroll-to-selection
  (`scrollOffset_`) once Keyboard's 10-demo + Back list overflowed a single screen — any
  category with enough demos to overflow the viewport now scrolls correctly, not just Keyboard.
- The Audio area's 5 categories, 12 demo screens total (see section 5.2), wired into
  `AreaCatalog.hpp` the same way. `AudioDemoHelpers.hpp`'s procedural sine-wave generator is new,
  reusable infrastructure for this and any future Audio demo that needs a tone with no WAV asset.
- The Devices area's 6 categories, 15 demo screens total (see section 5.3), wired into
  `AreaCatalog.hpp` the same way. This required forcing `CNA_DEVICES ON` in this app's own
  `CMakeLists.txt` (the option defaults `OFF` in `CNA` itself) so `CNA::Devices::*` compiles in
  at all.
- The Net area's 4 categories, 14 demo screens total (see section 5.4), wired into
  `AreaCatalog.hpp` the same way. This required linking the separate `CNA_Net`/
  `CNA_GamerServices` static-library targets (`cmake/ExamplesHelpers.cmake`) and registering a
  `GamerServicesComponent` in `CnaExamplesGame.hpp` at app startup.
- The Media area's 3 categories, 11 demo screens total (see section 5.5), wired into
  `AreaCatalog.hpp` the same way. Needed no CMake changes at all (FFmpeg-backed `Video`/
  `VideoPlayer` build into the umbrella `CNA` target directly on Linux); the only new
  infrastructure is `Content/MediaDemo/`'s 5 ffmpeg-generated synthetic assets.
- The 2D Graphics area's 4 Groups / 13 categories, 38 demo screens total (see section 5.6),
  wired into `AreaCatalog.hpp` the same way — the first Area to use the new Group navigation
  level (`GroupEntry`, `GroupScreen.hpp`, section 4). Needed no CMake changes; the only new
  infrastructure is `Demos/Graphics2D/TextureDemoHelpers.hpp`'s procedural texture generators
  (no image asset bundled). Found and fixed 5 real defects during Xvfb verification — a reversed
  glyph-order bug in the hand-built `SpriteFont` demo, a scissor/`End()`-ordering bug that
  clipped title text, and 3 text-layout overflow/collision fixes — see section 5.6's closing
  note for details.

Explicitly **out of scope** still (future work):

- Demo screens for the remaining Area (3D Graphics) — Input's five categories, Audio's five
  categories, Devices' six categories, Net's four categories, Media's three categories, and 2D
  Graphics' 13 categories are all done.
- Hands-on verification passes for the Gamepad and Touch demos (and the joystick/haptics
  screens within Other) against real hardware (see section 5.1's notes on each) — no
  controller, touchscreen, raw joystick, or haptic device was available while building them.
  (Audio, by contrast, was verified against real hardware — see section 5.2.)
- Hands-on verification of the Devices area's mobile-only Sensors/Vibration screens on real
  Android hardware, and of Camera/MessageBox/FileDialog against a physical webcam/real human
  interaction respectively — see section 5.3's per-screen notes on exactly what was and wasn't
  exercised on this desktop dev machine.
- A genuine two-process SystemLink host+discover+join round trip for the Net area — see section
  5.4's note on the specific test-harness limitation (two unmanaged, overlapping Xvfb windows
  with no reliable way to route synthetic input to one over the other) that blocked it this
  session; every single-process code path both screens exercise is verified.
- The Avatar sub-namespace (`GamerServices::AvatarRenderer`/...) — see section 5.4's closing
  note on why this is a deliberate omission (already covered by 8 existing `cna/examples/`
  demos, and cross-cutting enough to warrant its own pass).
- XACT (`AudioEngine`/`SoundBank`/`WaveBank`/`Cue`/`AudioCategory`) — see section 5.2's closing
  note on why this is a deliberate omission, not an oversight.
- `feature/media`'s much larger, still-unmerged rewrite (real `Song.Album/Artist/Genre`, a
  working `MediaLibrary`, real visualization) — see section 5.5's opening note; this Area
  describes `develop`'s actual current behavior only, and should be revisited once/if that
  branch merges. `Picture`/`PictureAlbum` also intentionally not given their own screens beyond
  the shared "unreachable" narration `Album`/`Artist`/`Genre`/`Playlist` already cover.
- The Phase 11 hardware-validation demonstrations are now covered in concept by the Keyboard/
  Mouse/Gamepad/Touch/Other demos above, modulo the hands-on verification notes above.
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
