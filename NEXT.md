# NEXT — short-term continuity for cna-examples

**Updated:** 2026-07-27
**Branch:** `feature/examples-phase-bcde` (branched from `develop` @ `d7353e3`)
**Authoritative plan:** [`plan.md`](plan.md). Historical record: [`plan20260727.md`](plan20260727.md).

This file is the continuity document: what just happened, what is in flight, what is
blocked, and exactly where to resume. `plan.md` holds the roadmap; this file holds the
state.

---

## 1. Where things stand

| | |
|---|---|
| Demo screens | **191** across 8 areas, 56 categories |
| Last full validation | 191/191 render, 0 layout problems, catalog+layout+docs clean |
| Phase A (correct what exists) | **Done** — see plan.md §7 |
| Phase B (navigation shell) | **Done** — search, drag-scroll, breadcrumbs, API footer |
| Phase C1 (Framework area) | **Done** — 5 categories, 17 screens |
| Phases C2–C5, D, E, F | Not started |

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
- **Only the `EASYGL` backend is verified.** Phase F3 adds `SDL_RENDERER`.
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
8. **`AutoScrollToSelection` will fight a manual scroll.** After a drag, it used
   to yank the list straight back to the selected entry, so dragging appeared to
   do nothing. `MenuScreen::userScrolled_` holds it off until the selection moves.

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
python3 tools/check_shots.py build/screenshots --quiet

# Search, assertable from a shell (same matcher the search screen uses)
./build/cna_examples --list-demos --search "fromstream"

# Run one demo headlessly
./tools/headless.sh --demo "Media/Pictures/Browse" --frames 90 --screenshot /tmp/a.png
./tools/headless.sh --demo "Album/Artist/Genre" --keys select,down,select --frames 120
./tools/headless.sh --search "occlusion" --frames 70 --screenshot /tmp/s.png
# Gestures: press at (480,560), drag to (480,300), release
./tools/headless.sh --keys down,select,select --pointer 480,560,300 --frames 200
```

## 7. Blocked / needs_human

*(none currently)*

## 8. Resume here

**C2 Math** — 5 categories, ~21 screens (Vectors, Matrix & Quaternion, Geometry, Curves,
Color & Packed Vectors). See plan.md §7 Phase C for the per-screen breakdown. Then C3 Content →
C4 Storage → C5 Diagnostics → D → E → F.

Pattern established by C1 and worth repeating:

1. Read the CNA header first and build against what is actually there — CNA's
   `DisplayOrientation` has no `PortraitDown`, and assuming the XNA/WP7 shape cost a compile
   cycle.
2. Put anything a demo mutates globally back in `UnloadContent()`, and *verify* the restore
   rather than trusting it (the back-buffer size is readable straight off the screenshot).
3. Syntax-check new screens with a throwaway TU before touching `AreaCatalog.hpp`:
   `g++ -std=c++23 -fsyntax-only $DEFS $INCLUDES /tmp/tu.cpp`, taking `$DEFS`/`$INCLUDES` from
   `build/CMakeFiles/cna_examples.dir/flags.make`. A full rebuild per iteration is far slower.
4. Sweep the new area alone (`./tools/sweep.sh "Area/"`) before the full sweep.
