# NEXT — short-term continuity for cna-examples

**Updated:** 2026-07-27 (end of the autonomous session)
**Branch:** `feature/examples-phase-bcde`, 7 commits ahead of `develop` @ `d7353e3`, all pushed.
The working tree is clean and both build trees are green.
**Authoritative plan:** [`plan.md`](plan.md). Historical record: [`plan20260727.md`](plan20260727.md).

This file is the continuity document: what just happened, what is in flight, what is
blocked, and exactly where to resume. `plan.md` holds the roadmap; this file holds the
state.

---

## 1. Where things stand

| | |
|---|---|
| Demo screens | **234** across 12 areas, 75 categories |
| Last full validation | 222/222 on EASYGL **and** SDL_RENDERER, 0 layout problems, catalog+layout+docs clean |
| Phase A (correct what exists) | **Done** — see plan.md §7 |
| Phase B (navigation shell) | **Done** — search, drag-scroll, breadcrumbs, API footer |
| Phase C1 (Framework area) | **Done** — 5 categories, 17 screens |
| Phase C2 (Math area) | **Done** — 5 categories, 16 screens |
| Phase C3 (Content area) | **Done** — 5 categories, 7 screens (CNJ category since added) |
| Phase C4 (Storage area) | **Done, reduced scope** — 2 categories, 2 screens |
| Phase C5 (Diagnostics area) | **Done, reduced scope** — 4 categories, 4 screens |
| Phase F3 (SDL_RENDERER pass) | **Done** — 222/222 on both backends, 3D gated on ThreeD |
| Phase D1 (Audio/XACT) | **Done, reduced scope** — 1 category, 2 screens |
| Phase D5 (3D Textures & Queries) | **Done** — 1 category, 4 screens |
| Phase D4 (Effect Reflection) | **Done, reduced scope** — 1 category, 3 screens |
| Phase D2 (PBR) | **Blocked** — see §7, `needs_human` |
| Phase D7 (Input EXT) | **Done, reduced scope** — 2 screens into existing categories |
| Phase D8 (Net) | **Done, reduced scope** — 1 screen; verdict amber, see #39 |
| Phase D6 (2D formats/events) | **Done, reduced scope** — 2 screens; Device Events already existed |
| Phases D3, E, F1, F2 | Not started |

`develop` is stable at `d7353e3` and is not being touched this session.

## 2. Session decisions (answered by the project owner, 2026-07-27)

1. **No third-party assets are vendored into this repo.** Both the Ms-PL MonoGame `.xnb`
   fixtures (Phase C3) and the avatar meshes (Phase E) are copied from `../cna` at CMake
   configure time instead. `../cna` is already a hard build dependency
   (`add_subdirectory`), so this adds nothing new, and `cna-examples` stays cleanly MIT.
   Demos whose fixtures are absent must say so on screen rather than crash.
   - Additional reason beyond licensing: `FontCalibri14.xnb` embeds a rasterised
     **Calibri** glyph atlas, a proprietary Microsoft typeface. It is not redistributed
     here under any option.
2. **Work happens on `feature/examples-phase-bcde`**, pushed continuously. `develop` is
   left stable for review/merge by the owner.
3. **Depth over breadth.** A finished area passes `tools/sweep.sh`,
   `tools/check_shots.py` and `tools/check_catalog.py` and is committed before the next
   one starts. Half-finished areas are not left lying around.

## 3. Assumptions made without asking

- **B4 (`apis` on `DemoEntry`)** is populated per area as each area is built or touched,
  not in one mass edit across all 174 existing demos now and again later. The field
  itself lands early so nothing has to be revisited structurally.
- **Planned screen counts are estimates, not commitments.** If a planned screen turns out
  to demonstrate nothing real — the API is unreachable from consumer code, or it
  duplicates an existing screen — it is dropped and the reason recorded, rather than
  padded out to hit a number. plan.md's ~291 total is a projection.
- Phases F2/F3 will create `build-emscripten/` and `build-sdlrenderer/` (roughly 1 GB
  each). Sizes are reported at the end so they can be removed.

