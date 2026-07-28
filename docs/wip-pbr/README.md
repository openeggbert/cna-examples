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
implemented in EasyGL with a real metallic-roughness BRDF shader), the seven things tried and ruled
out, and the one remaining untested lead: `../cna/examples/easygl_pbreffect_golden_test.cpp`
renders correctly and differs from this screen in exactly one respect — it uses **identity**
World/View/Projection with vertices already in NDC, where this screen sets a real camera. Verify
whether `PbrEffect`'s matrix setters reach the shader at all before assuming anything else.

Two API facts from the attempt are already committed to `NEXT.md` (#2, #3 of the D2 entry) and hold
regardless of whether this screen is ever revived:

* Tangent vertex types have **no** typed `DrawUserIndexedPrimitives` or `VertexBuffer::SetData`
  overload, so they silently bind the untyped `const void*` path and draw garbage instead of
  failing. Use a `VertexBuffer` plus `SetDataRaw(data, count, stride)`.
* `VertexPositionNormalTangentTexture::Tangent` is a `Vector4`; W carries glTF bitangent handedness.
