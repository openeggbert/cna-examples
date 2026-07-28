# NEXT — short-term continuity for cna-examples

**Updated:** 2026-07-28 (autonomous session continuing — D3, E, C4-extend, F1, D2 and now all three
of D2's follow-ups (RenderPipelineSettings, PbrMaterial) + D8's diagnosis correction all done)
**Branch:** `feature/examples-phase-bcde`, ahead of `develop` @ `d7353e3`, all
pushed. Working tree clean, no jobs in flight, both native build trees green.
**Authoritative plan:** [`plan.md`](plan.md). Historical record: [`plan20260727.md`](plan20260727.md).

This file is the continuity document: what just happened, what is in flight, what is
blocked, and exactly where to resume. `plan.md` holds the roadmap; this file holds the
state.

---

## 1. Where things stand

| | |
|---|---|
| Demo screens | **249** across 13 areas, 79 categories |
| Last full validation | **249/249 on EASYGL and SDL_RENDERER** (re-run after D2's RenderPipelineSettings follow-up and D8's diagnosis correction), 249 screenshots each, 0 layout problems, catalog+layout+docs clean |
| Head commit | `b5e9a1c` |

**Phases, in roadmap order:**

| Phase | Status |
|---|---|
| A — correct what exists | **Done** — see plan.md §7 |
| B — navigation shell | **Done** — search, drag-scroll, breadcrumbs, API footer |
| C1 Framework | **Done** — 5 categories, 17 screens |
| C2 Math | **Done** — 5 categories, 16 screens |
| C3 Content | **Done** — 5 categories, 7 screens |
| C4 Storage | **Done, extended 2026-07-28** — 2 categories, 4 screens |
| C5 Diagnostics | **Done, reduced scope** — 4 categories, 4 screens |
| D1 Audio/XACT | **Done, reduced scope** — 1 category, 2 screens |
| D2 PBR | **Done, 2026-07-28** — 1 category, 2 screens. Was `needs_human`; fixed once the root cause (a polymorphic-vertex `sizeof()` mismatch) was known. `RenderPipelineSettingsScreen` shipped same-day as the deferred second half of D2's original plan, behind a newly-enabled `CNA_NOXNA` (same precedent as `CNA_DEVICES`). A further same-day finding, `PbrMaterial` (also real but unread, bypassed by `PbrEffect` and CNA's own glTF loader alike), was folded into the existing PbrEffect screen rather than becoming a third — see §7 |
| D3 Model Content | **Done, reduced scope** — 4 screens added to the existing Model category |
| D4 Effect Reflection | **Done, reduced scope** — 1 category, 3 screens |
| D5 3D Textures & Queries | **Done** — 1 category, 4 screens |
| D6 2D formats & events | **Done, reduced scope** — 2 screens; Device Events already existed |
| D7 Input EXT | **Done, reduced scope** — 2 screens into existing categories |
| D8 Net | **Done, reduced scope** — 1 screen; its amber verdict is now DEFINITIVELY diagnosed (2026-07-28), not just "not yet made conclusive" — see #39 and #56 |
| E Avatars | **Done** — 3 categories, 7 screens, exactly at plan.md's original count |
| F1 defect sweep | **Done, 2026-07-28** — 2 real defects found and fixed (both cna-examples-side), 1 major upstream `../cna` content defect root-caused and left `needs_human` — see §5 #51 (updated), #53a, #53b and plan.md's F1 writeup |
| F2 Emscripten | **BLOCKED on an upstream CNA defect** — exact one-line fix identified, not applied — see §6b and §7 |
| F3 SDL_RENDERER pass | **Done** — 249/249 on both backends, 3D gated on ThreeD |