## 4. Known problems / limitations

- **`tools/check_shots.py` has one allowlisted false positive**:
  `3D Graphics/…/Buffers/VertexDeclaration Stride Gotcha`, whose 3D scene legitimately
  fills the viewport to the bottom edge (verified: 75% of its bottom-band ink is coloured
  geometry, not greyscale chrome text). See `BOTTOM_ALLOWED` in that script.
- **Hardware-unverified demos** (unchanged from before): Gamepad, Touch, Input's
  joystick/haptics screens, Devices' mobile-only Sensors/Vibration, Camera,
  MessageBox/FileDialog. They render and degrade gracefully; nobody has held the device.
- **`EASYGL` and `SDL_RENDERER` are both verified.** Remaining backends (Vulkan, bgfx, WebGPU)
  are untried here.
- **`build-sdlrenderer/` is ~777 MB** and `build/` is ~814 MB. Both are gitignored
  (`build-*/`). Delete `build-sdlrenderer/` if the space is wanted; it rebuilds from ccache
  fairly quickly.
- **`Framework/Window/ClientSizeChanged` and `Display Orientation` cannot be exercised
  headlessly.** Both need a window manager to drag the window with. They render and say so on
  screen; their event logs stay empty under Xvfb. Not a defect, but not verified either.
- **`Framework/Device Manager/VSync & MultiSampling` measures the same rate either way under
  Xvfb**, because a virtual display has no real refresh rate to synchronise to. The screen states
  this.
- A **phantom "select" event** was observed once reaching the window under Xvfb and
  silently toggling a demo's mode before the screenshot. Mitigated: any run with
  `--frames` ignores real input devices entirely (`InputState::SetScriptedOnly`). Root
  cause never identified; it did not reproduce in three consecutive identical runs.

## 4a. Traps in this repo's own workflow

**Never edit `tools/sweep*.sh` while a sweep is running.** bash reads a script incrementally from
a byte offset, so rewriting the file underneath it makes the running shell resume at the wrong
place. Doing this hung a sweep at 226/229 with no error -- the process stayed alive, no
`cna_examples` was running, and the screenshot directory simply stopped growing. `bash -n` on the
file afterwards was clean, which is what makes it confusing. Wait for the sweep, then edit.

- **Never rebuild while `tools/sweep.sh` is running.** The sweep launches the
  binary ~174 times; replacing it mid-run produced "53 of 174 demos failed" twice,
  with no real defect behind it. Let a sweep finish, or run it after the build.
- **`sweep.sh ... | tail -2` discards the per-demo results.** When a sweep does
  fail, redirect the whole log to a file first; otherwise there is nothing to
  diagnose from.

## 5. Technical discoveries worth not rediscovering

1. **An X11 root-window grab of a GL surface returns solid black under Xvfb.** Screenshot
   capture must go through `GraphicsDevice::GetBackBufferData` + `Texture2D::SaveAsPng`.
2. **`xvfb-run` alone does not make a run headless.** SDL3 selects the Wayland video
   driver whenever `WAYLAND_DISPLAY` is set and then ignores the `DISPLAY` that
   `xvfb-run` exports — the window opens on the real desktop while the command still
   looks virtual. Always go through `tools/headless.sh` / `tools/sweep.sh`, which force
   `SDL_VIDEODRIVER=x11` and unset `WAYLAND_DISPLAY`.
3. **`menufont.cnj` declares `lineSpacing: 29`, but its glyphs place ink up to 52px below
   the draw origin** (crop offset ≤ 46 + source height ≤ 24). `MeasureString().Y` derives
   its height from `lineSpacing`, so measuring does not reveal this. Use
   `DemoScreen::GlyphExtent()` (= 2 × lineSpacing) for any vertical layout decision.
4. **Class names in `src/Demos/` are ambiguous by design** — `Song` and `Video` both
   define `LoadAndPlayScreen`; four Input categories each define
   `StateEqualityHashScreen`. Anything that maps registrations to definitions must
   qualify by namespace (see `tools/check_catalog.py`).
