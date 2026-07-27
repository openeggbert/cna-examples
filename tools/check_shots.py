#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Report layout problems in the screenshots produced by tools/sweep.sh.

The sweep proves every demo renders without crashing; this proves the result is
actually readable. It reports, per screenshot:

  blank    -- almost nothing was drawn (a screen that "works" but shows nothing)
  right    -- ink touching the right edge, i.e. text running out of the window
  bottom   -- ink below the Back hint's row, i.e. content overlapping the chrome
  overlap  -- the "... (N more lines)" marker fired (checked by the caller via
              the demo's own output, not detectable from pixels alone)

Usage: tools/check_shots.py <directory-of-pngs> [--quiet]
"""

import sys
from pathlib import Path

from PIL import Image

# Anything at or above this is "ink" against the app's black background.
INK = 40
# Columns/rows this close to an edge count as touching it.
EDGE = 3

# Demos whose 3D scene legitimately fills the viewport all the way to the bottom
# edge, so the "content in the chrome band" heuristic cannot apply. Each entry
# was checked once by hand: the bottom-band ink is coloured geometry (75%+
# saturated pixels), not greyscale chrome text.
#
# Kept as an explicit list rather than by loosening the heuristic, so a genuine
# text overflow in one of these demos would still have to be re-justified here.
BOTTOM_ALLOWED = {
    "3D_Graphics__Primitives_&_Vertex_Types__Buffers__VertexDeclaration_Stride_Gotcha.png",
}


def analyse(path: Path) -> dict:
    image = Image.open(path).convert("L")
    width, height = image.size
    pixels = image.load()

    ink_total = 0
    right_edge = 0
    bottom_rows = 0

    # DemoScreen draws the "< Back" hint at height-40, left-aligned and roughly
    # 340px wide. Anything drawn in that band to the RIGHT of it is a demo's own
    # content spilling into the chrome. Restricting the x range matters: without
    # it every screenshot flags itself, because the Back hint is always there.
    back_band_top = height - 44
    back_hint_right = 400

    for y in range(height):
        row_ink = 0
        spill_ink = 0
        for x in range(width):
            if pixels[x, y] >= INK:
                row_ink += 1
                if x >= width - EDGE:
                    right_edge += 1
                if x >= back_hint_right:
                    spill_ink += 1
        ink_total += row_ink
        if y >= back_band_top and spill_ink > 0:
            bottom_rows += 1

    return {
        "size": (width, height),
        "ink_ratio": ink_total / float(width * height),
        "right_edge": right_edge,
        "bottom_rows": bottom_rows,
    }


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__)
        return 2

    directory = Path(sys.argv[1])
    quiet = "--quiet" in sys.argv
    shots = sorted(directory.glob("*.png"))
    if not shots:
        print(f"no screenshots in {directory}", file=sys.stderr)
        return 2

    problems = 0
    for shot in shots:
        stats = analyse(shot)
        flags = []
        if stats["ink_ratio"] < 0.002:
            flags.append("blank")
        if stats["right_edge"] > 0:
            flags.append(f"right({stats['right_edge']}px)")
        if stats["bottom_rows"] > 0 and shot.name not in BOTTOM_ALLOWED:
            flags.append(f"bottom({stats['bottom_rows']}rows)")

        if flags:
            problems += 1
            print(f"  {shot.name}: {' '.join(flags)}")
        elif not quiet:
            print(f"  {shot.name}: ok (ink {stats['ink_ratio']:.3f})")

    print()
    print(f"{len(shots)} screenshot(s), {problems} with layout problems")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
