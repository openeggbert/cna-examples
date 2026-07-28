# Phase D2 (PbrEffect) — preserved work-in-progress

This directory is **not part of the build**. It holds the reverted D2 attempt so a later session
does not have to rewrite it from scratch. `check_catalog.py` only scans `src/Demos/**/*Screen.hpp`,
so nothing here is registered or counted.

| File | What it is |
|---|---|
| `MetallicRoughnessScreen.hpp.txt` | The screen as it stood when reverted. Compiles; renders nothing. Saved as `.txt` so it cannot be picked up by an include glob. |
| `sphere-helper.patch` | `git diff` adding `BuildSphereTangentMesh()` to `src/Demos/Graphics3D/Geometry3DHelpers.hpp`. This part is believed **correct** and is independently useful. |

## Why it was reverted

The screen never rendered geometry and the cause was not found. Shipping a demo that draws nothing
is worse than not shipping it, so the tree was restored to its verified state.

**Read `NEXT.md` §7 before touching this.** It lists what was established (PbrEffect *is*
implemented in EasyGL with a real metallic-roughness BRDF shader), the five things tried and ruled
out, and — as of a 2026-07-28 follow-up investigation — a source-verified **root cause** (NOT yet
applied or re-measured live). **Do not re-chase the camera-matrix lead**: it was investigated and
ruled out, the WVP pipeline is mechanically correct.

**The actual bug**: `VertexPositionNormalTangentTexture` is polymorphic (inherits `IVertexType`,
which adds a hidden 8-byte vtable pointer), so its real `sizeof()` is **56**, not the 48 this
screen's `SetDataRaw(flat_.data(), count, sizeof(VertexPositionNormalTangentTexture))` call (line
~93-95 below) assumed. That's the same vtable-inflation bug already documented elsewhere in CNA for
`VertexPositionColor`/`VertexPositionNormalTexture` — just without a typed `SetData` overload here to
hide it via repacking. A stride of 56 makes `EasyGLGraphicsBackend::ApplyLayout` select the
*skinned*-vertex layout instead, so `aPos` reads bytes starting at the vtable pointer as float
position data — degenerate/NaN clip positions, i.e. exactly "draws nothing". See `NEXT.md` §7 item 6
for full file:line citations.

**Before touching anything else**, confirm with one line: `static_assert(sizeof
(VertexPositionNormalTangentTexture) == 48);` should FAIL to compile. If it does, the fix is: define
a private, non-polymorphic packed GPU-vertex struct (mirror `easygl_pbreffect_golden_test.cpp`'s own
`PbrGpuVertex`, or CNA's `VertexPositionColor`-repack pattern in `VertexBuffer.cpp:54-70`) and upload
*that* via `SetDataRaw(..., 48)` instead of the real `VertexPositionNormalTangentTexture` array.
`BuildSphereTangentMesh()` (`sphere-helper.patch`) stays reusable as-is — only the upload step needs
to change to repack its output first.

Two API facts from the attempt are already committed to `NEXT.md` (#2, #3 of the D2 entry) and hold
regardless of whether this screen is ever revived:

* Tangent vertex types have **no** typed `DrawUserIndexedPrimitives` or `VertexBuffer::SetData`
  overload, so they silently bind the untyped `const void*` path and draw garbage instead of
  failing. Use a `VertexBuffer` plus `SetDataRaw(data, count, stride)`.
* `VertexPositionNormalTangentTexture::Tangent` is a `Vector4`; W carries glTF bitangent handedness.