5. **`MediaSource`'s constructor is private** (`friend class MediaLibrary`), so
   `MediaLibrary`'s `NotSupportedException` guard for non-`LocalDevice` sources is real
   code that consumer applications cannot reach at all.
6. **`Harness::FlattenCatalog` returns pointers into the catalog you pass it.**
   `FlattenCatalog(BuildAreaCatalog())` binds to a temporary and leaves every
   `IndexedDemo::demo` dangling. This went unnoticed while `MakeDemo`'s factory
   lambda was stateless (calling through the dead `std::function` happened to
   work); it crashed every `--demo` launch with `std::bad_array_new_length` the
   moment the lambda started capturing the `apis` vector. The rvalue overload is
   now `= delete`, so the mistake is a compile error.
7. **Menu selection happens on pointer RELEASE, not press.** Selecting on press
   makes drag-to-scroll impossible — the finger that lands to start a scroll
   immediately runs whatever it landed on.
8. **`Matrix::Decompose` does not detect shear.** It derives scale from basis-row lengths and
   returns false only when an axis is ~zero. A sheared matrix returns true with parts that do
   not rebuild it -- checking the round trip is the only way to notice.
9. **`0.1f + 0.2f == 0.3f` in single precision.** The famous inequality is a double result.
   `0.3f + 0.6f != 0.9f` is the float equivalent.
10. **`MathHelper::WithinEpsilon` is `|a-b| < MachineEpsilonFloat`** (~5.96e-8), a fixed
   *absolute* tolerance with a strict `<`. It does not generalise across magnitudes, and it
   rejects a one-ULP gap at 0.9 because the gap exactly equals the tolerance.
11. **`Quaternion` has no default constructor** in CNA; seed out-parameters with identity.
12. **`MathHelper::GetMachineEpsilonFloat()` is private.** `WithinEpsilon` is the public route.
13. **`.xnb` support is off until registered.** A fresh `ContentManager` throws "references an
   unregistered .xnb content type reader" until
   `CNA::Internal::Xnb::RegisterAllBuiltInXnbReaders()` is called once. The error names the
   reader but not the function to call.
14. **`ContentManager::setGraphicsDevice` is required** before any texture load; passing only
   the `IServiceProvider` to the constructor is not enough.
15. **CNA's `ContentManager::Load<T>` returns BY VALUE**, unlike XNA's, so consumer code cannot
   demonstrate the cache by comparing object identity. Timing is the only external evidence.
16. **Storage saves land under a real per-OS root**, `~/.local/share/<AppName>/` on Linux via
   `StorageDevice::SetAppNameEXT`. A demo that writes must never scatter files into the repo;
   `StorageContainer::ResolvePath` is private, so an app cannot ask where a file physically
   went -- the sandbox is deliberate.
17. **CNA's logging has TWO filters.** `CNA::Logger` keeps a minimum level, and SDL keeps a
   separate per-category priority. CNA pushes its level into SDL only inside
   `SetMinimumLevel` (`SDL_SetLogPriorities`), so before the first such call SDL's own defaults
   still drop Debug/Trace even though CNA passes them. Measured: 4 of 6 lines reach stderr at
   startup, 6 of 6 after.
18. **`Logger::IsEnabled` and `Logger::ToString` are private**, so an application cannot ask
   whether a level would pass, nor name a level. `GetMinimumLevel` is public; compare against it.
19. **`Logger::Log(level, message, category)` and `Logger::WarnIf(message, condition)`** both
   take the message FIRST -- the reverse of what the names suggest.
20. **`--demo` matches on the catalog path with `/` separators**, not the screen's display title.
   `--demo "Diagnostics: Logger"` finds nothing; `--demo "Diagnostics/Logging/Logger"` works.
21. **Gating a demo's draw path is not enough.** 3D demos build vertex buffers in `LoadContent`,
   which runs long before `Draw`, so a capability check on Update/Draw still let four demos abort
   with `SDL_Renderer does not support 3D: CreateVertexBuffer`. `DemoScreen::LoadContent` and
   `UnloadContent` are now `final` and gate `OnDemoLoad`/`OnDemoUnload`; every demo screen uses
   those hooks. Do NOT reintroduce a `LoadContent() override` in a demo -- it will not compile,
   which is the point.
