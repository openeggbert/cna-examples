#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Report layout problems in the screenshots produced by tools/sweep.sh.

The sweep proves every demo renders without crashing; this proves the result is
actually readable. It reports, per screenshot:

  blank    -- almost nothing was drawn (a screen that "works" but shows nothing)
  right    -- ink touching the right edge, i.e. text running out of the window

There is deliberately no "content overlapping the bottom chrome" check here any
more. It existed, and it found three real bugs (3D demos drawing scene labels
through the Back hint). It stopped being able to discriminate once the API
footer was added: the bottom row is now legitimately occupied by chrome on every
single screen, and a legitimate last content line's ink reaches into the same
band, so the test flagged all 174 screenshots. Pixels cannot tell "two things
drawn in the same place" from "one thing drawn there".

That failure mode is caught at its cause instead, by tools/check_layout.py,
which flags demo source drawing text at a hardcoded Y near the bottom of the
window rather than clamping to DemoScreen::LabelBaselineLimit().

Usage: tools/check_shots.py <directory-of-pngs> [--quiet]
"""

import sys
from pathlib import Path

from PIL import Image

# Anything at or above this is "ink" against the app's black background.
INK = 40
# Columns/rows this close to an edge count as touching it.
EDGE = 3



def analyse(path: Path) -> dict:
    image = Image.open(path).convert("L")
    width, height = image.size
    pixels = image.load()

    ink_total = 0
    right_edge = 0

    for y in range(height):
        for x in range(width):
            if pixels[x, y] >= INK:
                ink_total += 1
                if x >= width - EDGE:
                    right_edge += 1

    return {
        "size": (width, height),
        "ink_ratio": ink_total / float(width * height),
        "right_edge": right_edge,
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
