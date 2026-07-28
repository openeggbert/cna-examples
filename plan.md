# cna-examples — Plan

**Revision date:** 2026-07-28
**Supersedes:** [`plan20260727.md`](plan20260727.md) — the previous plan, archived verbatim. That
document is the historical record of how the first seven Areas were designed, built and verified
(including per-screen detail and the defect write-ups from each verification pass). It is still
the reference for *why* an existing screen looks the way it does; this document is the reference
for *what the app is now and what it becomes next*.

---

## 1. Purpose

`cna-examples` is a single cross-platform application that lets a user browse and run live
demonstrations of every area of [CNA](https://github.com/openeggbert/cna) — the C++
reimplementation of the XNA 4.0 programming model, built on SDL3 and a pluggable graphics
backend layer.

It plays the role for CNA that [javafx-ensemble8](https://github.com/lusalome/javafx-ensemble8)
plays for JavaFX: one browsable sample-catalog app with in-app navigation, not a collection of
separate executables. It is conceptually inspired by ensemble8's structure and shares no code or
assets with it.

Two sibling repos cover adjacent ground and are deliberately *not* duplicated here:

- **`../cna-samples`** ports ~90 official XNA sample executables, one binary per sample, for
  structural/API-fidelity reference. Ms-PL, because it is a port of Microsoft's samples.
- **`../cna/examples/`** holds ~600 backend pixel-tests plus ~25 standalone `demo_*`
  executables that CNA's own development loop uses (`demo_xact`, the 8 `demo_avatar*` programs,
  the Net demos, …). Those are development instruments: CLI-driven, single-purpose, often
  headless-verified. `cna-examples` is the *user-facing* catalog — everything reachable from one
  menu, on desktop, web and mobile.

Where `../cna/examples/` has already solved a hard content problem (runtime XACT bank
generation, avatar mesh assets), `cna-examples` reuses that solution rather than re-deriving it.

## 2. Current state (2026-07-28)

Thirteen Areas, **249 demo screens** across 79 categories, all with real content. The numbers below
are produced by `tools/check_catalog.py`, which cross-checks the screen files on disk against the
`MakeDemo<>` registrations in `src/Navigation/AreaCatalog.hpp` and against the counts written into
this file and `README.md`. Nothing here is counted by hand.

| Area | Groups | Categories | Screens |
|---|---:|---:|---:|
| Framework | — | 5 | 17 |
| Math | — | 5 | 16 |
| Content | — | 5 | 7 |
| Storage | — | 2 | 4 |
| Diagnostics | — | 4 | 4 |
| Input | — | 5 | 52 |
| Audio | — | 6 | 12 |
| Devices | — | 6 | 15 |
| Net | — | 4 | 15 |
| Media | — | 4 | 17 |
| Avatars | — | 3 | 7 |
| 2D Graphics | 4 | 13 | 40 |
| 3D Graphics | 5 | 17 | 43 |
| **Total** | **13** | **79** | **249** |

Before the Phase A work described below, the catalog held **168** demos in 50 categories. (An
early draft of this document said 169 — that number came from counting `*Screen.hpp` files, which
also counts `DemoScreen.hpp`, the shared base class. The checker exists precisely because that
kind of hand count keeps going wrong.)

Per-category breakdown:

| Area | Categories (screens) |
|---|---|
| Framework | Game Loop (4), Game Components (4), Services & Dispatcher (3), Window (3), Device Manager (3) |
| Math | Vectors (5), Matrix & Quaternion (4), Geometry (3), Curves (2), Color & Packed Vectors (2) |
| Content | ContentManager Basics (2), Manifest (1), CNJ Format (2), XNB Format (1), Errors (1) |
| Storage | StorageDevice (1), StorageContainer (3) |
| Diagnostics | Logging (1), Platform & Build (1), Backend & Capabilities (1), Adapter & Display (1) |
| Input | Keyboard (10), Mouse (10), Gamepad (11), Touch (10), Other (11) |
| Audio | SoundEffect (2), SoundEffectInstance (3), 3D Audio (2), DynamicSoundEffectInstance (1), Microphone (2), XACT (2) |
| Devices | Sensors (4), Vibration (1), Camera (1), System & Display (3), Power (1), Desktop Integration (5) |
| Net | NetworkSession (6), NetworkGamer (2), GamerServices (5), Leaderboards (2) |
| Media | Song (6), Video (3), MediaLibrary (4), Pictures (4) |
| 2D Graphics | Drawing Basics (5), Sort Modes (5), DrawString (4), Begin/End & State (4), Texture2D Basics (5), SaveAs & Reload (2), SpriteFont (4), BlendState (2), SamplerState (2), Viewport & Scissor (3), Render-to-Texture Basics (2), Screen Transition (1), Dispose Safety (1) |
| 3D Graphics | Vertex Types (3), Primitive Types (2), Buffers (3), Basic Rendering (3), Lighting (3), Fog (1), AlphaTestEffect (2), DualTextureEffect (1), EnvironmentMapEffect (2), SkinnedEffect (1), Custom Shader (2), Depth & Culling (3), Camera & Projection (2), Model (6), Volume & Cube Textures (4), Effect Reflection (3), PbrEffect (2) |
| Avatars | AvatarDescription (2), AvatarRenderer (3), Appearance & Wardrobe EXT (2) |

### 2.1 Defects found in the pre-existing state

These were real, verified problems with what shipped before this revision. Items 1–3 are **fixed**
(Phase A1/A3/A5); item 4 is still open (A2).

1. **The Media area is factually wrong.** Its 11 screens narrate `MediaLibrary` as a "hard stub"
   whose every accessor throws `not implemented`, `Album`/`Artist`/`Genre`/`Playlist` as
   permanently unreachable, and `MediaPlayer` visualization as a "confirmed, permanent stub". All
   three claims were true against `../cna` `develop` when the screens were written. They stopped
   being true when `feature/media` merged into `develop` on **2026-07-18 18:52**: `MediaLibrary.cpp`
   is now 604 lines of real music/picture indexing, `Song::getAlbumProperty()`/`getArtistProperty()`/
   `getGenreProperty()` return real objects, and `MediaPlayer`'s visualization is a real post-mix
   capture with an FFT behind it. The screens must be rewritten against the merged behavior.
2. **`README.md` is stale.** It states that only Input is complete and that the other six Areas
   "still show an empty '(coming soon)' category list". Every Area has had real content since
   2026-07-18.
3. **The archived plan miscounts Audio.** `plan20260727.md` §8 claims 12 Audio screens; there are
   10. Nothing enforces that the prose and the code agree.
4. **Input predates the `feature/input` merge.** `plan20260727.md` §9 says so explicitly: the
   Input screens describe `develop`'s pre-merge Input behavior. `feature/input` merged on
   2026-07-18 19:25. *Re-checked in A2: no narration was actually invalidated — see §7's A2
   result. This one turned out to be a risk, not a defect.*

## 3. Platform targets

| Tier | Platforms | Status |
|---|---|---|
| Now | Linux, Windows | Built and run regularly |
| Now | Web (Emscripten) | Targeted by Phase F2 — CNA supports it; this app has not been built for it yet |
| Later | Android | CNA supports it; deferred (no hardware pass planned this cycle) |
| Future | macOS, iOS, consoles | Gated on CNA's own platform support |

CNA's `include/CNA/Entrypoint.hpp` provides the cross-platform `main()`/`SDL_main` wiring for all
of the above, so `cna-examples` needs no per-platform entry-point code — every platform uses the
same `Program.cpp`.

Because touch is a first-class input, navigation is designed touch-first: large tap targets,
vertical list menus, no reliance on hover. Phase B2 closes the one remaining touch gap (a
touch-only user cannot currently scroll a list past what is already on screen).

## 4. Graphics backend strategy

CNA selects its backend at compile time via `CNA_GRAPHICS_BACKEND`. This app defaults it to
`EASYGL` (CNA's most complete backend) and never hard-codes a backend-specific assumption — it
uses only public `Microsoft::Xna::Framework` APIs.

`EASYGL` and `SDL_RENDERER` are both verified (Phase F3). `SDL_RENDERER` is
deliberately 2D-only: every 3D call throws. That was not a reason to skip it — it was the reason
to do it. It forced the catalog to gate demos on `GraphicsDevice::SupportsCapability()` rather
than assume a full pipeline, which is exactly what a real CNA consumer has to do. The
`Diagnostics` area (Phase C5) makes that capability model visible to the user as a demo in its
own right.

**F3 result.** *(Figures below are as-of F3, when the catalog held 218 screens. It has since grown
to 248, and both backends were re-verified at 248/248 — see §7.0.)*
`tools/sweep_backend.sh build-sdlrenderer` renders **218/218** with zero layout
problems, and the EasyGL tree still renders 218/218 — no regression from the gating.

Two rounds were needed, and the second is the interesting one:

1. Gating `Update`/`Draw`/`HandleInput` on the capability took the failures from a crash on every
   3D demo down to **4 of 218**.
2. Those four still aborted with `SDL_Renderer does not support 3D: CreateVertexBuffer`, because
   they build their GPU resources in `LoadContent`, which runs long before `Draw`. Gating only
   the draw path is not enough — the load path is where a 3D demo actually touches the device.

`DemoScreen::LoadContent`/`UnloadContent` are now `final` and gate a new
`OnDemoLoad`/`OnDemoUnload` pair, so a demo cannot accidentally bypass the check by overriding
the wrong method. All 139 demo screens were migrated to the new hooks. A `loaded_` flag ensures
the unload hook runs if and only if the load hook did.

Gating is applied per category at the catalog assembly site (`Requiring()` in `AreaCatalog.hpp`),
not inside each screen, so a new demo dropped into a gated category inherits the gate instead of
having to remember it.

A gated demo shows an amber panel naming the missing capability and the active backend, and
points at `Diagnostics > Backend & Capabilities`. Verified by pixel measurement: 10196 amber
pixels where the 3D scene would otherwise be.

## 5. Navigation architecture

The shell is adapted from the XNA **Game State Management** pattern (`ScreenManager`,
`GameScreen`, `MenuScreen`, `MenuEntry`, `InputState`), which CNA already has proven working at
`../cna-samples/samples/GameStateManagement`. `cna-examples` reuses the pattern, not the sample's
screens.

### 5.1 Screen hierarchy

```
HomeScreen (MenuScreen)
 └─ one MenuEntry per Area
      ▼
AreaScreen (MenuScreen)
 └─ one MenuEntry per Group, OR one per Category
      ▼                              (an Area populates exactly one of groups/categories)
GroupScreen (MenuScreen)             [only when an Area needs the extra level]
 └─ one MenuEntry per Category
      ▼
CategoryScreen (MenuScreen)
 └─ one MenuEntry per Demo
      ▼
DemoScreen (GameScreen, one concrete subclass per demo)
```

Every level above `DemoScreen` is generic and data-driven, built from the declarative structs in
`src/Navigation/AreaCatalog.hpp`. Adding an Area, Group, Category or Demo is a data change, not a
new screen class.

### 5.2 Planned shell changes

`DemoEntry` gains one field in Phase B4:

```cpp
struct DemoEntry {
    std::string title;
    std::string description;
    std::vector<std::string> apis;   // NEW: the CNA/XNA symbols this demo exercises
    std::function<std::shared_ptr<GameScreen>()> create;
};
```

`apis` drives three things at once: a footer line on the demo screen itself, the search index
(Phase B1), and the coverage report (Phase A5) — so the "which API does this demo prove works?"
question is answered from one place instead of three.

## 6. Coverage analysis — CNA's public API vs this catalog

This is the analysis the roadmap is derived from. It was produced by enumerating every public
header under `../cna/include/` (excluding `CNA/Internal/**`, which is not a consumer-facing
surface) and checking each against the 169 existing screens.

### 6.1 Covered

`Microsoft::Xna::Framework::Input` (+`Touch`), `CNA::Input`, `Microsoft::Devices::Sensors`,
`Microsoft::Devices::VibrateController`, `CNA::Devices`, `Microsoft::Xna::Framework::Net`,
most of `GamerServices`, `Media` (`Song`/`Video`, though see §2.1), and the `SpriteBatch` /
`Texture2D` / `SpriteFont` / state-object / `RenderTarget2D` / vertex-buffer / stock-`Effect`
half of `Graphics`.

### 6.2 Uncovered or partial

| CNA surface | Coverage | Addressed by |
|---|---|---|
| `Game`, `GameComponent`, `DrawableGameComponent`, `GameComponentCollection`, `GameServiceContainer`, `GameTime`, `GameWindow`, `GraphicsDeviceManager`, `FrameworkDispatcher`, `LaunchParameters`, `TitleContainer` | **none** | C1 Framework |
| `Vector2/3/4`, `Matrix`, `Quaternion`, `Plane`, `Ray`, `BoundingBox`, `BoundingSphere`, `BoundingFrustum`, `MathHelper`, `Curve`/`CurveKey`/`CurveKeyCollection`, `Color`, `Point`, `Rectangle`, `ContainmentType`, `PlaneIntersectionType` | **none** | C2 Math |
| `Graphics::PackedVector` (20 types) | **none** | C2 Math |
| `Content::ContentManager`, `ContentReader`, `ResourceContentManager`, `ContentLoadException`, content manifest, `.cnj`, `.xnb` (incl. LZX), custom type readers | **none** (the menu font is loaded, but nothing demonstrates the API) | C3 Content |
| `Storage::StorageDevice`, `StorageContainer` | **none** | C4 Storage |
| `CNA::Logger`/`LogLevel`/`LogCategory`, `CNAException`, `Platform`, `DesktopOS`, `GraphicsBackendType`, `GraphicsCapability` | **none** | C5 Diagnostics |
| `GraphicsAdapter`, `DisplayMode`, `DisplayModeCollection`, `GraphicsProfile`, `PresentationParameters`, `GraphicsDeviceStatus`, `DeviceLostException`, `DeviceNotResetException` | **none** (`Viewport` is covered) | C5 Diagnostics |
| XACT: `AudioEngine`, `SoundBank`, `WaveBank`, `Cue`, `AudioCategory` | **none** (deliberately deferred by the previous plan) | D1 |
| `PbrEffect`, `SkinnedPbrEffect`, `CNA::Graphics::{PbrMaterial, RenderPipelineSettings, RenderQuality, ShadowQuality, TonemappingMode}` | **none** (deliberately deferred) | D2 |
| `Model` from real content, `ModelMeshPart`/`EffectMaterial`, `SkinnedModelEXT`, `AnimationPlayer`, `MorphTargetEXT` | **partial** — `Model` screens are procedural only | D3 |
| `EffectParameterCollection`, `EffectTechniqueCollection`, `EffectPassCollection`, `EffectAnnotation`, `Effect::Clone`, `CurrentTechnique` | **none** (a custom GLSL shader is drawn, but never reflected on) | D4 |
| `Texture3D`, `TextureCube`, `RenderTargetCube`, `OcclusionQuery` | **none** (`TextureCube` is used indirectly by `EnvironmentMapEffect`) | D5 |
| `SurfaceFormat` matrix, DXT compression, `Texture2D::FromStream`, MSAA, `PresentInterval`, device reset / back-buffer resize | **none** | D6 |
| `GamePad` gyro/accelerometer/light-bar/trigger-rumble EXT; `TouchPanel` display orientation | **none** | D7 |
| `Net::QualityOfService`, the full `SendDataOptions` matrix, simulated network conditions | **partial** | D8 |
| `GamerServices` Avatar sub-namespace: `AvatarDescription`, `AvatarRenderer`, `AvatarAnimation`, `AvatarExpression`, `AvatarAppearanceEXT`, `AvatarBodyType`, presets | **none** (deliberately deferred) | E Avatars |
| `Media::Picture`, `PictureAlbum`, `PictureCollection`, saved pictures | **none** | A1 |

### 6.3 Feasibility notes established while producing this analysis

- **XACT needs no bundled bank files.** `../cna/examples/demo_xact/src/XactFileGen.hpp` is a
  389-line, self-contained, dependency-free generator that writes `.xgs`/`.xsb`/`.xwb` binaries
  matching exactly what CNA's parser expects. It is adapted into this app rather than shipping
  opaque binary assets.
- **`.xnb` fixtures already exist**, checked into `../cna/tests/assets/xnb/monogame/windows/`:
  uncompressed textures, an LZX-compressed texture and font, a `Song`, and six `SoundEffect`
  files covering PCM 8/16-bit, float, MS-ADPCM and IMA-ADPCM. CNA consumes `.xnb` but never
  produces it, so reusing real MonoGame-produced fixtures is the only honest option.
- **Avatar assets are version-controlled** in `../cna/examples/demo_avatar/Content/avatar/`
  (812 KB, male + female + wardrobe). No Blender/mesh-craft pipeline run is needed to render an
  avatar — only to author a *new* one.
- **`PbrEffect`, `OcclusionQuery`, `Texture3D` and `TextureCube` all work on EasyGL** and have
  golden pixel tests in `../cna/examples/`. `Texture3D`/`TextureCube` carry a documented,
  cross-backend caveat: the `SurfaceFormat` argument is ignored and every texture is RGBA8. D5
  demonstrates that caveat rather than hiding it.
- **`Storage` is fully implemented** (473 lines across `StorageDevice`/`StorageContainer`),
  using XNA 4.0's fake-async `Begin*`/`End*` pattern where the `Begin` completes synchronously.

## 7. Roadmap

Six phases. A is corrective and comes first because the app currently tells users things that are
untrue. B is enabling: a 290-screen catalog is unusable without search and scrolling. C, D and E
are the content build-out. F is verification.

Original target end state: **13 Areas, ~290 demo screens.**

### 7.0 Status as of 2026-07-28 — 249 screens, A/B/C/D/E/F1/F1b/F3 all done; only F2 remains, blocked upstream

| Phase | Status |
|---|---|
| A, B, C1–C5 | **Done** (C4 extended to 4 screens 2026-07-28, still reduced from 6; C5 at reduced scope) |
| D1 XACT · D3 Model Content · D4 Effect Reflection · D5 Textures & Queries · D6 Formats · D7 Input EXT · D8 Net | **Done**, all at reduced scope — see each phase's result section |
| **D2 PBR** | **Done 2026-07-28** — 2 screens. Root cause (a polymorphic-vertex `sizeof()` mismatch) fixed by repacking into a private packed POD before upload; verified live by pixel probe. `docs/wip-pbr/` removed, superseded. `RenderPipelineSettingsScreen` shipped same-day as a follow-up (`CNA_NOXNA` now enabled, same precedent as `CNA_DEVICES`) |
| **E Avatars** | **Done** — 7 screens, 3 categories, exactly at plan.md's original target |
| **F1** defect sweep | **Done** — 2 real defects found and fixed (see its own result section) |
| **F1b** compiler warnings | **Done 2026-07-28** — `-Wall -Wextra` enabled on the `cna_examples` target only; 13 warnings found (all in this project's own code), all fixed, 0 remaining |
| **F2** Emscripten | **BLOCKED on a CNA defect** — compiles fully for wasm, fails linking `libCNA.a`. Exact one-line upstream fix now identified, not applied here |
| **F3** SDL_RENDERER | **Done** — 249/249 on both backends (re-verified after D2's follow-up, D8's diagnosis correction, and the warnings audit) |

**The ~290 target will not be reached by building every planned screen, and that bias was
consciously reversed mid-session (see NEXT.md §2a): D4 shipped 3 of 4, D6 2 of 6, D7 2 of 3, D8 1 of
3 under the old depth-over-breadth default, each cut verified as already covered elsewhere in the
catalog or as an API that does not exist. Phase E (built after the reversal) shipped exactly its
planned 7 rather than cutting further, even though one of its ideas (`AvatarExpression`) also had
nothing visual to show on its own — it was folded into a richer screen instead of dropped.**
**Before building any remaining phase, grep `src/Demos/` for the APIs its row claims are missing**
— three phases in a row shrank by half once that was done, and D3/E both found real, un-duplicated
gaps once they actually checked.

### Phase A — Correct what already exists

| # | Work |
|---|---|
| A1 | **Done.** Media rewritten against merged `feature/media`. The 2 MediaLibrary screens became 4 (Catalog Access, Song Metadata, Album/Artist/Genre, Playlists), a new **Pictures** category adds 4 (Browse, Album Tree, SavePicture, Tokens & Identity), and `Song/Visualization` became a live spectrum + waveform display instead of a "the buffer stays zero" proof. Media: **11 → 17 screens, 3 → 4 categories.** |
| A2 | **Done — outcome: nothing was wrong.** See below. |
| A3 | **Done.** `README.md` rewritten: real inventory, real verification status, headless usage. |
| A4 | **Done.** This document, plus the archive at `plan20260727.md`. |
| A5 | **Done.** `tools/check_catalog.py` cross-checks screen files, `MakeDemo<>` registrations and the counts in `README.md`/`plan.md`. It resolves each registration through its `Build*Demos()` function's `using namespace`, because class names alone are ambiguous on purpose (Song and Video both define `LoadAndPlayScreen`; four Input categories each define `StateEqualityHashScreen`). |

#### A2 result

The `feature/input` merge (`e78fa509`) touched 16 files across `Input`, `Game` and the SDL input
bridge. Diffing it against every Input demo's narration: **no existing claim was invalidated.**
The one absolute statement in the area — `LiveButtonsGridScreen`'s "with none connected,
`GetState()` always returns up" — is still exactly true.

The one change that looked dangerous turned out not to be. `KeyboardState` now drops keys whose
numeric value is ≥ 256 from its bitfield, which would silently remove keys from
`GetPressedKeys()`. Checked against the enum: the highest `Keys` value CNA defines is 254, so the
filter cannot drop a real key and no demo's behaviour changes.

What the merge did produce is four newly-demonstrable APIs that nothing covers yet. They are
folded into D7 rather than treated as new scope here:

- `TouchCollection::FindById` now writes an `Invalid` sentinel on the not-found path instead of
  leaving the out-parameter untouched (matching FNA). Not exercised by any demo.
- `TouchPanel::NO_FINGER`. (`MAX_TOUCHES` is already covered by two Touch demos.)
- `GamePadButtons::FromButtonArray` (NOXNA).
- `Game::IsActive` now follows desktop focus (`SDL_EVENT_WINDOW_FOCUS_LOST`/`GAINED`), not just
  mobile background/foreground. This belongs to the **Framework** area (C1), not Input.

#### A1 supporting work

- `Content/MediaLibraryDemo/` — a synthetic 6-song / 3-album / 2-artist / 2-genre / 1-playlist
  music tree plus a 5-picture album tree, regenerated by `tools/gen_media_library.sh`. Shaped
  deliberately to exercise the indexer: real Vorbis comments so tag parsing has something to
  parse, one untagged `.wav` so the filename-fallback path is visible, and exactly one album with
  a `cover.png` so `Album::HasArt` differs between albums.
- The demos redirect `MediaLibrary` at that tree via `MediaLibraryPaths::Set*RootOverride()` — a
  NOXNA test hook, labelled as such on screen. Without it the demos would rummage through the
  user's real music folder and produce different results on every machine. `Catalog Access` can
  flip to the real OS roots on demand, so the unredirected behaviour is visible too.
- `Pictures/SavePicture` writes to a temp directory, not to `Content/`: it is the one member of
  the whole Media area that creates files, and running a demo must not leave new files inside the
  checked-out repository.

### Phase B — Navigation shell

| # | Work |
|---|---|
| B1 | **Done.** `SearchScreen`, reachable as the first Home entry, matching incrementally as the user types over each demo's path + description + `apis`. Space-separated terms AND together, so typing more narrows. The matcher lives in `Harness/DemoIndex.hpp` and is shared with `--list-demos --search <q>`, which is what makes the filter assertable from a shell instead of only inspectable in a screenshot. Also `--search <q>` to deep-link into the screen. |
| B2 | **Done.** Drag-to-scroll and a scrollbar in `MenuScreen`. Two behavioural changes fell out and are the substance of it: selection moved from pointer *press* to *release* (selecting on press makes dragging impossible), and a manual scroll now suppresses `AutoScrollToSelection` until the selection moves (it used to yank the list straight back, making a drag look like a no-op). No inertia — deferred, since it needs velocity tracking and adds no correctness. |
| B3 | **Done.** Demo screens show `Area > Group > Category > Demo` in place of the bare title — replacing it rather than adding a row, because the band between title and first content line is fully used by every existing demo. Home/Area/Group entries carry a `(N)` demo count. |
| B4 | **Field landed; populated per area.** `DemoEntry::apis` renders right-aligned on the Back row (costing no vertical space) and feeds the search index. Media's 17 demos are populated; the other areas are filled in as each is built or revisited — see the assumption recorded in `NEXT.md`. |
| B5 | **Done, pulled forward.** `--list-demos`, `--demo <path>`, `--keys`, `--key-interval`, `--frames`, `--screenshot`, `--help`, plus `tools/headless.sh`, `tools/sweep.sh` and `tools/check_shots.py`. Built first because A1 could not be verified without it — see §7.2. |

### Phase C — New Areas

Five new Home entries, **72 screens**.

#### C1 Framework — 5 categories, 17 screens — **DONE**

| Category | Screens |
|---|---|
| Game Loop (4) | Fixed vs variable time step · `TargetElapsedTime` live (60/30/10 Hz) · `IsRunningSlowly` under deliberate Update overload · `SuppressDraw()` + `ResetElapsedTime()` |
| Game Components (4) | `GameComponent` lifecycle, added/removed live · `UpdateOrder`/`DrawOrder` reordering · `GameComponentCollection` added/removed events · `Enabled`/`Visible` + their change events |
| Services & Dispatcher (3) | `GameServiceContainer` add/get/remove, `IGraphicsDeviceService` lookup · `FrameworkDispatcher` — what `Game::Update` pumps for you · `LaunchParameters` from real argv |
| Window (3) | Title + `ClientBounds` live · `ClientSizeChanged` log, `AllowUserResizing` · `DisplayOrientation`, `SupportedOrientations`, `OrientationChanged` |
| GraphicsDeviceManager (3) | Resolution + `ToggleFullScreen`/`ApplyChanges` · `SynchronizeWithVerticalRetrace` + `PreferMultiSampling` · `PreparingDeviceSettings`/`DeviceCreated`/`DeviceReset`/`DeviceResetting` |

**Built as planned, 17/17.** Notes worth carrying forward:

- Every screen in this Area mutates process-global state (the timestep, the window title, the
  back-buffer size, v-sync). Each restores what it touched in `UnloadContent()`. This is verified,
  not assumed: changing the resolution and leaving produces a measurably 800x600 back buffer while
  the demo is open and a 960x640 one after, read straight off the captured screenshot.
- The `GraphicsDeviceManager` is owned privately by the app's `Game` subclass, so the demos reach
  it the XNA-idiomatic way instead — it registers itself in `Game::Services` as
  `IGraphicsDeviceManager`, and `Demos/Framework/DeviceManager/GdmAccess.hpp` looks it up there.
  No app-specific back door was added.
- `IsRunningSlowly` is demonstrated with a real busy-wait in `Update`, not a sleep: sleeping
  releases the core and is not what an overrunning game loop does.
- `DisplayOrientation` in CNA has exactly four values — there is no `PortraitDown` (that was
  Windows Phone 7, not the XNA 4.0 desktop enum). The demo says so rather than silently omitting it.
- `DeviceCreated` is subscribed to but can never appear in the log: the device is created during
  `Game` startup, long before a screen exists to listen. The screen states this instead of leaving
  a permanently empty line looking broken.

#### C2 Math — 5 categories, 16 screens — **DONE**

| Category | Screens |
|---|---|
| Vectors (5) | `Vector2` ops drawn as arrows · `Vector3` `Cross`/`Dot`/`Reflect` in 3D · `Lerp`/`SmoothStep`/`Barycentric`/`CatmullRom`/`Hermite` on a moving dot · `Transform` + `Distance`/`DistanceSquared` · `MathHelper` (`WrapAngle`, `ToRadians`, `Clamp`, …) |
| Matrix & Quaternion (4) | Composition order (T·R·S) with live sliders · `Decompose`/`Invert`/`Transpose`/`Determinant` round trip · `CreateFromAxisAngle`/`CreateFromYawPitchRoll` on a cube · `Slerp` vs `Lerp` vs Euler compared side by side |
| Geometry (5) | `BoundingBox` vs box/sphere/ray/plane/frustum → `ContainmentType` · `BoundingSphere` `CreateFromPoints`/`CreateMerged`/`Transform` · mouse-picking `Ray` against sphere/box/plane · `Plane::DotCoordinate` + `PlaneIntersectionType` + `BoundingFrustum` planes/corners · frustum culling a cube grid, live drawn-vs-culled count |
| Curves (3) | `Curve`/`CurveKey` spline with live key editing · `CurveTangent` Flat/Linear/Smooth compared · `CurveLoopType` Constant/Cycle/CycleOffset/Oscillate/Linear |
| Color & Packed Vectors (4) | `Color` construction, `ToVector3/4`, named-color grid · `Color::Lerp`, `PackedValue`, `FromNonPremultiplied` · all 20 `PackedVector` types, pack/unpack round trip + bit layout · precision loss visualized (`Bgr565`, `Bgra4444`, `HalfSingle`, `NormalizedByte2`, …) |

**Built as 16 screens, not the projected 21.** Five planned screens were dropped as
duplicates rather than padded out, per the estimate-not-commitment rule:

- *BoundingSphere* and *Plane & Frustum* folded into **Bounding Volumes** and **Frustum
  Culling** — `CreateFromPoints`/`CreateMerged` and the frustum's own planes are shown there,
  and separate screens would have repeated the same API with different framing.
- *Curve evaluation* folded into **Tangents**, which already evaluates a curve continuously;
  a third curve screen showing only `Evaluate` had nothing left to add.
- *Color Lerp/Pack* and *PackedVector precision* folded into **Conversions** and the
  **Gallery**, whose error column already is the precision story.

**Three demo claims were wrong and were corrected before shipping**, found by
`tools/checks/math_claims.cpp` — a verification program written specifically because these
screens assert facts rather than merely display state:

1. "A sheared matrix cannot be decomposed" — **false**. CNA's `Matrix::Decompose` (following
   FNA) derives scale from basis-row lengths and returns `false` only when an axis is ~zero.
   A shear returns `true` and yields parts that do **not** rebuild the original. The demo now
   multiplies the parts back together and shows the mismatch, because the return value alone
   will never reveal it.
2. "`0.1f + 0.2f != 0.3f`" — **false in single precision**. That famous example is a
   *double* result; in `float` the rounding coincides and the comparison is exactly equal.
   The demo now shows both that case and `0.3f + 0.6f`, which genuinely differs.
3. "`WithinEpsilon` is the fix" — **not for that pair**. `WithinEpsilon` is
   `|a-b| < MachineEpsilonFloat`, a fixed absolute tolerance of ~5.96e-8; the gap there is
   exactly one ULP, which equals the tolerance, and the test is a strict `<`. The demo now
   states the tolerance and why an absolute epsilon does not generalise.

Keeping that checker in the repo is the point: a screen that *asserts* something needs its
assertion tested, not just its pixels.

#### C3 Content — 4 categories, 5 screens — **DONE (reduced scope, see below)**

| Category | Screens |
|---|---|
| ContentManager Basics (4) | `Load<Texture2D>` + instance caching · `RootDirectory` and the real path-resolution order (`.xnb` → literal → `.cnj`) · `Unload()` and reload · `TitleContainer::OpenStream` + `TitleLocation` |
| CNJ Format (3) | This app's own `menufont.cnj`, explained and reloaded live · a `BasicEffect`/`AlphaTestEffect` `.cnj` descriptor · a `.cnj` model loaded and drawn |
| XNB Format (4) | Uncompressed `.xnb` texture · LZX-compressed `.xnb` · `FontCalibri14.xnb` rendered · the six wave-format `.xnb` sound effects, each played |
| Manifest & Custom Readers (2) | Content-manifest introspection: list every discoverable asset · `RegisterTypeReader<T>`/`RegisterCnjLoader<T>` for an app-defined type |
| Errors (2) | Missing asset / wrong type / corrupt file → the real `ContentLoadException` messages · deliberately-unsupported readers (`.fx` bytecode, `Model` `.xnb`) and what they actually say |

**Shipped as 5 screens across 4 categories, not the projected 15 across 5.** What is there is
complete and verified; the reduction is recorded rather than hidden.

*Delivered:* ContentManager Basics (Load & Cache, Asset Name Resolution), Manifest, XNB Format,
Errors.

*Not built, and why:*
- **CNJ Format** — **since built (2 screens, not the projected 3).** `Envelope` parses
  `cnjVersion`/`type`/`sourceFile` live from three real files including a deliberately invalid
  one; `Custom Loaders` registers two differently-named `.cnj` types (`EnemyDefinition` and
  `LootTable`) that both deserialise into one C++ struct, which is the whole point of
  `RegisterCnjLoader<T>` as distinct from `RegisterTypeReader<T>`, and exercises all three
  fail-fast registration rules live. A third screen showing only a `.cnj`-described SpriteFont
  would have repeated what `Asset Name Resolution` already covers.
  Verified by `tools/checks/cnj_claims.cpp` — 10 claims, all holding — rather than by
  screenshot, because these screens catch their own exceptions and would render cleanly even
  if every load failed. The demo assets (`Content/ContentDemo/cnj/*.cnj`) are this app's own,
  so nothing is borrowed.
- The remaining XNB screens (SpriteFont, sound effects, LZX as a separate screen) collapsed into
  one **XNB Fixtures** screen that cycles an uncompressed texture, a DXT1 cube map and an
  LZX-compressed texture. Splitting them would have repeated the same load call three times.
  The SpriteFont fixture could not be shipped at all — it is `FontCalibri14.xnb`, whose glyph
  atlas is a proprietary typeface.

**Assets are borrowed, not vendored.** `cmake/ExamplesHelpers.cmake` copies
`../cna/tests/assets/xnb` into the build output at build time and deletes `FontCalibri14.xnb`
(and its decompressed reference) on the way. Nothing Ms-PL enters this repository's history. A
checkout without `../cna` builds fine and the XNB screen says the fixtures are unavailable.

**Two findings that changed the demos:**

1. **`.xnb` support is off until you switch it on.** A fresh `ContentManager` throws
   *"references an unregistered .xnb content type reader 'Texture2DReader'"* on the first load.
   CNA deliberately does not auto-register its built-in readers; a game calls
   `CNA::Internal::Xnb::RegisterAllBuiltInXnbReaders()` once at startup. The error message does
   not name that function, so the screen shows the registration state before and after calling
   it. This cost a debugging cycle and is exactly the kind of thing the area should teach.
2. **`ContentManager` needs its `GraphicsDevice` set explicitly** (`setGraphicsDevice`) before
   any texture load; supplying only the service provider is not enough, matching what CNA's own
   `.xnb` tests do.

**Verified beyond "it renders":** the uncompressed fixture decodes to a fully white 150x150
preview (22500/22500 white pixels), the LZX-compressed one decodes to a genuinely coloured image
(15992 coloured pixels — real block decompression, not a placeholder), and the cube-map fixture
correctly fails to load as a `Texture2D`, which is the wrong-type case the screen is there to
show.

#### C4 Storage — 2 categories, 4 screens — **DONE (reduced scope, see below)**

| Category | Screens |
|---|---|
| StorageDevice (3) | `BeginShowSelector`/`EndShowSelector` per `PlayerIndex`, `IsConnected` · `DeviceChanged` event, free/total space · `SetAppNameEXT`/`GetStorageRootEXT` — where files really land on this OS |
| StorageContainer (3) | Save-game write + read round trip · directories and files: `CreateDirectory`/`GetFileNames`/`GetDirectoryNames`/`DeleteFile` · container lifetime: `Dispose`, `Disposing`, reopen, `DeleteContainer` |

**Shipped as 4 screens, not 6.** The original 2 covered the API's substance for the device/round-trip
half; **extended 2026-07-28** (per the owner's "push closer to projected counts" instruction) with
the two screens this section's own notes always called "the obvious next step": directory/file
operations and container lifetime.

- **StorageDevice** — the fake-async selector (`BeginShowSelector`/`EndShowSelector`) across all
  four `PlayerIndex` values, `IsConnected`/`FreeSpace`/`TotalSpace`, and the NOXNA
  `SetAppNameEXT`/`GetStorageRootEXT` pair that decides where saves actually land. `DeviceChanged`
  and the space-requirement selector overloads folded in here rather than becoming their own
  screens.
- **Save Game Round Trip** — open device, open container, `CreateFile`, write, close, `OpenFile`,
  read back, `DeleteFile`.
- **Directories & Files** (new) — `CreateDirectory`, `DirectoryExists`, nested `CreateFile`/`OpenFile`
  paths, `GetDirectoryNames()`/`GetDirectoryNames(pattern)`, `GetFileNames()`/`GetFileNames(pattern)`,
  `DeleteDirectory`. **Real finding**: neither `GetFileNames()` nor `GetDirectoryNames()` recurses —
  both only ever look at the container's own root. A file written to `notes/todo.txt` is completely
  real (`FileExists` confirms it, `CreateFile`/`OpenFile` happily accept a nested relative path) and
  is genuinely invisible to a root-level `GetFileNames()` call; only a caller that already knows the
  subdirectory's name can list what's inside it. **Second finding**: `DeleteDirectory` throws on a
  non-empty directory (confirmed live, caught and reported on screen) — it is not a recursive delete,
  unlike `StorageDevice::DeleteContainer` below.
- **Container Lifetime** (new) — `Dispose()`/`IsDisposed`/`Disposing`, reopening the same container
  name while an old handle is still alive, and `StorageDevice::DeleteContainer`. **Real findings, all
  verified live, not assumed from the header**: (1) `Disposing` is genuinely idempotent — a second
  `Dispose()` call does not re-raise it, confirmed by an event-subscriber counter staying at 1. (2)
  **`Dispose()` does not gate anything** — no method in `StorageContainer` checks `IsDisposed` before
  running, so `FileExists()` (and every other operation) on an already-disposed handle keeps working
  exactly as before; "disposed" here means only "the `Disposing` event has fired," not "this object is
  now unusable." (3) Reopening is not tracked at all: `StorageDevice::EndOpenContainer` hands out a
  brand-new `StorageContainer` every time with no notion of "already open" — two independent live
  handles over the same on-disk directory coexist fine, and what one writes, the other immediately
  sees. (4) `StorageDevice::DeleteContainer` removes the **entire** tree in one call
  (`fs::remove_all` under the hood) with no "must be empty" restriction, unlike `DeleteDirectory`
  above — confirmed by opening a third, brand-new handle after deletion and finding the marker file
  gone.

**Persistence is verified across processes, not just within one.** The demo recovers its counter
by parsing the file it wrote, precisely so that a restart continues rather than resetting — an
in-memory counter would look identical on screen and prove nothing. Checked by running the
binary three times: the first wrote `save #1`, a second, separate process read it back and wrote
`save #2`, and the delete path removed the file. Saves land under the real per-OS root
(`~/.local/share/cna-examples/StorageDemo/Player1/` here), never in the repository. The two new
screens use their own dedicated container names (`StorageDemoDirs`, `StorageDemoLifetime`) so none
of the four Storage screens can leave state that confuses another.

`StorageContainer::ResolvePath` is private, so an application cannot ask where a file physically
went — deliberately, since the sandbox is the point. The screens show the storage root instead
and say why.

#### C5 Diagnostics — 4 categories, 4 screens — **DONE (reduced scope, see below)**

| Category | Screens |
|---|---|
| Logging (3) | All seven `CNA::Logger` levels into a live in-app log view · `LogCategory` routing + `SetMinimumLevel` filtering · the `*If()` conditional variants and `IsEnabled()` short-circuiting |
| Platform & Build (3) | `CNA::Platform` + `DesktopOS` · which optional features this binary was compiled with (`CNA_DEVICES`, sound, Net, GamerServices) · `CNAException` and the XNA exception family — what throws where |
| Backend & Capabilities (4) | `GraphicsBackendType` — which backend this binary is · all 8 `GraphicsCapability` values queried live · the same draw with and without a capability check · `GraphicsProfile` Reach vs HiDef and what this backend enforces |
| Adapter & Display (3) | `GraphicsAdapter::Adapters`/`DefaultAdapter`, descriptions · `CurrentDisplayMode` + `SupportedDisplayModes` per `SurfaceFormat` · live `PresentationParameters` + `GraphicsDeviceStatus` |

**Shipped as 4 screens, one per category, not 13.** Each category's material collapsed naturally
into a single screen; splitting further would have repeated the same query with different
framing.

**`Graphics Capabilities` is the screen Phase F3 depends on.** It queries all eight
`GraphicsCapability` values live. A build against `SDL_RENDERER` will show `ThreeD` unsupported
there, which is exactly the signal the catalog needs in order to gate its 3D demos instead of
crashing.

**A claim was corrected during verification.** The Logger screen originally said its output
"appears on the terminal", with a column reporting which levels "reached SDL". Measuring stderr
showed only four of six lines arriving at startup. The cause is that there are **two** filters:
CNA keeps its own minimum level, and SDL keeps a separate per-category priority. CNA pushes its
level into SDL only inside `SetMinimumLevel` (via `SDL_SetLogPriorities`), so before the first
such call SDL's own defaults still drop Debug and Trace even though CNA passes them. Confirmed by
measurement — 4 lines before, 6 after — and the screen now states this rather than the simpler
untruth.

**Two API constraints found and reported honestly rather than worked around:**
`CNA::Logger::IsEnabled` and `Logger::ToString` are both private, so an application cannot ask
whether a level would survive the filter, nor get a level's name. The screen compares against the
public `GetMinimumLevel` and carries its own name table, and says why. `Logger::Log` also takes
`(level, message, category)` and `WarnIf` takes `(message, condition)` — message first in both,
which is the reverse of what the names suggest.

### Phase D — Deepen existing Areas

**38 screens.**

| # | Area | Addition | Screens |
|---|---|---|---:|
| D1 | Audio | **XACT** category: `AudioEngine` (runtime-generated `.xgs`) · `SoundBank` cue playback · `WaveBank` in-memory vs streaming · `AudioCategory` volume/pause · cue variables and RPCs · XACT error paths | 6 |
| D2 | 3D Graphics | **PbrEffect** category (Effects Gallery group): metallic-roughness grid, verified live by pixel probe after a polymorphic-vertex `sizeof()` bug was found and fixed, plus an honest `RenderPipelineSettings` follow-up screen (real, faithful store; read by no backend at all) and a `PbrMaterial` finding folded into the main screen (real, faithful store; bypassed entirely by `PbrEffect` and CNA's own glTF loader, which both set `PbrEffect`'s own texture/factor properties directly instead) — see the D2 result section for the full account. | 2 |
| D3 | 3D Graphics | **Model Content** category: real `Model` via `ContentManager` · `ModelMesh`/`ModelMeshPart`/`EffectMaterial` traversal and effect swapping · `SkinnedModelEXT` · `AnimationPlayer` clip playback · `MorphTargetEXT` | 4 |
| D4 | 3D Graphics | **Effect Reflection** category: enumerate and set `EffectParameter`s live · techniques/passes + `CurrentTechnique` switching · `EffectAnnotation` · `Effect::Clone` independence | 4 |
| D5 | 3D Graphics | **Textures & Queries** category: `Texture3D` volume + slice/box `SetData` · `TextureCube` faces and `CubeMapFace` · `RenderTargetCube` rendered per face and used as an env map · `OcclusionQuery` occluded vs visible `PixelCount` · the documented "SurfaceFormat is ignored, everything is RGBA8" caveat, shown honestly | 5 |
| D6 | 2D Graphics | **Formats & Device Events** group — *Surface Formats* (every `SurfaceFormat` attempted against this backend · DXT1/3/5 · `Texture2D::FromStream` on PNG/JPG/BMP) and *Device Events* (MSAA on/off · `PresentInterval` with measured frame rate · back-buffer resize → device reset → resource survival) | 6 |
| D7 | Input | Gamepad gyro/accelerometer/light-bar/trigger-rumble EXT · `TouchPanel` display width/height/orientation · the four APIs the `feature/input` merge exposed (see A2 result): `TouchCollection::FindById`'s Invalid sentinel, `NO_FINGER`, `GamePadButtons::FromButtonArray` | 3 |
| D8 | Net | `QualityOfService` · the full `SendDataOptions` matrix · simulated network conditions | 3 |

#### D1 XACT — 1 category, 2 screens — **DONE (reduced scope, see below)**

| Category | Screens |
|---|---|
| XACT (2) | Engine & Banks — the three authored files and the construction order · Cues & Categories — the seven-flag cue state machine and `AudioCategory` |

**Shipped as 2 screens, not 6.** XACT's substance is the two ideas above; cue variables/RPCs and
the error paths are variations that would have repeated the same calls without adding a concept.

**The banks are generated at runtime, and no Ms-PL source entered this repository.** XACT needs
three real binary files (`.xgs`/`.xsb`/`.xwb`), CNA reads that format without ever writing it, and
no XACT authoring tool exists on Linux — so unlike every other asset here they cannot be produced
locally. `../cna` already solved this for its own `demo_xact` program with a dependency-free
generator; `cmake/ExamplesHelpers.cmake` puts that header on the *include path* and never copies
it, because it is Ms-PL and this repo is MIT. Compiling against it is no different from linking
CNA itself. Guarded on existence: without `../cna` the screens build and explain their own
absence. Output goes to `/tmp/cna-examples-xact/`, never into the checked-out tree — the same rule
Media/Pictures follows. Measured: `Demo.xgs` 136 B, `Sounds.xsb` 318 B, `Waves.xwb` 282 484 B.

**The seven cue flags are not seven independent booleans**, which is the one thing a state grid
invites you to get wrong. Six of them (`IsCreated`, `IsPreparing`, `IsPrepared`, `IsPlaying`,
`IsStopping`, `IsStopped`) are a single mutually-exclusive state value — exactly one is ever true —
so a cue straight out of `SoundBank::GetCue` reads `IsPrepared` and **never** `IsCreated`. This
plan originally assumed otherwise; `tools/checks/xact_claims.cpp` caught it. `IsPaused` is the
exception: real FACT only sets and clears a PAUSED bit and never touches PLAYING, so a paused cue
stays `IsPlaying == true`. Both facts are now asserted and stated on screen.

**Verified by assertion, not by screenshot.** Both screens catch their own exceptions and print
them, so a total failure to open the engine would still render a clean screenshot and pass a
sweep — the trap that hid two real bugs in the XNB screen. `tools/checks/xact_claims.cpp` asserts
all 24 on-screen claims directly against CNA: bank sizes, open order, `ContentVersion == 46`,
`SpeedOfSound == 343` out of the `.xgs`, all four cues resolving by authored name, the full
state machine, category volume, and the rejection paths for unauthored cue/category/variable names.

**Supporting harness fix.** `--keys` gained `left` and `right`, and a scripted action now also
reports itself through `IsNewKeyPress` as the key it stands for. Left/Right previously had no menu
equivalent, so *no* demo binding them — several in Audio and Input — was reachable from the
harness at all. The cue screen uses them for pause/resume and stop.

#### D5 Textures & Queries — 1 category, 4 screens — **DONE**

| Category | Screens |
|---|---|
| Volume & Cube Textures (4) | `Texture3D` volume + sub-box · `TextureCube` six faces · `RenderTargetCube` rendered per face and used as an env map · `OcclusionQuery` occluded vs visible |

Registered as a new **Textures & Queries** group in the 3D Graphics area, wrapped in
`Requiring(CNA::GraphicsCapability::ThreeD, ...)` like every other 3D category.

**Every screen verifies its own claim and shows the verdict as a coloured swatch**, so the sweep
asserts correctness by pixel rather than by "it rendered". This was the direct lesson of the XNB
and XACT screens: a screen that catches its own exceptions looks identical whether it worked or
not. Measured: the Texture3D round trip checks all 8 192 voxels; the blue channel steps exactly
32 per slice and the sub-box lands exactly on Z 3–5, both confirmed by sampling the screenshot.
`TextureCube` verifies all six faces. `OcclusionQuery` confirms the occluded count is strictly
lower than the unoccluded one — occlusion queries genuinely work on EASYGL.

**`RenderTargetCube::GetData` silently returns zeros on EASYGL — this is a real finding.**
`ITextureCubeBackend::GetData` is declared a no-op by default and EasyGL's render-target cube
backend overrides only `SetData`. `TextureCube::GetData` still runs its full validation, allocates
a **zero-initialised** staging buffer, calls the backend (which does nothing), and copies that
buffer to the caller. The result is 32×32 transparent-black texels, no exception, no error code —
plausible-looking data that is silently wrong. The first version of this screen assumed a working
round trip and reported FAIL; the second assumed the caller's buffer was left untouched, which was
also wrong. Only a sentinel-fill probe distinguished the three cases. The screen now classifies
the outcome (real pixels / silently zeroed / untouched / threw) and reports which actually
happened, so it stays honest if a backend later implements the readback.

Because the readback is unavailable, that screen proves the six faces really differ the way a game
would actually use them: the cube is fed to `EnvironmentMapEffect` as a reflection source, and the
reflected hue measurably changes as the object spins (green → dark green → blue across frames).

**Not done in D5:** the planned "SurfaceFormat is ignored, everything is RGBA8" screen. That is a
claim about backend behaviour rather than an API demonstration, and it belongs with D6's surface
format work where it can be shown across every format at once.

#### D2 PBR — **DONE 2026-07-28, 1 category, 2 screens** (was BLOCKED `needs_human`)

A `PbrEffect` metallic/roughness grid was written, **reverted** because it never rendered geometry,
diagnosed in a follow-up investigation pass, and then successfully fixed and shipped once the real
cause was known. Kept as a case study because the whole arc -- write, revert, diagnose from source
reading alone, fix, verify by measurement -- is a genuine instance of this project's own discipline
working as intended across three separate sessions.

**Root cause (confirmed live, not just source-read): `VertexPositionNormalTangentTexture` is
polymorphic.** It inherits `IVertexType`, which declares a virtual destructor, so the struct carries
a hidden 8-byte vtable pointer and its real `sizeof()` is 56, not the naive 48 = 12+12+16+8 the
reverted screen assumed when calling `SetDataRaw(..., sizeof(VertexPositionNormalTangentTexture))`.
`static_assert(sizeof(VertexPositionNormalTangentTexture) == 48)` fails to compile, confirming it
directly. This is the same vtable-inflation bug class already documented elsewhere in CNA
(`VertexPositionColor` is 40 bytes not 16, `VertexPositionNormalTexture` is 40 not 32) -- PBR's
tangent vertex type just has no typed `VertexBuffer::SetData` overload to hide it via repacking,
unlike those two.

**The fix**, shipped in `src/Demos/Graphics3D/PbrEffect/PbrMetallicRoughnessScreen.hpp`: define a
private, non-polymorphic, tightly-packed `GpuVertex` POD (`static_assert(sizeof(GpuVertex) == 48)`,
mirroring `../cna/examples/easygl_pbreffect_golden_test.cpp`'s own `PbrGpuVertex` field-for-field),
repack `BuildSphereTangentMesh()`'s output into it, and upload *that* via `SetDataRaw(..., 48)` --
never the polymorphic `VertexPositionNormalTangentTexture` array's raw bytes directly. Once the
stride was honest, the geometry rendered correctly on the first try; no other change was needed
(the WVP pipeline, texture binding, and lighting setup were already correct, as the intermediate
investigation pass established).

**Verified live, not assumed:** the screen probes `GetBackBufferData` at the centre grid cell (which
sits exactly at the camera's look-at target with this layout) against a background reference point
above the grid, and shows a green/red verdict swatch based on a real measured colour difference --
the same route `tools/headless.sh --screenshot` itself uses. Confirmed by screenshot: five columns
of increasing roughness × three rows of increasing metallic, the bottom (fully metallic) row
visibly darker where it has no diffuse floor to fall back to, exactly as the metallic-roughness BRDF
predicts.

Registered as a new **PbrEffect** category inside the existing **Effects Gallery** group (3D
Graphics area), alongside AlphaTestEffect/DualTextureEffect/EnvironmentMapEffect/SkinnedEffect/
CustomShader -- it's a single-technique effect showcase like its siblings there, not a new group.

**Second screen, shipped as a same-day follow-up: `RenderPipelineSettingsScreen.hpp`.** D2's
original ~5-screen projection paired PbrEffect with an honest screen for the `RenderPipelineSettings`
bag; deferred at the time to keep the rendering-defect fix focused, then picked up right after.
Re-verified the premise from scratch rather than trusting the old investigation: grepped the entire
`../cna` source tree fresh and confirmed nothing outside `RenderPipelineSettings`' own `.cpp`
reads it -- no `GraphicsDevice`, no backend, no effect. The type lives behind `CNA_NOXNA`
(`../cna/include/CNA/Graphics/RenderPipelineSettings.hpp`), default `OFF` in CNA for the same reason
`CNA_DEVICES` is -- a NOXNA extension beyond XNA 4.0 -- so this project's own `CMakeLists.txt` now
force-enables it too, mirroring the existing `CNA_DEVICES` precedent exactly (small, self-contained,
7-file surface: `RenderPipelineSettings` + `PbrMaterial` + 3 small enum headers; no dependency on
any other subsystem; full rebuild confirmed no regressions). The screen sets every property to a
deliberately non-default value and reads every one back -- verified live, a faithful, exact round
trip -- then states plainly that changing any of it has zero visible rendering effect, with an
amber (not green/red) verdict swatch in the same honest spirit as D8 Net's amber verdict: this is
not a pass/fail on correctness, it's a plain statement of what is and isn't wired up.

**Follow-up investigation, same day: `PbrMaterial` exists but is the same "unread store" situation,
not new demo substance.** The prior note flagged `../cna/include/CNA/Graphics/PbrMaterial.hpp` as
contradicting this project's old "there is no `PbrMaterial` type" claim -- that claim was indeed
stale, but the type itself changes nothing about scope. `PbrMaterial` is a real, non-stub glTF-style
settings bag (5 texture slots + 6 scalar factors), but grepping all of `../cna/src`, `../cna/include`,
`../cna/examples/` and `../cna/tests/` for its use turns up exactly one constructor call outside its
own `.cpp`: `../cna/examples/noxna_settings_example.cpp`'s own round-trip test (the same file that
tests `RenderPipelineSettings`). Neither `PbrEffect` nor CNA's real glTF content pipeline ever
constructs or reads a `PbrMaterial` -- `RuntimeGltfModelTests.cpp`'s
`LoadsPbrMaterialWithAllFourMapsAndFactorsFromGltf` test (whose NAME suggested otherwise) actually
asserts against `PbrEffect`'s own texture/factor properties directly
(`getTextureProperty`/`getMetallicFactorProperty`/etc.), the exact same properties
`PbrMetallicRoughnessScreen.hpp` already sets -- `PbrMaterial` is bypassed entirely by the one real
consumer that might have used it. Rather than ship a third, thin screen for a second instance of the
same non-finding, this is folded into `PbrMetallicRoughnessScreen.hpp` itself: a `PbrMaterial` round
trip is verified live in `OnDemoLoad` and folded into the screen's existing verdict, with one on-screen
line stating the finding. No new screen registered; count stays at **249**. Verified: 249/249 on
EASYGL, `check_catalog.py`/`check_layout.py`/`check_shots.py` all clean; a text-layout collision from
the added on-screen line (spheres partly hidden behind text) was caught by eye and fixed by trimming
the surrounding lines rather than by widening the scene viewport.

**Historical record kept for what it teaches:** two real API facts came out of the original attempt
and hold regardless of the fix above: tangent vertex types have **no** typed
`DrawUserIndexedPrimitives` overload, so an untyped array silently binds the `const void*` overload
and draws garbage (or, as it turned out, nothing) rather than failing; and
`VertexPositionNormalTangentTexture::Tangent` is a `Vector4` whose W carries glTF bitangent
handedness. `docs/wip-pbr/` (the original reverted screen and the sphere-mesh patch) is now
superseded by the real shipped screen and has been removed from the repo -- its content lives on
here and in `NEXT.md`'s D2 history.

#### D3 Model Content — 4 screens added to the existing Model category — **DONE (reduced scope, see below)**

| Screen | Point |
|---|---|
| Load & Traverse | Real `ContentManager.Load<Model>()` from a synthesized `.cnj` fixture, traversed via `Bones`/`Meshes`/`MeshParts` |
| EffectMaterial & Effect Swapping | `EffectMaterial` verified standalone (never built by either content reader); live `ModelMeshPart::setEffectProperty()` swap |
| Skeletal Animation (AnimationPlayer) | A real `SkinningData` `Tag`, `AnimationPlayer`, two `SkinnedEffect`s driven by `SetBoneTransforms()` |
| Morph Targets (MorphTargetEXT) | A real `MorphTargetDataEXT`, hand-evaluated `MorphWeightTrackEXT`, `BlendMorphTargetsEXT`/`SetMorphWeightsEXT` |

**Everything is synthesized procedurally at `OnDemoLoad()` into a temp directory** (`.cnj` +
`.skeleton.bin`/`.clip.bin`/morph binary sidecars, plus 2x2 QOI textures), the same technique
`../cna`'s own `easygl_model_skinned_animation_playback_test.cpp` golden test uses to build its
fixture in-process. This turned out to make the originally-planned XNB-fixture borrow (`../cna`'s
`BlenderDefaultCube.xnb`) unnecessary: `ContentManager`'s own `.cnj` `ModelTypeReader` is a
first-class, fully-documented CNA content format, not a MonoGame-compiled binary, so nothing needs
borrowing and nothing Ms-PL is at risk of entering this repo's history. Added to the existing
**Model** category (not a new one) since `ModelGroup/ProceduralModelScreen.hpp` and
`ModelBoneHierarchyScreen.hpp` already occupy that slot and the four new screens are a natural
continuation of the same idea.

**`SkinnedModelEXT` dropped from this category — it belongs to Phase E, not here.** Its own doc
comment states it plainly: "used by `AvatarRenderer::EnableRealRenderingEXT`... deliberately not
built on `Model`/`ModelBone`/`ModelMesh`." It is Avatar-specific infrastructure, disjoint from the
general `Model` content path. The general-purpose equivalent — skeletal animation for a real
`Model` via `SkinningData`/`AnimationPlayer` — is exactly what the Skeletal Animation screen
demonstrates instead, so nothing planned is actually missing; the plan's own line item was
mis-scoped, not the coverage.

**A wrong assumption was caught by empirical verification, not by reasoning about source code
alone** — worth recording since it is exactly the trap this project's own notes warn about
repeatedly. Reading `ContentManager.cpp`'s `ModelTypeReader` in isolation suggested that a
`ModelMeshPart`'s effect is never registered into its parent `ModelMesh`'s own `Effects`
collection (the collection `Model::Draw()` pushes `World`/`View`/`Projection` into) — i.e. that
every content-loaded model's camera matrices would silently be no-ops. A standalone debug binary
linked directly against `../cna`'s `libCNA.a` (mirroring the golden tests' own approach) proved
this **false**: `Effects.Count == 1` immediately after `Load<Model>()`, with zero extra code. The
real mechanism, found by then reading `ModelMeshPart.cpp` directly rather than re-guessing:
`ModelMeshPart::setEffectProperty()` self-maintains its parent mesh's `Effects` collection (`Add()`
on set, ref-counted `Remove()` on change — only if no other part in the mesh still shares the old
effect), but **only once the part already has a parent**. `ModelTypeReader` constructs the
`ModelMesh` first, then calls `setEffectProperty()` on its parts, so the sync fires correctly.
`ModelGroup/ProceduralModelScreen.hpp` calls `setEffectProperty()` **before** constructing its
owning `ModelMesh` (parent still null), so its sync is silently skipped — which is exactly why that
screen's own `OnDemoLoad()` needs a manual `mesh_->getEffectsPropertyMutable().Add()` afterward.
Both screens are correct; the API is just silently construction-order-dependent, with no error
either way. The **Load & Traverse** screen states and verifies this live.

**`EffectMaterial` is a second, independent finding.** Its own doc comment claims it is "created
internally by the content pipeline; games do not instantiate it directly." Neither of CNA's two
Model readers agrees — the `.cnj` `ModelTypeReader` and the `.xnb` `ModelContentTypeReaders.cpp`
path both always construct a real stock effect (`BasicEffect`/`SkinnedEffect`/`DualTextureEffect`/
`PbrEffect`/`SkinnedPbrEffect`) directly into a part's `Effect` slot; grepping both files for any
`EffectMaterial` construction finds none. The only place it is ever built in this codebase at all
is its own unit test. It is fully constructible and clonable standalone (verified the same way that
test does), but reading `EffectMaterial.cpp` directly shows `OnApply()` is an empty function body —
applying one, even a successfully-constructed one, would bind no parameters and change no GPU
state. Stated from source, not attempted live: drawing with a deliberately no-op shader application
is not something to try against a real GL context.

**The morph-target screen caught a bug in its own first draft**, also by verification rather than
by trusting the arithmetic: the test initially asserted vertex 0's absolute Y was `0.0` at weight 0
and `+0.5` at weight 1, which is wrong — `BuildQuadNormalTextureMesh`'s own vertex order puts
vertex 0 at the quad's bottom edge (`Y = -0.35` in the bind pose), not the origin. The screenshot
showed a `FAIL`, which is what caught it. Fixed to check the **delta** between the two samples
(`+0.5` exactly, matching the authored target regardless of the base value), which is both correct
and the more robust form of the check anyway.

#### D4 Effect Reflection — 1 category, 3 screens — **DONE (reduced scope, see below)**

| Category | Screens |
|---|---|
| Effect Reflection (3) | What Is Actually Exposed · Parameter Classes, Types & Values · Clone: What Is and Is Not Copied |

**Shipped as 3 screens, not 4.** The planned "techniques/passes + `CurrentTechnique` switching"
screen has nothing to switch between: `Effect`'s constructor adds exactly one technique, named
`"Default"`, and CNA's stock effects add no more. That fact is stated on the first screen instead
of being spread over a screen that would have demonstrated a choice of one.

**The headline finding: CNA's built-in effects populate none of the reflection API.** Measured
live from a `BasicEffect` — `Parameters.Count` is 0, `Techniques.Count` is 1, and
`Parameters["World"]` returns **nullptr**. In XNA those parameters come out of a compiled `.fxb`,
so the classic port line

    effect.Parameters["WorldViewProj"]->SetValue(m);

compiles here and dereferences a null pointer at runtime. The lookup does not throw — it returns
a pointer, and callers must check it. CNA's effects keep their state in typed C++ fields and
property setters (`World`, `VertexColorEnabled`, `setDiffuseColorProperty`) instead. The API is
not a stub: `EffectParameter` is fully functional standalone, and anything building an effect can
`Add()` parameters of its own.

**`SetValue(Matrix)` vs `SetValueTranspose(Matrix)` is a silent trap.** They store the same matrix
transposed relative to each other, and each has its own getter. A mismatched pair returns the
transpose with no error. The screen proves it with a deliberately asymmetric matrix — a symmetric
one would hide the bug entirely.

**`Effect::Clone()` returns a raw owning pointer**, a documented deviation (FNA returns a
GC-managed `Effect`; C++ has no equivalent), so forgetting to delete it leaks silently. Verified
live that cloned state is copied and then independent, and that the clone gets its own technique
collection. `ShaderEffect` is the documented exception to the sharing rule: it uniquely owns a
compiled program, so its `Clone()` recompiles the GLSL rather than sharing — cloning one in a loop
is not free.

**Layout fix this phase forced.** All three screens initially drew their verdict swatch past
`LabelBaselineLimit()` — 538px in a 640px window — because the swatch position was computed from
wherever `DrawLines` happened to finish. `DemoScreen::DrawVerdict()` now clamps it, and the three
screens were trimmed to fit naturally so the clamp stays a safety net rather than the normal path.
`tools/check_layout.py` did not catch this: it only inspects literal draw positions in source, and
these were computed.

#### D7 Input EXT — 2 screens added to existing categories — **DONE (reduced scope)**

Added to **Gamepad** and **Other** rather than creating a category: these fill gaps in areas that
already exist. Checked the existing 50 Input screens first, which changed the scope — `GetPowerInfoEXT`,
`GetGUIDEXT`, `GetPlayerIndexEXT` and `SetTriggerVibrationEXT` were **already covered**, so the
planned "gamepad EXT" screen would have largely duplicated them. The genuine gaps were `GetGyroEXT`,
`SetLightBarEXT`, and the three `feature/input` APIs, none of which appeared anywhere in the tree.

| Screen | Category | Point |
|---|---|---|
| Motion Sensors & Light Bar | Gamepad | The EXT calls' failure modes differ: the sensor getters return **bool** and fill their out-parameter only when true, so reading it unchecked yields stale data; `SetLightBarEXT` returns **void** and is a silent no-op on a pad without a bar or with no pad at all — write-only hardware with no feedback channel. |
| Sentinels & Factories | Other | `TouchCollection::FindById` returns bool **and writes a sentinel** on failure — ignoring the return value still hands you a `TouchLocation`, one whose state is `Invalid`. Plus `TouchPanel::NO_FINGER` and `GamePadButtons::FromButtonArray`. |

The second screen needs no touchscreen or controller — it builds its own `TouchCollection` — which
is precisely why it earns a screen: these are the parts that cannot be checked by waving hardware
at the app. All 8 of its claims are verified live and reported by a swatch.

#### D8 Net — 1 screen — **DONE (reduced scope)**

`QualityOfService` was already read on the Discover & Join screen and the full `SendDataOptions`
matrix already appears across nine existing Net screens, so the genuine gap was simulated network
conditions — added to **NetworkSession** as *Simulated Latency & Packet Loss*.

**The finding: CNA really implements these, and FNA does not.** In FNA `SimulatedLatency` and
`SimulatedPacketLoss` are plain, inert auto-properties — settable, readable, consumed by nothing
anywhere in its stubbed-out source. CNA's `ENetBackend` holds delayed AppData in a per-session
delivery queue and drops packets at exactly the configured rate. Netcode "tested" against FNA's
versions was tested against nothing. Both are deliberately scoped to **AppData only**, so
session-management traffic and a host's relay hop for two other peers are unaffected and 100% loss
does not tear the lobby down; 0.0 and 1.0 are handled deterministically without touching the RNG.

**The screen's verdict is amber, honestly — and, as of a 2026-07-28 follow-up, definitively so, not
just "not yet tried".** The original write-up above guessed the fix was "get two genuinely
signed-in local gamers into the session"; that guess was wrong, and has now been disproven by
reading the source directly rather than guessed at again. `NetworkSession::Update()`
(`NetworkSession.cpp`) gates its entire `PacketSend` delivery path behind
`ENetBackend::RealNetworkingEnabled(sessionType_)`, which is `true` **only** for
`NetworkSessionType::SystemLink` (`ENetBackend.cpp`: `return sessionType == NetworkSessionType::SystemLink;`).
For `NetworkSessionType::Local` — what this screen deliberately uses — every `PacketSend` is an
*unconditional* no-op regardless of local-gamer count: `LocalNetworkGamer::EnqueuePacket` has
exactly one call site in the whole codebase, and it sits behind that same gate. Confirmed by
constructing two real non-guest `SignedInGamer`s via `SignedInGamer::CreateInternal(...)` and the
explicit `NetworkSession::Create(sessionType, vector<SignedInGamer*>, ...)` overload (which bypasses
the guest-filtering that caps the simple `Create(sessionType, maxLocalGamers, maxGamers)` overload
at 1 non-guest gamer, per `GamerRosterScreen.hpp`'s own existing finding) — even with a real second
local gamer wired in, delivery is still architecturally unreachable on `Local`.

**The real (SystemLink) path genuinely works** — proven end-to-end in CNA's own test suite
(`ENetBackendTests.cpp`: `ZeroSimulatedLatencyAndPacketLossDeliverAppDataImmediately`,
`SimulatedPacketLossOfOneDropsAllAppDataDeterministically`) — but reaching it from a single
self-contained demo screen isn't possible through the public XNA API this catalog demonstrates.
`NetworkSession::Find()`/`Join()` (see `DiscoverAndJoinScreen.hpp`) is real UDP LAN broadcast
discovery requiring a **second, separate `cna_examples` process** to answer it — not something one
screen's `OnDemoLoad()` can set up for itself. CNA's own tests get a same-process loopback
connection only by dropping to `CNA::Internal::Net::ENetHostHandle` and hand-encoding
`AppDataMessage` packets directly — internal transport plumbing, not the public
`Microsoft::Xna::Framework::Net` surface this catalog sticks to, so that route was deliberately not
taken here either. **This is now a settled architectural limitation, not an open follow-up**: the
screen's on-screen text and code comments were rewritten 2026-07-28 to state the corrected diagnosis
precisely, so nobody re-attempts the "just add a second local gamer" dead end again.

#### D6 Formats — 2 screens — **DONE (reduced scope, see below)**

*SurfaceFormat Matrix* added to **Texture2D Basics**: all nineteen `SurfaceFormat` values are
constructed for real and the outcome tabulated, because there is no `SupportsFormat()` query — the
only way to find out is to attempt it and see whether it throws.

**Measured on EASYGL: exactly one of the nineteen — `Color` — constructs at all. The other
eighteen throw.** This refutes the assumption carried in D5's notes and this plan's D6 row, that
`SurfaceFormat` is ignored and everything is silently stored as RGBA8. On this backend unsupported
formats are *refused outright*, not quietly coerced. The screen still checks the coercion case (it
writes a Color and reads it back, reporting whether the round trip was exact) because other
backends may behave differently — which is precisely why the table is probed live rather than
hard-coded.

**`FromStream` added as a second screen.** A real PNG off disk through a `FileStream`, plus a 2×2
24-bit BMP *synthesised in memory* — BMP's header is simple enough to hand-write, which proves a
second decoder exists without vendoring another asset. The more useful half is the failure paths:
an image loader is fed untrusted bytes more than almost anything else in a game, so empty streams,
random noise and a truncated PNG (valid 8-byte signature, nothing after it — the nastiest case,
because the magic number says "trust me") are all tried. All three throw; none silently returns a
garbage texture, and the swatch asserts that.

**The Device Events group is dropped: it is already covered.** `Framework/DeviceManager` has
`DeviceEventsScreen` and `VSyncAndMultiSamplingScreen`, which between them do MSAA on/off,
`PresentInterval`/VSync and the DeviceReset event. Adding a D6 "Device Events" group would have
duplicated them. **D6 is therefore complete at 2 screens rather than 6.**

### Phase E — Avatars Area — 3 categories, 7 screens — **DONE**

New top-level Home entry (not a category folded into an existing area), built on real avatar
content borrowed from `../cna/examples/demo_avatar/Content/` at build time via the same
copy-at-configure, guard-on-existence pattern the `.xnb`/XACT fixtures already use (see
`cmake/ExamplesHelpers.cmake` — copied to `Content/AvatarDemo/`, nothing Ms-PL enters this repo's
git history). A missing `../cna` checkout makes every real-rendering screen catch a
`ContentLoadException` and report the absence on screen rather than crash.

| Category | Screens |
|---|---|
| AvatarDescription (2) | `CreateRandom & IsValid` · `Preset & BodyType Name Tables` |
| AvatarRenderer (3) | `The Faithful (No-Op) XNA Surface` · `Real Render (Male & Female)` · `Animation Preset Cycling` |
| Appearance & Wardrobe EXT (2) | `Per-Slot Tinting (AvatarAppearanceEXT)` · `Hot-Swap (AttachPartEXT/RemovePartEXT)` |

**Shipped exactly at plan.md's original 7, following ../cna's own reference programs
(`demo_avatar`, `demo_avatar_wardrobe_hotswap`) for the proven call sequences rather than guessing
from headers alone** — the same lesson every prior phase re-learned. `AvatarDescription` is left
**ungated** (pure C++, no `GraphicsDevice` call), so it still shows on 2D-only backends;
`AvatarRenderer`/`Appearance & Wardrobe EXT` both real-render through `SkinnedEffect` and are
gated on `GraphicsCapability::ThreeD` like every other 3D category. Verified 245/245 on both
EASYGL and SDL_RENDERER, including confirming the gated categories report "Not available on this
build" cleanly on SDL_RENDERER while `AvatarDescription` keeps working there.

**Corrected scope, one real cut, one real addition.**
- `AttachPartEXT`/`RemovePartEXT` are NOT Avatar APIs — they live on `Graphics::SkinnedModelEXT`
  (D3 deliberately left this type alone, judging it Avatar-only; that judgement call is now
  confirmed correct).
- The planned "`AvatarExpression` eye/eyebrow/mouth" screen doesn't exist as such: `AvatarExpression`
  is only consumed by the faithful (non-EXT) `Draw(bones, expression)` overload, which is a
  genuine no-op — there is nothing visual to show. Folded into `The Faithful (No-Op) XNA Surface`
  instead, alongside `Draw()`'s own no-op behavior, `State`'s permanent `Unavailable`, and
  `BindPose`/`ParentBones` — richer coverage of the same idea rather than a screen with nothing to
  demonstrate.
- "All 31 animation presets auto-cycled" as originally planned is **not what the content
  supports**, and the screen says so rather than silently only using 21: a single loaded
  `SkinnedModelEXT` bakes in only its own gender's 10 clips plus the 11 neutral ones (21 total);
  the other gender's 10 preset names are absent from its `Clips` map and throw
  `ArgumentException` from `ComputeBoneTransformsEXT`. Verified directly against all 31 before
  ever attempting a draw, then auto-cycles the 21 that actually exist.

**Findings:**
1. **`AvatarRenderer::getParentBonesProperty()` is NOT empty, contradicting this screen's own
   first-draft assumption** — caught by a live screenshot, not by reading the header alone. The
   constructor initializes `parentBoneIds_` from a real, hardcoded 71-entry table
   (`AvatarRenderer.cpp`'s own anonymous namespace), so it returns all 71 real parent-bone indices
   unconditionally. `BindPose` by contrast IS sized to 71 at construction but stays default/identity
   and is unreachable anyway (`getBindPoseProperty()` always throws `InvalidOperationException`,
   since nothing ever sets `State` to `Ready`) — the two "real skeletal data" getters behave
   differently from each other in a way the doc comments alone don't make obvious.
2. **`SkinnedModelEXT::GetOwnedPartCountForTesting()` (a `NOXNA` testing accessor) makes
   `AttachPartEXT`'s replace-by-name claim independently checkable without a GPU or a pixel probe**:
   the base avatar owns exactly 5 parts (Body/Hair/Pants/Shirt/Shoes), and the Hot-Swap screen
   confirms this stays exactly 5 across every baked-in/Cap/Ponytail swap, proving neither a leak
   nor a drop rather than trusting the header comment.
3. **`AvatarAppearanceEXT` tinting reaches real rendered pixels**, confirmed with a live backbuffer
   probe (`GraphicsDevice::GetBackBufferData`, the same route `tools/headless.sh --screenshot`
   itself uses) at a fixed torso coordinate before and after switching appearances — not just that
   the struct's setters/getters round-trip.
4. **ROOT-CAUSED 2026-07-28 (Phase F1), and much bigger than first thought.** What Phase E recorded
   as "Stand2's head goes invisible" turns out to be one visible symptom of a **systemic content
   defect spanning nearly the entire avatar animation library**, both genders. A standalone
   diagnostic (a throwaway `.cpp` linked directly against `../cna`'s already-built `libCNA.a`,
   loading each real `SkinnedModelEXT` and dumping every clip's raw per-keyframe data) found: for
   roughly **60 (clip, bone) track pairs** — `Stand0`–`Stand7`, `Wave`, `Celebrate`, `Clap`, every
   `Male*`/`Female*` emote, every idle variant — the `Translation` channel is correct **only** on a
   track's first and last keyframe (exactly matching `BindPoseLocal`) and reads as raw `(0,0,0)` on
   **100% of the interior keyframes**. `Stand2`'s head-bone (index 12) track: keys 1–108 all read
   `(0,0,0)` against a bind pose of `(0, 0.100, 0)`; only keys 0 and 109 are correct. `Rotation`/
   `Scale` are unaffected — smooth, continuous, plausible motion throughout. This pulls the
   affected bone toward its parent's origin for nearly the whole clip, snapping back only at the
   very first/last frame; for a leaf bone like the head this reads as "sunk into the torso,
   invisible". **`Stand7`'s own ROOT bone (index 0, bind length 1.0) shows the identical 138/138
   interior-collapse pattern** — the same defect, at whole-body scale, on a different clip.
   `ContentManager::ReadAnimationClipFileEXT` (the `.clip.bin` binary reader,
   `../cna/src/.../ContentManager.cpp`) was read in full and ruled out: three sequential float
   reads per axis, already hardened against a real evaluation-order bug (its own comment cites Task
   11.11). **The defect is upstream in the content itself** — the `.clip.bin` files
   `../cna/tools/avatar_builder/`'s pipeline baked apparently write a real translation only on a
   track's first/last keyframe and zero everywhere between. Per the owner's 2026-07-28 instruction
   (investigate, do not modify `../cna`), no fix was attempted; this is left precisely diagnosed for
   the maintainer, alongside D2/F2 in `NEXT.md` §7. Does not affect this screen's own verified claim
   (the valid/invalid preset split, and `DrawRealEXT` not throwing) — the screen already reports
   honestly rather than masking this.

### Phase F — Verification

| # | Work |
|---|---|
| F1 | **Done, 2026-07-28.** Defect sweep over everything this roadmap added since the last such pass (D1, D3, D4, D5, D6, D7, D8, Phase E, C4's extension — ~27 screens plus supporting infrastructure). Not a re-verification of "does it render" (already 247/247 clean going in) but a skeptical read for behavioral/visual defects that survive a clean automated sweep. Full writeup below. |
| F2 | **BLOCKED on a CNA defect.** `emcmake` configures and **every translation unit compiles for wasm**; two real cna-examples bugs were found and fixed getting there (the web branch linked `SDL3::SDL3-static`, a target that never existed in this scope, and the three Media/Video screens needed a platform gate). The link then fails inside CNA's own archive: `libCNA.a(VideoContentTypeReader.cpp.o)` references `Media::Video`, whose implementation CNA does not build for Emscripten. Any web consumer of CNA hits this. The exact one-line fix location in `../cna/cmake/CnaLibrary.cmake` has since been identified but not applied (belongs in that repo). See `NEXT.md` §6b/§7. |
| F3 | **Done.** The catalog builds and runs against the 2D-only `SDL_RENDERER` backend, with every 3D Graphics category gated on `SupportsCapability(ThreeD)`. **249/249 render on both backends** (re-verified after D2's `RenderPipelineSettings` follow-up and D8's diagnosis correction, 2026-07-28), 249 screenshots each, 0 layout problems. See below. |
| F1b | **Done, 2026-07-28.** Compiler-warnings audit: `-Wall -Wextra` enabled on the `cna_examples` target only (not CNA's own targets). 13 warnings found, all inside this project's own `src/` (zero from `../cna` headers), all fixed: 11 cosmetic `-Wmissing-field-initializers` in `AreaCatalog.hpp`'s `AreaEntry` initializers, 1 `-Wunused-parameter`, and 1 `-Woverloaded-virtual=` (a private helper in `OcclusionQueryScreen` coincidentally named `Draw`, hiding the inherited virtual `DemoScreen::Draw` — renamed to `DrawMesh`). Zero behavior changes; re-verified 249/249 on both backends. See `NEXT.md` §5 item 60. |

Android hardware verification is **not** part of this cycle — see §10.

#### F1 defect sweep — findings

**Two real defects found and fixed, both in cna-examples code (not upstream):**

1. **Layout: `DrawVerdict()`'s caption could overlap `DrawLines()`'s own last line.** Both
   independently clamp to the same `LabelBaselineLimit()`, so a screen whose body text reaches
   exactly that limit gets its verdict caption drawn on top of its last content line instead of
   below it — a gap in the "already fixed" story from item 34 (that fix stops the swatch from
   running *under* the Back hint; it does not reserve room *against* `DrawLines()`'s own last line).
   Hit by `Net/NetworkSession/Simulated Latency & Packet Loss` (13 lines, one over the ~12-line
   budget every other screen in this codebase already respects) — confirmed by screenshot, not
   assumed. Fixed the same way D4's screens were: trimmed to 12 lines, re-verified by screenshot
   showing clean separation. Spot-checked every other `DrawVerdict()` screen at or near the 12-line
   boundary across D1/D3/D4/D5/D6/D7/E/C4 (13 screens checked by direct pixel/screenshot
   inspection) — no other instance found.
2. Nothing else — search reachability, breadcrumbs, and the `apis` API-footer field were all
   confirmed populated correctly for every area added this roadmap (spot-checked D3/Avatars
   registrations in `AreaCatalog.hpp` directly). No TODO/FIXME/HACK/stub/placeholder markers found
   anywhere under the areas in scope. No shared-file regressions found in `DemoScreen.hpp`,
   `Geometry3DHelpers.hpp`, or `AreaCatalog.hpp` (all changes this roadmap were additive).

**One major finding, `needs_human`, upstream in `../cna` — not fixed here:** chasing Phase E's own
"Stand2 unresolved" note (see Phase E section, item 4) turned it from a one-clip curiosity into a
precisely root-caused, systemic defect spanning ~60 `(clip, bone)` pairs across nearly the entire
avatar animation library, both genders — a `.clip.bin`-content-level bug in
`../cna/tools/avatar_builder/`, not a cna-examples bug, not a reader bug (the reader was read in
full and ruled out). Full diagnosis in the Phase E section and `NEXT.md` §5 item 51 / §7. Per the
owner's standing 2026-07-28 instruction, no fix was attempted in `../cna`.

**Re-validated after both fixes:** 247/247 on EASYGL and SDL_RENDERER (full unfiltered sweeps, not
filtered), `check_catalog.py`/`check_layout.py`/`check_shots.py` all clean.

**Addendum, D2 fixed 2026-07-28 (after F1 completed):** F1's own report flagged that the D2 root
cause it inherited from the earlier investigation pass "is applicable inside cna-examples' own
screen code and may be worth revisiting." It was: see the D2 result section above. 248/248
re-verified on both backends after that fix.

### 7.2 Verification harness — findings that shaped it

B5 was built before A1 could be signed off, because there was no way to look at a demo otherwise.
Three things had to be discovered the hard way and are now encoded in the tooling:

1. **An X11 root-window grab of a GL surface returns solid black under Xvfb.** Capturing has to go
   through `GraphicsDevice::GetBackBufferData` + `Texture2D::SaveAsPng` — the same route CNA's own
   pixel tests use. `--screenshot` does that on the final drawn frame, before `Exit()`.
2. **`xvfb-run` alone does not make a run headless.** SDL3 selects the Wayland video driver
   whenever `WAYLAND_DISPLAY` is set and then ignores the `DISPLAY` that `xvfb-run` exports, so
   the window opens on the developer's real desktop while the command still looks virtual.
   `tools/headless.sh` and `tools/sweep.sh` force `SDL_VIDEODRIVER=x11` and unset
   `WAYLAND_DISPLAY`; nothing should invoke the binary headlessly without them.
3. **A stray desktop event can silently corrupt a screenshot.** One sweep captured a demo in the
   wrong mode because a phantom "select" reached the window and toggled it — the screenshot looked
   entirely plausible. Any run with `--frames` now ignores real input devices altogether
   (`InputState::SetScriptedOnly`), so a scripted run depends only on what it scripted.

Shell-level fixes came out of the same pass and benefit every demo, not just Media:

- `DemoScreen::DrawLines` ellipsizes each line to the viewport width. Demos print real filesystem
  paths, exception messages and API names, none of which have a bounded length, and a line running
  off the right edge silently loses the part that usually matters most.
- **The menu font's declared line spacing understates its real glyph extent by ~23px.**
  `menufont.cnj` declares `lineSpacing: 29`, but its glyph descriptors place ink as far as 52px
  below the draw origin (crop offset up to 46 plus source height up to 24). `MeasureString().Y`
  derives its height from `lineSpacing`, so it reports 29 too — measuring does not reveal the
  problem. Every vertical layout in the app was therefore under-reserving space.
  `DemoScreen::GlyphExtent()` now bounds it conservatively at `2 × lineSpacing`, and both
  `DrawLines`' clip and the new `LabelBaselineLimit()` use it.
- Three 3D Graphics demos (`Camera & Projection/Perspective vs. Orthographic`,
  `Buffers/Immediate vs. Buffered Draw`, `Primitive Types/LineList…`) were drawing their scene
  labels straight through the Back hint because of the above. Fixed by clamping the label row to
  `LabelBaselineLimit()`. These were pre-existing defects, found by the first full sweep.
- `DrawLines` stops above the Back hint with an explicit `... (N more lines)` marker rather than
  drawing underneath it. Silent truncation reads as "the screen is complete" when it isn't.
- `2D Graphics/DrawString/SpriteEffects & Newline Quirk` ran one explanatory line past the right
  edge of the window (it used `sb.DrawString` directly, which ellipsizing does not reach). Split
  across three rows through `DrawLines`.

**Sweep result:** 174/174 demos render, 0 layout problems, catalog and docs consistent.

`tools/check_shots.py` flags blank screens, ink touching the right edge, and content spilling into
the chrome. It has one known false positive: a 3D demo whose scene legitimately fills the viewport
down to the bottom edge (`Buffers/VertexDeclaration Stride Gotcha`). Distinguishing that from a
text overflow is done by colour — 75% of its bottom-band ink is coloured geometry, whereas
overflowing chrome text is greyscale.

### 7.1 Projected end state vs. final (updated 2026-07-28 — the roadmap is done bar F2)

This table used to hold a "Now" column that was a mid-session snapshot (right after Phase A's Media
rewrite, before any of the C/D/E build-out) and stayed frozen at that point for the rest of the
document's life even as the catalog kept growing — it is replaced here by the actual final count per
area, run through `tools/check_catalog.py` rather than hand-counted.

| Area | Before Phase A | Final (2026-07-28) | Projected | Delta vs. projected |
|---|---:|---:|---:|---:|
| Input | 50 | 52 | 52 | on target |
| Audio | 10 | 12 | 16 | −4 (XACT shipped 2 of a planned 6, see D1's result section) |
| Devices | 15 | 15 | 15 | on target |
| Net | 14 | 15 | 17 | −2 (D8 shipped 1 of a planned 3, see D8's result section) |
| Media | 11 | 17 | 17 | on target |
| 2D Graphics | 38 | 40 | 44 | −4 (D6 shipped 2 of a planned 6, see D6's result section) |
| 3D Graphics | 30 | 43 | 51 | −8 (D2, D3, D4 and D5 each shipped fewer screens than their own row projected — see each phase's own result section for the exact count and reason; this total delta is not a precise per-phase sum, just the net) |
| Framework | — | 17 | 17 | on target |
| Math | — | 16 | 21 | −5 (built under the pre-2026-07-28 depth-over-breadth default, not retroactively revisited — see NEXT.md §2a) |
| Content | — | 7 | 15 | −8 (same — built under the old default) |
| Storage | — | 4 | 6 | −2 (2 of the original 6 deliberately deferred, then shipped as the 2026-07-28 C4 extension; the other 2 were judged pure variations, see C4's result section) |
| Diagnostics | — | 4 | 13 | −9 (built under the old default) |
| Avatars | — | 7 | 7 | on target |
| **Total** | **168** | **249** | **291** | **−42** |

**Reading the delta column honestly:** every shortfall above is recorded, with a reason, in its own
phase's result section — none is a silent gap. Two different causes produced them, and they are not
equally revisable: Math/Content/Diagnostics were built under the depth-over-breadth default *before*
the 2026-07-28 "push closer to projected counts" instruction (NEXT.md §2a) and were deliberately not
reopened once policy changed, since the owner asked to apply the new bias going forward rather than
retroactively pad already-shipped, already-verified areas. Audio/Net/2D/3D's shortfalls, by
contrast, were verified as *the plan overcounting real distinct ideas* (variations on an already-
demonstrated concept, or APIs that turned out not to exist/not to be reachable) — those would not
close even under the new bias, because adding the extra screens would be padding, not coverage. If
the project owner wants the pre-2026-07-28 areas revisited under the new bias specifically for
coverage-metric reasons, that is a legitimate ask but a new one — nothing here assumes it.

## 8. Project layout

```
cna-examples/
├── plan.md                        This file
├── plan20260727.md                Archived previous plan (the 7-Area build-out record)
├── README.md
├── CMakeLists.txt                 Top-level; sibling add_subdirectory of ../cna
├── cmake/ExamplesHelpers.cmake
├── tools/
│   ├── gen_menu_font.py           Regenerates the menu SpriteFont from a system TTF
│   ├── gen_media_library.sh       A1: regenerates Content/MediaLibraryDemo/
│   ├── headless.sh                B5: run one demo on a virtual display (never the real one)
│   ├── sweep.sh                   B5: screenshot every demo
│   ├── check_shots.py             B5: flag blank / overflowing screenshots
│   └── check_catalog.py           A5: screen files vs catalog vs docs consistency
├── Content/
│   ├── menufont.cnj, menufont_atlas.png, blank.png
│   ├── MediaDemo/                 5 synthetic ffmpeg-generated audio/video assets
│   ├── MediaLibraryDemo/          A1: synthetic music + picture library
│   ├── ContentDemo/               NEW (C3): .xnb fixtures copied from ../cna/tests/assets
│   └── Avatar/                    NEW (E): avatar meshes copied from ../cna/examples/demo_avatar
└── src/
    ├── Program.cpp                CLI parsing + entry point
    ├── CnaExamplesGame.hpp
    ├── Harness/                   B5: CommandLine.hpp (options), DemoIndex.hpp (flat catalog)
    ├── GameStateManagement/       Screen-stack navigation
    ├── Navigation/
    │   ├── HomeScreen.hpp, AreaScreen.hpp, GroupScreen.hpp, CategoryScreen.hpp
    │   ├── SearchScreen.hpp       NEW (B1) -- will reuse Harness/DemoIndex.hpp
    │   └── AreaCatalog.hpp        The Area → Group → Category → Demo registry
    └── Demos/
        ├── DemoScreen.hpp         Shared leaf-screen chrome
        ├── Input/  Audio/  Devices/  Net/  Media/
        ├── Graphics2D/  Graphics3D/
        ├── Framework/             NEW (C1)
        ├── Math/                  NEW (C2)
        ├── Content/               NEW (C3)
        ├── Storage/               NEW (C4)
        ├── Diagnostics/           NEW (C5)
        └── Avatars/               NEW (E)
```

`AreaCatalog.hpp` is already 961 lines and will roughly double. Phase C splits it into one
`Catalog/<Area>Catalog.hpp` per Area, with `AreaCatalog.hpp` reduced to the `BuildAreaCatalog()`
assembly — otherwise every new Area touches the same file and every merge conflicts.

## 9. Build system

- CMake ≥ 3.20, C++23.
- Consumes CNA via `add_subdirectory(../cna CNA_BUILD)`, which transitively pulls in
  `../sharp-runtime`.
- Defaults `CNA_GRAPHICS_BACKEND` to `EASYGL`; forces `CNA_DEVICES=ON`.
- One executable, `cna_examples`, linked against `CNA`, `CNA_Net`, `CNA_GamerServices` and
  `SHARP_RUNTIME`.
- Per the openeggbert build rules: build into the fixed in-repo `build/` directory, always with
  `-DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache`, and always with an
  explicit `-j4` ceiling. Never in a per-session scratchpad.

New Content is generated or copied at configure time, never hand-authored as opaque binary:

- `Content/ContentDemo/` — copied from `../cna/tests/assets/xnb/`, which holds real
  MonoGame-produced `.xnb` files. CNA consumes `.xnb` and never writes it, so these cannot be
  generated locally.
- `Content/Avatar/` — copied from `../cna/examples/demo_avatar/Content/`.
- XACT banks — generated at runtime by the demo itself (§6.3), so nothing is bundled.

## 10. Out of scope

- **Android hardware verification.** CNA supports Android and the app should build for it, but no
  device pass is planned this cycle. The mobile-only Sensors/Vibration screens remain
  desktop-unverified, as `plan20260727.md` already recorded.
- **Remaining hardware gaps carried over from the previous plan:** real gamepad, touchscreen, raw
  joystick, haptic device, physical webcam. Those screens handle absence gracefully and say so.
- **A genuine SystemLink round trip** for Net — investigated 2026-07-28, definitively NOT
  achievable within this catalog's single-screen/single-process demo model, and this is a hard
  architectural fact rather than an Xvfb/input-routing limitation (that old concern is moot: B5's
  scripted `--frames` path needs no real interactive input, so it was never actually the blocker).
  `NetworkSession::Create()`/`BeginCreate()` gates on a single process-wide `activeSession_` — only
  one real `NetworkSession` can be alive in one process at a time. This is confirmed as **"a real,
  preserved FNA constraint"** by CNA's own test suite
  (`../cna/tests/CNA/Internal/Net/ENetBackendTests.cpp:46-52`), so it is not something this project
  could ask to change even if `../cna` were in scope this session. CNA's own tests work around it by
  pairing one real `NetworkSession` with a raw internal `ENetHostHandle` standing in for "the other
  machine" (`SystemLinkSessionFixture`, same file) — genuine wire-level UDP traffic, but the "far
  side" is CNA's own internal implementation type, not the public `Microsoft::Xna::Framework::Net`
  API surface this catalog exists to demonstrate, so that pattern is not appropriate to reuse in a
  demo screen. A real two-peer round trip therefore needs two actual OS processes, which is a
  different mechanism than every other screen in this 249-screen catalog and was judged out of
  proportion to add for one Net screen. Confirmed out of scope for good, not "maybe later."
- **Compiled XNA `.fx` bytecode.** Confirmed to always throw in CNA; tracked there, not a
  demo-content gap. C3's Errors category demonstrates the throw.
- **macOS, iOS and console targets** — gated on CNA's own platform support.
- **Multi-backend CI.** F1–F3 are manual/local verification passes, not automated gates.

## 11. Relationship to existing repos

- **`../cna`** — the framework being demonstrated. Consumed as a sibling `add_subdirectory`,
  never vendored. This app tracks whatever branch `../cna` has checked out (`develop` by
  default). §2.1 exists precisely because that tracking is not automatic: `../cna` moved and this
  app's prose did not.
- **`../sharp-runtime`** — pulled in transitively through `../cna`.
- **`../cna-samples`** — reference only, for CMake structure and the `GameStateManagement`
  pattern adapted into the shell. No build or runtime dependency.
- **`../cna/examples/`** — source of the XACT bank generator and the avatar assets (§6.3), and
  the place to look for a working reference when a demo here needs to exercise something exotic.