22. **`AutoScrollToSelection` will fight a manual scroll.** After a drag, it used
   to yank the list straight back to the selected entry, so dragging appeared to
   do nothing. `MenuScreen::userScrolled_` holds it off until the selection moves.

23. **A Cue's seven flags are not seven booleans.** Six of them (`IsCreated`, `IsPreparing`,
   `IsPrepared`, `IsPlaying`, `IsStopping`, `IsStopped`) are one mutually-exclusive state value --
   exactly one is ever true. A cue from `SoundBank::GetCue` is therefore `IsPrepared` and **never**
   `IsCreated`; the constructor sets `State::Prepared` directly. `IsPaused` is the one real
   exception: FACT only sets/clears a PAUSED bit and never touches PLAYING, so a paused cue reads
   `IsPlaying == true` as well. I assumed the opposite and `tools/checks/xact_claims.cpp` caught it.
24. **A Cue is single-use.** Once stopped it does not go back to Playing, so each replay needs a
   fresh `GetCue`. That is XACT's design, not a CNA limitation.
25. **XACT bank generation lives outside this repo on purpose.** `../cna/examples/demo_xact/src/`
   is put on the *include path* by `cmake/ExamplesHelpers.cmake` and never copied: it is Ms-PL,
   this repo is MIT. Guarded by `CNA_EXAMPLES_HAS_XACT_FILEGEN`, so a missing `../cna` yields a
   screen explaining the absence instead of a build failure. Same policy as the `.xnb` fixtures.
26. **Scripted keys now cover Left/Right, and reach `IsNewKeyPress`.** Before this, `--keys` only
   emitted the four menu verbs and only through `IsMenuUp`/`IsMenuDown`/`IsMenuSelect`/
   `IsMenuCancel` -- so every demo binding raw `Keys::Left`/`Keys::Right` (several in Audio and
   Input) was unreachable from the harness. `InputState::ScriptedKey()` maps each action to the
   key it stands for and `IsNewKeyPress` honours it.

27. **`RenderTargetCube::GetData` silently returns zeros on EASYGL.**
   `ITextureCubeBackend::GetData` is a no-op by default and EasyGL's render-target cube backend
   overrides only `SetData`; `TextureCube::GetData` still validates, allocates a
   **zero-initialised** staging buffer, calls the no-op, and copies the zeros out. No exception.
   A caller gets plausible transparent-black data and never learns. Do not assume "no throw"
   means "worked" for any readback -- probe with a sentinel fill, which is the only way to
   distinguish real pixels / zeroed / untouched.
28. **A verification swatch beats a screenshot.** Every screen in the Volume & Cube Textures
   category computes its own pass/fail and draws a 24x24 colour block at x=40, so a sweep can
   assert correctness with a pixel probe instead of trusting that "it rendered". Cheap to add,
   and it caught the RenderTargetCube finding immediately. Worth repeating for any new screen
   that makes a checkable claim.
29. **`Color` has no default constructor.** `std::vector<Color> v(n);` does not compile --
   `std::vector<Color> v(n, Color::Black);` does. Costs a compile cycle every time.
30. **`BasicEffect::VertexColorEnabled` is a public FIELD, not a property setter.** There is no
   `setVertexColorEnabledProperty`. `World`/`View`/`Projection` are fields too, while
   `EnvironmentMapEffect` *does* use `setWorldProperty`-style accessors -- the two effects are
   genuinely inconsistent, so check the header rather than pattern-matching from a sibling.

31. **CNA's stock effects populate none of the reflection API.** A live `BasicEffect` reports
   `Parameters.Count == 0`, `Techniques.Count == 1` (the `"Default"` one `Effect`'s constructor
   adds), and `Parameters["World"] == nullptr`. The XNA idiom
   `effect.Parameters["X"]->SetValue(v)` therefore compiles and dereferences null at runtime --
   the lookup returns a pointer and never throws. State lives in typed C++ fields/properties.
   `EffectParameter` itself is fully functional and needs no device, so it can be exercised and
   asserted standalone.