**Scope kept shrinking, and that was correct.** D4 went 4→3, D6 6→2, D7 3→2, D8 3→1, D3 5→4. Every
cut was verified as already-covered, non-existent, or (D3's `SkinnedModelEXT`) genuinely out of
category rather than absent, and each is recorded with its reason in `plan.md`. **Grep
`src/Demos/` for the APIs a phase claims are missing before writing any code** — three phases in a
row (D6, D7, D8) shrank once that check was actually done.

**2026-07-28 (this session): the project owner asked to push closer to `plan.md`'s originally
projected screen counts** rather than cutting for depth-over-breadth by default (see §2a below) —
D3's own cut is a case of following that instruction correctly anyway: `SkinnedModelEXT`
is architecturally Avatar-only (its own doc comment says so), not merely a duplicate idea, so
shipping it in the Model category would have been a category-boundary mistake, not padding
avoidance.

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

## 2a. Session decisions (answered by the project owner, 2026-07-28)

1. **Push closer to `plan.md`'s originally projected screen counts** rather than defaulting to
   depth-over-breadth cuts — supersedes §2 item 3's bias where the two conflict. A planned idea
   substantial enough to split into more than one screen should be split, per the owner's explicit
   answer. Cuts are still made, but only for a concrete reason (already covered elsewhere, the API
   doesn't exist, or — new this session — the idea belongs to a different category/phase
   entirely), never merely to keep the count down.
2. **D2 (PbrEffect) and F2 (Emscripten): investigate further and write up findings, but do not
   modify `../cna`.** Both are diagnosed as defects living inside CNA itself rather than in
   cna-examples, and `../cna` is the same owner's own repo (not a true third party) — but the
   owner chose to keep this session's changes confined to `cna-examples`. Leave `../cna`'s
   pre-existing uncommitted files (`cmake/Tests/EasyGLTests.cmake`, `cmake/Tests/
   SdlRendererTests.cmake`, untracked `examples/xvfb_screenshot_demo.cpp`) untouched — unrelated
   in-progress work from another session, not this one's to manage.
   **Outcome, D2:** the investigation later found the cause actually lived in cna-examples' own
   reverted screen code, not `../cna` — so this instruction's premise didn't hold for D2, and it
   was fixed within the same `../cna`-untouched scope this instruction describes (see §7). F2's
   cause is genuinely in `../cna`; it remains blocked exactly as this instruction anticipated.
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

43. **`ContentManager`'s own `.cnj` `ModelTypeReader` needs no borrowed asset at all.** Unlike
   `.xnb` (a MonoGame-compiled binary CNA only reads) and XACT (no Linux authoring tool exists),
   `.cnj` + `.skeleton.bin`/`.clip.bin`/morph-target binary sidecars are CNA's own plain,
   documented formats -- a demo can synthesize a full skeleton+animation+morph-target Model
   in-process at `OnDemoLoad()`, exactly the technique `../cna`'s own
   `easygl_model_skinned_animation_playback_test.cpp` golden test uses to build its fixture. No
   asset-borrowing decision was needed for D3 after all.
44. **`ModelMeshPart::setEffectProperty()` self-maintains its parent `ModelMesh`'s `Effects`
   collection -- but only if the part already has a parent when it is called.** It `Add()`s the
   new effect and `Remove()`s the old one (properly ref-counted: only if no other part in the mesh
   still shares it), all inside the setter. `ContentManager.cpp`'s `ModelTypeReader` constructs the
   `ModelMesh` first, then calls `setEffectProperty()` on its parts, so the sync fires and
   `Model::Draw()`'s per-frame World/View/Projection push reaches the loaded effects with zero
   extra code. `ModelGroup/ProceduralModelScreen.hpp` calls `setEffectProperty()` **before**
   constructing the owning `ModelMesh` (parent still null), so the sync silently does nothing --
   which is exactly why that screen's own code needs a manual
   `mesh_->getEffectsPropertyMutable().Add()` afterward. Both are correct; the API is just
   silently construction-order-dependent. **This was first read wrong from source alone** (grepping
   `ContentManager.cpp` for `.Add(` misses that the Add/Remove live in `ModelMeshPart.cpp`, one
   layer down) and only caught by writing a standalone debug binary linked against `../cna`'s own
   `libCNA.a` and checking `Effects.Count` for real -- see D3's `plan.md` writeup for the exact
   repro. Re-confirms this repo's own repeated lesson: verify live, do not trust a source-reading
   conclusion that was never actually run.
45. **`EffectMaterial` is never constructed by either Model content reader (`.cnj` or `.xnb`)**,
   contradicting its own doc comment ("created internally by the content pipeline"). Both readers
   always build a real stock effect (`BasicEffect`/`SkinnedEffect`/etc.) directly. It IS
   constructible/clonable by hand, but `EffectMaterial::OnApply()` (read from source) is an empty
   function body -- applying one binds no parameters and changes no GPU state.

46. **`AttachPartEXT`/`RemovePartEXT` are `SkinnedModelEXT` methods, not Avatar methods** -- D3
   deliberately left `SkinnedModelEXT` alone on this basis and Phase E confirms the judgement was
   right. `AvatarRenderer::EnableRealRenderingEXT(device, shared_ptr<SkinnedModelEXT>)` is the only
   bridge between the two: the real, GPU-skinned render path is layered entirely on top of
   `SkinnedModelEXT`, completely separate from the faithful (never-renders) `Draw()`/`State`/
   `BindPose`/`ParentBones` surface real XNA exposes.
47. **`AvatarRenderer::getParentBonesProperty()` is populated unconditionally from a real, hardcoded
   71-entry table at construction** -- it does NOT come back empty the way a first read of the
   getter alone suggests (nothing in the getter itself populates anything; the constructor does).
   `getBindPoseProperty()` is sized identically at construction but stays default/identity content
   and is unreachable anyway, since it throws `InvalidOperationException` unconditionally (`State`
   never becomes `Ready`). The two "real skeletal data" members behave oppositely from each other.
   Caught by a live screenshot after the screen's own first-draft assumption (mirroring
   `getBindPoseProperty()`'s behavior) turned out wrong -- source-reading the getter alone was not
   enough; the constructor is where the real answer lives.
48. **A single loaded `SkinnedModelEXT` only ever has 21 of the 31 `AvatarAnimationPreset` clip
   names in its `Clips` map** (11 gender-neutral + its own gender's 10) -- the other gender's 10 are
   simply absent, and `ComputeBoneTransformsEXT`/`DrawRealEXT` throw `ArgumentException` for them.
   "All 31 presets auto-cycled on one avatar" (plan.md's original Phase E wording) was never
   achievable; verify every clip name against `ComputeBoneTransformsEXT` before ever drawing with
   it, rather than discovering the throw live.
49. **`SkinnedModelEXT::GetOwnedPartCountForTesting()`/`GetOwnedVertexBufferCountForTesting()`/etc.
   are real, `NOXNA`-marked testing accessors on the type itself**, not hidden behind a test-only
   friend or a separate header -- a demo screen can assert `AttachPartEXT`'s replace-by-name
   contract (constant part count across repeated swaps) directly, no GPU pixel probe required. Used
   in the Wardrobe Hot-Swap screen to confirm the base avatar's 5 parts (Body/Hair/Pants/Shirt/
   Shoes) stay exactly 5 across every baked-in/Cap/Ponytail cycle.
50. **`DemoScreen::DrawVerdict()`'s caption is NOT ellipsized the way `DrawLines()`'s text is** --
   it calls `spriteBatch.DrawString()` directly with no width bound. A caption written the same
   length as a `DrawLines()` line (which auto-truncates) runs straight off the right edge with no
   warning; `tools/check_shots.py`'s `right(Npx)` flag is what catches it, not a compile or runtime
   error. Every existing verdict caption in this codebase happens to be short enough to avoid this
   -- keep new ones under roughly 55-60 characters (`"PASS: meshes loaded; Effects self-maintenance
   verified"`, 56 chars, is a safe reference length) rather than assuming `DrawVerdict()` wraps or
   truncates like `DrawLines()` does.
51. **ROOT-CAUSED 2026-07-28 (Phase F1) -- was recorded as "Stand2's head goes invisible", turned
   out to be systemic.** A standalone diagnostic linked directly against `../cna`'s already-built
   `libCNA.a` (load each real `SkinnedModelEXT`, dump every clip's raw per-keyframe data) found
   ~60 `(clip, bone)` track pairs across BOTH genders and nearly every expressive clip
   (`Stand0`-`Stand7`, `Wave`, `Celebrate`, `Clap`, every `Male*`/`Female*` emote, every idle
   variant) where `Translation` is correct ONLY on a track's first and last keyframe (exactly
   matching `BindPoseLocal`) and reads raw `(0,0,0)` on 100% of the interior keyframes.
   `Stand2`/head-bone(12): keys 1-108 all `(0,0,0)` vs bind pose `(0,0.100,0)`; only keys 0 and 109
   correct. `Rotation`/`Scale` are fine throughout (smooth, plausible motion) -- only `Translation`
   collapses. This pulls the affected bone toward its parent's origin for nearly the whole clip,
   snapping back only at the very first/last frame; for a leaf bone like the head that reads as
   "sunk into the torso, invisible". **`Stand7`'s own ROOT bone (index 0, bind length 1.0) shows the
   identical 138/138 interior-collapse pattern** -- a whole-body-scale instance of the same defect.
   `ContentManager::ReadAnimationClipFileEXT` (the `.clip.bin` reader) was read in full and ruled
   out -- three sequential float reads per axis, already hardened against a real evaluation-order
   bug (Task 11.11). **The defect is upstream, in the content itself**: the `.clip.bin` files
   `../cna/tools/avatar_builder/` bakes apparently only ever write a real translation on a track's
   first/last keyframe. See §7 for the needs_human writeup -- no fix attempted here per the owner's
   2026-07-28 instruction to investigate `../cna`-side defects without modifying `../cna`.
52. **`GetFileNames()`/`GetDirectoryNames()` never recurse.** Both only ever list this container's own
   root. `CreateFile`/`OpenFile` happily accept a nested relative path like `"notes/todo.txt"` and the
   file is completely real (`FileExists` confirms it) -- it is simply invisible to a root-level
   listing call. Only a caller that already knows a subdirectory's name can enumerate what is inside
   it. `DeleteDirectory` is also stricter than it looks: it throws on a non-empty directory rather
   than doing anything recursive, unlike `StorageDevice::DeleteContainer` (next item).
53. **`StorageContainer::Dispose()` does not gate anything.** No method on `StorageContainer` checks
   `IsDisposed` before running, so `FileExists`/`CreateFile`/etc. on an already-disposed handle keep
   working exactly as before -- confirmed live, not assumed. `Dispose()` here means only "the
   `Disposing` event has fired" (itself genuinely idempotent: a 2nd call does not re-raise it,
   confirmed by an event-subscriber counter). Reopening the same container name is not tracked at all
   -- `StorageDevice::EndOpenContainer` hands out a brand-new object every time with no "already open"
   concept, so two independent live handles over the same directory coexist fine and each immediately
   sees what the other writes. `StorageDevice::DeleteContainer`, by contrast, really does remove the
   entire tree in one call (`fs::remove_all`), no "must be empty" restriction.
54. **`DrawVerdict()`'s clamp and `DrawLines()`'s own stop condition clamp independently to the
   SAME `LabelBaselineLimit()`, so they can collide.** Found by Phase F1's screenshot spot-check,
   not by `check_layout.py` (which only inspects literal source coordinates, not computed ones --
   the same blind spot recorded for item 34's original finding). `DrawLines()` happily draws its
   last body line right up to that limit; if the screen's line count reaches exactly that point,
   `DrawVerdict()`'s own clamped Y lands on the SAME pixel row instead of below it, so the verdict
   caption is drawn directly on top of the last content line -- both become unreadable, overlapping
   text. Hit by `Net/NetworkSession/Simulated Latency & Packet Loss` at 13 lines (one over the
   ~12-line budget). Item 34's clamp only prevents drawing *under* the Back hint; it does not
   reserve room *against* `DrawLines()`'s own last line. Fixed by trimming that screen to 12 lines
   (same remedy as D4's screens, see plan.md) rather than changing the shared helper -- 13 other
   `DrawVerdict()` screens at or near the boundary were spot-checked directly and none reproduce it,
   so this stays a per-screen budget discipline issue, not a rearchitected shared component.
55. **The "Stand2 head invisible" finding (item 51, superseded above) is not a one-clip curiosity --
   it is a systemic upstream `.clip.bin` content defect spanning ~60 (clip, bone) track pairs across
   nearly the entire avatar animation library, both genders.** See the rewritten item 51 above and
   §7 for the full diagnosis and reproduction recipe. The technique worth remembering: a throwaway
   `.cpp` linked directly against an ALREADY-BUILT `libCNA.a` (no rebuild, no CMake reconfigure --
   just the same `$DEFS`/`$FLAGS`/`link.txt` recipe `tools/checks/*.cpp` already use) can load real
   game content and dump its raw data for inspection in seconds, far faster than adding
   screenshot-based instrumentation to a live demo screen for a one-off investigation.
56. **D2's polymorphic-vertex diagnosis was confirmed live, not just by source-reading.** A
   `static_assert(sizeof(VertexPositionNormalTangentTexture) == 48)` genuinely fails to compile
   (real size is 56), and repacking into a private, non-polymorphic 48-byte POD before
   `SetDataRaw` immediately fixed the render on the first attempt -- no other change was needed.
   Worth noting for the NEXT time this bug class is suspected on a new vertex type: the
   `static_assert` check costs seconds and settles it before writing any rendering code.
57. **`RenderPipelineSettings`/`PbrMaterial` live behind `CNA_NOXNA`, a real CMake option (default
   OFF in CNA), NOT enabled in this project until 2026-07-28.** `CNA_NOXNA:BOOL=OFF` in
   `build/CMakeCache.txt` meant the entire `CNA::Graphics` namespace was compiled out of this app,
   even though the source files are always present in `../cna` (grepping the source tree, not the
   configured build, silently hides this). `CMakeLists.txt` now force-enables it the same way it
   already force-enables `CNA_DEVICES`. Small, contained blast radius (7 files total in `../cna`),
   full rebuild confirmed no regressions across all 249 screens on both backends.
58. **`../cna/include/CNA/Graphics/PbrMaterial.hpp` exists (the old D2 note claiming otherwise was
   stale) but is bypassed entirely, not merely unread -- investigated 2026-07-28, resolved.**
   `PbrMaterial` is a real, non-stub glTF-style settings bag (5 texture slots + 6 scalar factors).
   Grepping ALL of `../cna/src`, `../cna/include`, `../cna/examples/`, `../cna/tests/` for its use
   finds exactly one constructor call outside its own `.cpp`: the round-trip test in
   `../cna/examples/noxna_settings_example.cpp` (same file that tests `RenderPipelineSettings`).
   The trap: `RuntimeGltfModelTests.cpp`'s test named
   `LoadsPbrMaterialWithAllFourMapsAndFactorsFromGltf` sounds like it exercises `PbrMaterial`, but
   reading it shows it asserts against **`PbrEffect`'s own** texture/factor properties directly
   (`getTextureProperty`/`getMetallicFactorProperty`/etc.) -- the exact same properties
   `PbrMetallicRoughnessScreen.hpp` already sets. CNA's real glTF content pipeline populates
   `PbrEffect` directly and never constructs a `PbrMaterial` at all. Folded into
   `PbrMetallicRoughnessScreen.hpp` as a verified-live round trip plus one on-screen line, rather
   than a third thin screen for a second instance of the same "real store, zero readers" finding as
   `RenderPipelineSettings` -- no new screen registered, count stays at 249. A text-layout
   collision (the added line pushed content into the sphere-grid viewport) was caught by eye and
   fixed by trimming surrounding lines, not by moving the scene viewport.
59. **D8 Net's amber verdict was previously misdiagnosed as "not enough local gamers"; the real
   cause is architectural and a second local gamer cannot fix it.**
   `NetworkSession::Update()` (`NetworkSession.cpp`) gates its entire `PacketSend` delivery path
   behind `ENetBackend::RealNetworkingEnabled(sessionType_)`, which is `true` **only** for
   `NetworkSessionType::SystemLink` (`ENetBackend.cpp`) -- for `NetworkSessionType::Local`, every
   `PacketSend` is an unconditional no-op regardless of gamer count (`LocalNetworkGamer::EnqueuePacket`
   has exactly one call site, and it sits behind that same gate). Confirmed empirically too, not just
   by source-reading: constructing two real non-guest `SignedInGamer`s via `SignedInGamer::CreateInternal(...)`
   and `NetworkSession::Create(sessionType, vector<SignedInGamer*>, ...)` (which bypasses the
   guest-filtering that caps the simpler `Create(sessionType, maxLocalGamers, maxGamers)` overload
   at 1 non-guest gamer -- see `GamerRosterScreen.hpp`'s own existing finding) still delivered
   0 packets on `Local`. The real (SystemLink) path works -- proven in `ENetBackendTests.cpp` -- but
   reaching it from one self-contained demo screen isn't possible through the public XNA API: real
   discovery needs a second `cna_examples` process, and CNA's own tests only get same-process
   loopback by dropping to `CNA::Internal::Net::ENetHostHandle` + hand-encoded `AppDataMessage`
   packets, which is internal transport plumbing, not the public `Microsoft::Xna::Framework::Net`
   surface this catalog demonstrates. `SimulatedConditionsScreen.hpp`'s comments and on-screen text
   were rewritten to state this precisely -- this is now a settled limitation, not an open
   follow-up; do not re-attempt "just add a second local gamer."

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

## 6b. Phase F2 — the Emscripten build (BLOCKED upstream, detail here; summary in §7)

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

**D2 (PbrEffect) — RESOLVED 2026-07-28. No longer needs_human.** Full history and the fix are in
`plan.md`'s D2 result section (search `#### D2 PBR`); kept there rather than duplicated here since
this is a continuity/status file, not the roadmap record. Summary: root cause was a polymorphic
`VertexPositionNormalTangentTexture` (hidden vtable pointer inflates its real `sizeof()` to 56, not
the naive 48 the original screen assumed), fixed by repacking into a private packed POD before
upload. Verified live by pixel probe; shipped as `3D Graphics > Effects Gallery > PbrEffect >
Metallic & Roughness`. `docs/wip-pbr/` has been removed (superseded by the real screen).

One independent, dormant CNA-side finding surfaced while confirming the diagnosis, **not
cna-examples' to fix, not urgent** (worth a heads-up upstream sometime):
`VertexPositionNormalTangentTexture.cpp`'s `getVertexDeclarationStatic()` hardcodes per-field
`VertexElement` offsets `0,12,24,40`, which assume no vtable pointer; the true offsets (given the
real 56-byte layout) would be `8,20,32,48`. No typed `SetData` overload exists to exercise this
declaration today, so it's latent rather than actively wrong, but would misbehave the instant
something did use it.

---

**F2 (Emscripten) — `needs_human`: blocked on a defect in CNA itself, not in this app.**

`emcmake` configures cleanly and **every translation unit compiles for wasm**. Two genuine
cna-examples bugs were found and fixed getting there (see §6b for both, including the
`SDL3::SDL3-static` target that never existed). The link now fails only inside CNA's own archive:

    wasm-ld: error: CNA_BUILD/libCNA.a(VideoContentTypeReader.cpp.o):
             undefined symbol: Microsoft::Xna::Framework::Media::Video::Video(...)
             undefined symbol: vtable for Microsoft::Xna::Framework::Media::Video

CNA compiles `VideoContentTypeReader.cpp` into `libCNA.a` for Emscripten while omitting the
`Video`/`VideoPlayer` implementation those objects reference. **Any** web consumer of CNA hits
this, not only this app. The fix belongs upstream — exclude that translation unit from the
Emscripten build too, or provide stub definitions for `Video`. Nothing in cna-examples can work
around it, because the unresolved symbols are inside a library this project only consumes.

**EXACT LOCATION AND FIX, found 2026-07-28 by source-reading (not applied -- read-only investigation
per the owner's instruction; the fix belongs in `../cna`, a separate repo).**
`../cna/cmake/CnaLibrary.cmake:50-54`:

```cmake
if(NOT CNA_FFMPEG_AVAILABLE)
    list(FILTER CNA_SOURCES EXCLUDE REGEX ".*/CNA/Internal/Media/VideoDecoder\\.cpp$")
    list(FILTER CNA_SOURCES EXCLUDE REGEX ".*/Media/Video/VideoPlayer\\.cpp$")
    list(FILTER CNA_SOURCES EXCLUDE REGEX ".*/Media/Video/Video\\.cpp$")
endif()
```

`CNA_FFMPEG_AVAILABLE` is forced `OFF` on Emscripten (`CnaLibrary.cmake:7-11`: `MINGW OR WIN32 OR
EMSCRIPTEN OR ANDROID`), so this block excludes `Video.cpp`/`VideoPlayer.cpp`/`VideoDecoder.cpp` from
`libCNA.a` -- but `src/CNA/Internal/Xnb/VideoContentTypeReader.cpp` lives at a different path
(`CNA/Internal/Xnb/...`, not `CNA/Internal/Media/...` or `Media/Video/...`) and matches none of the
three regexes, so it stays compiled in and still calls the real `Video(...)` constructor
(`VideoContentTypeReader.cpp:64,97`) whose definition lives only in the excluded `Video.cpp`. This is
a **simple oversight, not structural** -- `VideoContentTypeReader.cpp` is a single dedicated file,
not part of a shared glob with other readers, so excluding it is safe and risks no other content
type. **Precise one-line fix**, add after line 53 in the same `if`-block:

```cmake
list(FILTER CNA_SOURCES EXCLUDE REGEX ".*/CNA/Internal/Xnb/VideoContentTypeReader\\.cpp$")
```

(A consumer's `Video` content-load requests would then need to fail gracefully rather than hit an
"unregistered reader" error on that platform -- a design detail for whoever applies this upstream.)
**Sibling check performed**: grepped every `EXCLUDE REGEX` filter in `cmake/*.cmake` against all 16
files under `src/CNA/Internal/Xnb/`. Video is the only FFmpeg/platform-gated content type there; no
other reader has an excluded-but-referenced implementation, so this does not need to be found again
piecemeal.

This is worth raising with the CNA maintainer (i.e. yourself, in `../cna`) as a one-line fix rather
than sitting on it -- it's precise enough to apply directly without re-investigating.

---

**Avatar animation content (found during Phase F1, 2026-07-28) -- `needs_human`: systemic
`.clip.bin` translation-channel defect in `../cna`'s avatar content, not a cna-examples bug.**

Full diagnosis and reproduction method are in §5 item 51 and `plan.md`'s Phase E section (item 4).
Summary: nearly every animated bone track in nearly every avatar animation clip (~60 `(clip, bone)`
pairs, both genders) has its `Translation` channel correct only on the first and last keyframe and
zeroed on 100% of interior keyframes, while `Rotation`/`Scale` are fine. The reader
(`ContentManager::ReadAnimationClipFileEXT` in `../cna/src/.../ContentManager.cpp`) was read in
full and is not the cause -- it does a straightforward sequential read of whatever is in the file.
**The `.clip.bin` files themselves, produced by `../cna/tools/avatar_builder/`, are the suspect.**

Reproduction: build a throwaway `.cpp` against `../cna`'s already-built `build/CNA_BUILD/libCNA.a`
(see the exact `g++`/flags recipe pattern in `tools/checks/math_claims.cpp`'s header comment -- same
approach, different includes), `ContentManager::Load<std::shared_ptr<SkinnedModelEXT>>(...)` a real
avatar, then for each clip/track print every keyframe's `Translation` alongside
`BindPoseLocal[track.BoneIndex]`. Any track whose bind-pose translation length exceeds ~0.02 and
whose interior keyframes read as `(0,0,0)` reproduces the defect immediately -- no rendering or
screenshot needed.

Not fixed here: the owner's 2026-07-28 instruction was investigate-and-document, not modify
`../cna`. Worth raising with the CNA maintainer (yourself) alongside D2/F2 above -- likely the
highest-impact of the three, since it visibly affects nearly the entire avatar animation library
rather than one code path.

---

Everything else in the roadmap is unblocked. The three that could
have been were settled up front and are recorded in §2: asset licensing, branch policy, and
depth-over-breadth.

**RESOLVED 2026-07-28, see §2a.** The judgement call below was flagged for the owner and answered:
push closer to `plan.md`'s projected counts going forward, superseding the depth-over-breadth
default. D3 (this session) already follows the new instruction. Content/Storage/Diagnostics
(below) were built under the OLD default and are not being retroactively revisited unless the
owner asks.

Historical record: every new area up to that point shipped **fewer screens than projected**
(Content 5 of 15, Storage 2 of 6, Diagnostics 4 of 13), because the projections counted API
surface rather than distinct demonstrations, and splitting a single idea across three screens
teaches nothing extra. Each omission is itemised in `plan.md`.

## 8. Resume here

Everything below is unblocked and needs no decision from the project owner. Take them in this
order; each is self-contained and ends in a commit.

**D3 — Model Content is DONE** (4 screens: Load & Traverse, EffectMaterial & Effect Swapping,
Skeletal Animation (AnimationPlayer), Morph Targets — see `plan.md`'s own D3 writeup and NEXT.md
§5 items 43–45 for the findings). It needed **no** borrowed asset in the end — everything is
synthesized procedurally via CNA's own `.cnj` content format, the same technique the CNA test suite
uses. `SkinnedModelEXT` was deliberately NOT put here; it went to Phase E instead (Avatar-only type).

**Phase E — Avatars is DONE** (3 categories, 7 screens: AvatarDescription's `CreateRandom & IsValid`
+ `Preset & BodyType Name Tables`; AvatarRenderer's `The Faithful (No-Op) XNA Surface` + `Real
Render (Male & Female)` + `Animation Preset Cycling`; Wardrobe's `Per-Slot Tinting
(AvatarAppearanceEXT)` + `Hot-Swap (AttachPartEXT/RemovePartEXT)` — see `plan.md`'s own Phase E
writeup and NEXT.md §5 items 46–51 for the findings, including one unresolved cosmetic issue, #51,
recorded rather than chased down). It DOES need real avatar mesh/wardrobe assets, borrowed from
`../cna/examples/demo_avatar/Content/` the same way as `.xnb`/XACT/D3.

**C4 Storage extension is DONE** (2 new screens: Directories & Files, Container Lifetime — see
`plan.md`'s C4 section for the full findings writeup). Two real findings verified live: `GetFileNames`/
`GetDirectoryNames` never recurse into subdirectories even though `CreateFile`/`OpenFile` happily
accept nested relative paths; and `StorageContainer::Dispose()` does not gate any further use of the
object at all (no method checks `IsDisposed`) — it only means "the `Disposing` event has fired."

**Phase F1 — defect sweep is DONE** (see plan.md's F1 writeup and NEXT.md §5 items 54–55). Two real
cna-examples defects found and fixed (a `DrawVerdict()`/`DrawLines()` overlap trap, and — nothing
else; search/breadcrumbs/API-footer integration and TODO/FIXME markers were all checked clean). One
major upstream finding: the "Stand2" cosmetic issue from Phase E turned out to be a systemic
`.clip.bin` content defect across ~60 track pairs in nearly the whole avatar animation library —
root-caused precisely, `needs_human`, not fixed here (see §7). Re-validated 247/247 on both backends
after the fixes.

**D2 — PbrEffect is now DONE too (2026-07-28, after F1).** F1's own report noted the fix was local to
cna-examples, not `../cna` — the owner's "no fixes in `../cna`" instruction didn't apply once the
cause turned out to live in this repo's own reverted screen. Fixed by repacking
`VertexPositionNormalTangentTexture` into a private packed POD before upload (its real `sizeof()` is
56, not the naive 48 the original code assumed — the hidden `IVertexType` vtable pointer). Shipped as
a new **PbrEffect** category in **3D Graphics > Effects Gallery**, verified live by pixel probe.
`docs/wip-pbr/` removed (superseded). See `plan.md`'s D2 section and §7 above for the full account.

**D2's `RenderPipelineSettings` follow-up is now DONE too (2026-07-28, same day).** The honesty
screen deferred from D2's original scope: `CNA_NOXNA` is now enabled in `CMakeLists.txt` (same
precedent as `CNA_DEVICES`), and `RenderPipelineSettingsScreen` proves the settings bag is a real,
faithful store (every property round-trips exactly) that nothing in this backend consumes (verified
by a fresh full-tree grep of `../cna`, not assumed from the old investigation). Amber verdict, same
honest spirit as D8's. See §5 item 57 for the `CNA_NOXNA` finding.

**D2's `PbrMaterial` finding is now RESOLVED too (2026-07-28, same day, third follow-up).** The
incidental discrepancy from the `RenderPipelineSettings` follow-up (an old, stale D2 note claiming
"there is no `PbrMaterial` type") has been investigated properly: `PbrMaterial` is real but is the
same "faithful store, zero readers" situation as `RenderPipelineSettings` -- neither `PbrEffect` nor
CNA's real glTF content pipeline ever constructs one; both set `PbrEffect`'s own texture/factor
properties directly instead. Folded into `PbrMetallicRoughnessScreen.hpp` as a verified round trip
plus one on-screen line rather than a third thin screen — no new screen registered, count stays at
**249**. See §5 item 58 for the full citation trail (including the misleadingly-named
`RuntimeGltfModelTest.LoadsPbrMaterialWithAllFourMapsAndFactorsFromGltf`, which does NOT touch
`PbrMaterial` despite its name).

**D8 Net's amber verdict is now DEFINITIVELY diagnosed (2026-07-28), not just "not yet made
conclusive."** The old "two local gamers" follow-up idea was WRONG and has been disproven, not just
left untried: `NetworkSessionType::Local` gates its entire `PacketSend` path off unconditionally
(`RealNetworkingEnabled` is `SystemLink`-only), so no number of local gamers can ever make it
deliver. The only real path (SystemLink) needs a second real process and isn't reachable from one
self-contained screen through the public API. See §5 item 59 for the full citation trail.
`SimulatedConditionsScreen.hpp`'s comments/on-screen text now state this precisely — **do not
re-attempt "add a second local gamer," it has been tried and conclusively fails.**

**249/249 re-verified on both backends after all three follow-ups (RenderPipelineSettings, PbrMaterial,
D8 diagnosis).**

**The roadmap is now substantially complete.** Every phase through D2/F1/F3 is DONE, including all
three of D2's small follow-ups and D8's diagnosis correction; only F2 (Emscripten) remains,
`needs_human`-blocked on a defect inside `../cna` itself, precisely diagnosed (exact one-line fix
location identified, see §7) but deliberately not applied here per the owner's 2026-07-28 instruction
to keep `../cna` untouched this session. **Do NOT start F2** without reading §7 first, and do not
modify `../cna` without new authorization from the owner.

**Next unblocked work, if this session continues**, per the general autonomous-work mandate (do not
stop merely because the planned roadmap is done — reassess for further safe, valuable work): a fresh
audit pass in the spirit of F1 but broader than "this session's additions" — e.g. a TODO/FIXME/stub
sweep across the FULL `src/` tree (F1 only checked the areas added this session), a compiler-warnings
pass (`-Wall -Wextra` if not already the default), or revisiting Phase E's Stand2 animation-content
defect once `../cna` gets attention (it is `needs_human`, see §7 — do not attempt to fix `.clip.bin`
content from this repo).

**Before writing any code for a phase**, grep `src/Demos/` for the APIs its plan row claims are
missing. D6, D7, D8 and D3 all shrank once that was checked (or, for D3, once the architecture was
actually understood) — the plan over-estimates gaps, and the existing 249 screens already cover
more than it assumes.

**Workflow that works here, in order:**

1. Read the CNA header first and build against what is actually there.
2. Syntax-check a new screen with a throwaway TU before touching `AreaCatalog.hpp`
   (`g++ -std=c++23 -fsyntax-only $DEFS $INCLUDES /tmp/tu.cpp`, taking the flags from
   `build/CMakeFiles/cna_examples.dir/flags.make`). A full rebuild per iteration is far slower.
3. Register it, build, and run just that demo through `tools/headless.sh` with `--screenshot`.
4. **Verify by measurement, not by "it rendered".** Give any screen that makes a checkable claim a
   `DrawVerdict()` swatch and assert its colour with a pixel probe; that is what caught the
   `RenderTargetCube` and `SurfaceFormat` findings.
5. Update `README.md`, `plan.md` and `NEXT.md` counts, then `check_catalog.py` + `check_layout.py`.
6. Sweep the new area alone (`./tools/sweep.sh "Area/"`), then both backends in full.
7. Commit with the finding in the message, and push.

Keep going to the next item without stopping to summarise. Stopping after each phase to write a
status report was the main process failure of the last session; the owner had to re-prompt three
times.

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
