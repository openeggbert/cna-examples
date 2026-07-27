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
| Demo screens | **222** across 12 areas, 73 categories |
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
| Phases D2–D8, E, F1, F2 | Not started |

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

## 7. Blocked / needs_human

*(none)* — nothing in the roadmap is currently blocked on a human decision. The three that could
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

**(a) Phase D2 onwards** — deepening existing areas (PBR & pipeline, Model content, effect
reflection, Texture3D/Cube/RenderTargetCube, occlusion queries, 2D surface formats and device
events, Input EXT screens, Net QoS). **D1 (XACT) is done.** D2–D5 are all 3D and must be wrapped
in `Requiring(CNA::GraphicsCapability::ThreeD, ...)` like the existing 3D categories, or the
SDL_RENDERER sweep will abort on them.

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