32. **`SetValue(Matrix)` and `SetValueTranspose(Matrix)` are silently mismatched.** Each has its
   own getter; crossing them returns the transpose with no error. Always test matrix round trips
   with an ASYMMETRIC matrix -- a symmetric one hides it completely.
33. **`Effect::Clone()` returns a raw OWNING pointer** (documented deviation from FNA's
   GC-managed return). Adopt it into a `unique_ptr` immediately. `ShaderEffect::Clone()` uniquely
   RECOMPILES its GLSL rather than sharing the program, so it is not cheap.
34. **`check_layout.py` cannot see computed draw positions.** It inspects literal coordinates in
   source only. Three screens drew their verdict swatch past `LabelBaselineLimit()` (538px in a
   640px window) and it passed clean. `DemoScreen::DrawVerdict()` now clamps; keep body text to
   ~12 lines from y=82 (the step is `lineSpacing + 6` = 35px) so the clamp stays a safety net.

35. **The sweeps used to leave stale screenshots behind.** Renaming a category left the old
   `.png` in `build/screenshots` forever, so `check_shots.py` counted 233 files for 229 demos --
   and a DELETED screen would have looked like it was still passing. `tools/sweep.sh` and
   `tools/sweep_backend.sh` now clear `*.png`/`*.log` first, but only on an unfiltered run, since
   a filtered sweep is not authoritative about what should exist.

36. **Check what Input already covers before adding "missing" EXT screens.** D7's plan listed
   gamepad EXT sensors/haptics as a gap, but `GetPowerInfoEXT`, `GetGUIDEXT`, `GetPlayerIndexEXT`
   and `SetTriggerVibrationEXT` were already demonstrated across the existing 50 Input screens.
   Only `GetGyroEXT`, `SetLightBarEXT`, `TouchCollection::FindById`, `TouchPanel::NO_FINGER` and
   `GamePadButtons::FromButtonArray` were genuinely absent. One grep over `src/Demos/Input/`
   settles it.
37. **The EXT calls fail in two different ways.** `GetGyroEXT`/`GetAccelerometerEXT` return
   **bool** and fill the out-parameter only on true -- unchecked reads give stale data.
   `SetLightBarEXT` returns **void**: no light bar and no controller are both silent no-ops, so
   nothing can branch on it.
38. **`TouchCollection::FindById` writes a SENTINEL on failure**, it does not leave the
   out-parameter alone and does not throw. The miss is signalled by
   `TouchLocationState::Invalid`. `TouchCollection` uses `Add()`, not `push_back()`.

39. **CNA really implements `SimulatedLatency`/`SimulatedPacketLoss`; FNA does not.** In FNA both
   are inert auto-properties consumed by nothing. CNA's `ENetBackend` delays and drops AppData for
   real, scoped to **AppData only** (session-management and host-relay traffic are unaffected, so
   100% loss does not kill the lobby), with 0.0/1.0 handled deterministically.
   **Still unproven here:** the demo could not observe a dropped packet, because `SendData` with no
   recipient broadcasts to the OTHER gamers and a local session in this environment yields too few
   local gamers for the packets to have anywhere to go. Tried `Create(Local, 1, 4)` and
   `Create(Local, 2, 4)`, plus an explicit second receiver; both gave 0 received even at 0% loss.
   To make it conclusive, get two genuinely signed-in local gamers into the session first.

40. **On EASYGL only ONE SurfaceFormat works: `Color`.** All eighteen others throw on
   `Texture2D` construction. There is no `SupportsFormat()` query, so attempting construction is
   the only way to find out. This **refutes** the "SurfaceFormat is ignored, everything is silently
   RGBA8" assumption that D5's notes and the D6 plan row both carried -- unsupported formats are
   refused outright here, not coerced. Do not design a screen around Dxt5 or a float format
   without probing first.

41. **D6's Device Events group was already built.** `Framework/DeviceManager` covers MSAA,
   VSync/`PresentInterval` and `DeviceReset` between `DeviceEventsScreen` and
   `VSyncAndMultiSamplingScreen`. Grep before adding a planned group; this is the third phase in a
   row (D7, D8, D6) whose scope shrank once existing coverage was checked.
42. **`Texture2D::FromStream` rejects all malformed input properly** -- empty, random noise, and a
   truncated PNG (valid signature, no data) all throw rather than returning a garbage texture.
   BMP decodes too; the demo hand-builds a 2x2 24-bit BMP rather than vendoring an asset.
   Content pictures live under `Content/MediaLibraryDemo/Pictures/` (capital P).

## 6. Commands

```bash
# Build (ccache + -j4 are mandatory here -- see ../CLAUDE.md)
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build build -j4 --target cna_examples

# Validate
python3 tools/check_catalog.py --list        # screens vs registrations vs docs
python3 tools/check_layout.py                # hardcoded bottom-of-window draw positions
./tools/sweep.sh                             # screenshot every demo (virtual display)
./tools/sweep.sh Media                       # ...or a filtered subset
./tools/sweep_backend.sh build-sdlrenderer   # sweep a second backend's build tree
python3 tools/check_shots.py build/screenshots --quiet

# Assert on-screen claims directly (screens that swallow exceptions look fine when broken).
# tools/checks/{math,cnj,xact}_claims.cpp -- build recipe is in each file's header comment.

# Search, assertable from a shell (same matcher the search screen uses)
./build/cna_examples --list-demos --search "fromstream"

# Run one demo headlessly
./tools/headless.sh --demo "Media/Pictures/Browse" --frames 90 --screenshot /tmp/a.png
./tools/headless.sh --demo "Album/Artist/Genre" --keys select,down,select --frames 120
./tools/headless.sh --search "occlusion" --frames 70 --screenshot /tmp/s.png
# left/right now scriptable too (reaches IsNewKeyPress, not just the menu verbs)
./tools/headless.sh --demo "XACT/Cues" --keys select,left,left,right --frames 140
# Gestures: press at (480,560), drag to (480,300), release
./tools/headless.sh --keys down,select,select --pointer 480,560,300 --frames 200
```

## 6b. Phase F2 — the Emscripten build, in progress

**Where it got to:** `emcmake cmake -S . -B build-web` configures cleanly and **every translation
unit compiles for wasm**. The build fails only at link, on twelve undefined symbols, all of them
`Microsoft::Xna::Framework::Media::Video` / `VideoPlayer`. CNA does not build its video
implementation for Emscripten, but the headers still declare it, so the three
`src/Demos/Media/Video/*Screen.hpp` screens compile and then fail to link.

**Two cna-examples bugs were found and fixed getting that far** (committed):

1. `cmake/ExamplesHelpers.cmake`'s Emscripten branch linked `SDL3::SDL3-static`, a target that
   does not exist. Neither does `SDL3::SDL3`, from this project's scope: CNA imports SDL3 via
   `find_package` inside its own `cna_configure_vendored_sdl()` **function**, and IMPORTED targets
   are directory-scoped, so they are invisible outside `../cna`. Linking `CNA` alone is enough --
   its link interface carries the static SDL archives. **`../cna-samples` hard-codes
   `SDL3::SDL3-static` too and will fail identically if anyone builds it for web.**
2. SDL3 for wasm is already prebuilt at `../cna/.sdl-prebuilt-emscripten/install/lib/*.a` (21 MB),
   so nothing needs rebuilding -- do not delete that directory.

**DONE since:** the three Video screens are now gated out of the wasm translation unit
(`#if !defined(__EMSCRIPTEN__)` around the includes and the `BuildVideoDemos()` body, which
returns an empty category there). That removed all twelve of *this app's* undefined symbols.

**NOW BLOCKED ON AN UPSTREAM CNA DEFECT — `needs_human`.** The link still fails, but every
remaining undefined symbol comes from **CNA's own archive**, not from cna-examples:

    wasm-ld: error: CNA_BUILD/libCNA.a(VideoContentTypeReader.cpp.o):
             undefined symbol: Microsoft::Xna::Framework::Media::Video::Video(...)
             undefined symbol: vtable for Microsoft::Xna::Framework::Media::Video
             undefined symbol: typeinfo for Microsoft::Xna::Framework::Media::Video

CNA compiles `VideoContentTypeReader.cpp` into `libCNA.a` for Emscripten while omitting the
`Video`/`VideoPlayer` implementation those objects reference, so the archive is internally
inconsistent on that target: **any** web consumer of CNA hits this, not just this app. Nothing in
cna-examples can fix it. Upstream needs to either exclude `VideoContentTypeReader.cpp` from the
Emscripten build too, or provide stub definitions for `Video`. Until then F2 cannot finish linking.

**Historical next step (now superseded):** gate the three Video screens out of the Emscripten build. `Requiring()` takes a
`GraphicsCapability` and this is a platform condition, so it needs a separate predicate --
conditional compilation (`#if !defined(__EMSCRIPTEN__)`) around the Video includes and the
`BuildVideoDemos()` body is the smallest honest fix. Watch out: `tools/check_catalog.py` counts
screens from the source tree, so the web build will legitimately have 3 fewer than the native 234.

Build command (emsdk is NOT on PATH by default):

```bash
source ~/emsdk/emsdk_env.sh
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build build-web -j4 --target cna_examples
```

## 7. Blocked / needs_human

**D2 (PbrEffect) — `needs_human`: PbrEffect renders nothing from this app, cause not found.**

A `PbrEffect` metallic/roughness grid screen was written, verified by pixel measurement, and
**reverted** because it never rendered geometry. The repo is back at the clean 229-demo state; the
work-in-progress screen and the sphere-helper patch are preserved under the session scratchpad
(`pbr-wip/`) but are NOT in the tree.

What was established, so none of it needs redoing:

1. **PbrEffect IS implemented in EasyGL.** `EasyGLGraphicsBackend::EnsurePbrProgram()` compiles a
   real metallic-roughness BRDF (`PbrLight()`, three directional lights, normal/emissive/occlusion
   maps). This is not an unimplemented-feature dead end.
2. **Tangent vertex types are second-class across the API.** `GraphicsDevice` has typed
   `DrawUserIndexedPrimitives` overloads for only four legacy vertex types; a
   `VertexPositionNormalTangentTexture` array therefore binds the untyped `const void*` overload,
   which carries NO vertex declaration, so the GPU reinterprets stride-48 data under whatever
   layout was last bound. It does not throw -- it draws garbage that fills the viewport. That was
   the first symptom and it was diagnosed by measurement, not by the app failing.
   `VertexBuffer::SetData` has the same gap; uploading needs `SetDataRaw(data, count, stride)`.
3. **`Tangent` is a `Vector4`, not a `Vector3`** -- W is the bitangent handedness sign, glTF
   convention `Bitangent = cross(Normal, Tangent.xyz) * Tangent.W`. For a UV sphere `cross(N,T)`
   works out to `d(position)/d(phi)`, which points along +V, so W = +1.
4. **Things tried that did NOT fix it** (all measured, viewport stayed empty): the buffered path
   with an explicit `VertexDeclaration`; the two-argument `VertexBuffer(device, count)` constructor
   that `../cna`'s own working example uses; `SetDataRaw` with an explicit stride; flipping the
   sphere's triangle winding; `RasterizerState::CullNone`; binding a base-colour `Texture2D`
   (the PBR shader samples albedo, so a null texture was a plausible cause); expanding to a flat
   non-indexed list drawn with `DrawPrimitives` instead of `DrawIndexedPrimitives`.
5. **The one untested difference from the known-good path.**
   `../cna/examples/easygl_pbreffect_golden_test.cpp` renders correctly and differs from the app in
   exactly one remaining respect: it uses **identity** World/View/Projection with quad vertices
   already in NDC, whereas the app sets real camera matrices. Start there -- verify whether
   `PbrEffect`'s `setWorldProperty`/`setViewProperty`/`setProjectionProperty` actually reach the
   shader, e.g. by drawing one NDC-space triangle with identity matrices first and only then
   introducing a camera. If the matrices are the problem, that is a CNA bug worth reporting
   upstream rather than working around here.

Everything else in the roadmap is unblocked. The three that could
have been were settled up front and are recorded in §2: asset licensing, branch policy, and
depth-over-breadth.

One judgement call worth re-examining if it ever bites: every new area this session shipped
**fewer screens than projected** (Content 5 of 15, Storage 2 of 6, Diagnostics 4 of 13), because
the projections counted API surface rather than distinct demonstrations, and splitting a single
idea across three screens teaches nothing extra. Each omission is itemised in `plan.md`. If the
project owner actually wants the higher counts — for coverage-metric reasons, say — that is a
preference worth stating, because the current bias is deliberate and will otherwise continue.

## 8. Resume here

Candidates, in this order:

**(a) Phase D2/D3/D4/D6/D7/D8** — deepening existing areas (PBR & pipeline, Model content, effect
reflection, Texture3D/Cube/RenderTargetCube, occlusion queries, 2D surface formats and device
events, Input EXT screens, Net QoS). **D1 (XACT) and D5 (Textures & Queries) are done.** D2-D4 are
all 3D and must be wrapped in `Requiring(CNA::GraphicsCapability::ThreeD, ...)` like the existing
3D categories, or the SDL_RENDERER sweep will abort on them.

**(b) Extend C4 Storage** — container directory operations and container lifetime.

**(c) Phase F2, the Emscripten build** — `~/emsdk` is installed (`emcc` at
`~/emsdk/upstream/emscripten/emcc`, not on `PATH`; source `~/emsdk/emsdk_env.sh`). Expect to gate
Net, Camera, FileDialog, SystemTray, Microphone and Storage the way 3D is now gated — the
`Requiring()` + `SetRequiredCapability` machinery is in place, though a platform gate would need
a predicate other than `GraphicsCapability`.

Then E (Avatars, reusing the build-time asset copy already working for `.xnb`) → F1.

**The asset-borrowing mechanism is built and working** (`cmake/ExamplesHelpers.cmake`), so
Phase E's avatar meshes can reuse the same pattern: copy from `../cna` at build time, guard on
existence, and have the demo report absence on screen.

Pattern established by C1 and worth repeating:

1. Read the CNA header first and build against what is actually there — CNA's
   `DisplayOrientation` has no `PortraitDown`, and assuming the XNA/WP7 shape cost a compile
   cycle.
2. Put anything a demo mutates globally back in `OnDemoUnload()` (NOT `UnloadContent`,
   which is now `final` on DemoScreen), and *verify* the restore
   rather than trusting it (the back-buffer size is readable straight off the screenshot).
3. Syntax-check new screens with a throwaway TU before touching `AreaCatalog.hpp`:
   `g++ -std=c++23 -fsyntax-only $DEFS $INCLUDES /tmp/tu.cpp`, taking `$DEFS`/`$INCLUDES` from
   `build/CMakeFiles/cna_examples.dir/flags.make`. A full rebuild per iteration is far slower.
4. Sweep the new area alone (`./tools/sweep.sh "Area/"`) before the full sweep.
5. **A screen that catches its own exceptions renders cleanly when everything failed.** Both the
   XNB and CNJ screens do this by design, so a green sweep says nothing about whether they
   loaded anything. `tools/checks/*.cpp` assert the claims directly; add one for any new area
   whose screens swallow errors.
6. **Check that a demo actually did the thing**, not just that it rendered. The Content area's
   XNB screen catches its own exceptions and prints them, so a total failure to load still
   produced a clean-looking screenshot and a passing sweep. What caught it was measuring the
   preview region: no texture drawn meant no load. Two real bugs hid behind that.
6. **If a screen asserts a fact, test the fact.** The Math area's screens state things
   ("a shear cannot be decomposed", "0.1f + 0.2f != 0.3f"); three such statements were
   confidently wrong. `tools/checks/math_claims.cpp` caught them. Screens that merely display
   live state do not need this; screens that make claims do.
