#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Flag demo source that lays text out against a hardcoded bottom-of-window Y.

Why this exists, concretely:

`menufont.cnj` declares `lineSpacing: 29`, but its glyph descriptors place ink as
far as 52px below the draw origin (crop offset up to 46 plus source height up to
24). `SpriteFont::MeasureString().Y` computes its result from `lineSpacing`, so
it also reports 29 -- measuring does not reveal the discrepancy. Any layout that
reasons "the window is 640 tall, the Back hint is at 600, so 560 is safe" is
therefore wrong by about 23px, and three 3D Graphics demos drew their scene
labels straight through the Back hint because of exactly that arithmetic.

The correct spelling is to clamp against `DemoScreen::LabelBaselineLimit(font)`,
which subtracts `GlyphExtent()` rather than `lineSpacing`.

This lint is a source check rather than a pixel check on purpose: once the demo
screens grew an API footer, the bottom row became legitimate chrome on every
screen, and no pixel heuristic can distinguish "content overlapping chrome" from
"chrome". See the note at the top of tools/check_shots.py.

Usage:
  tools/check_layout.py            # exit non-zero if anything is flagged
  tools/check_layout.py --list     # also print every Y literal it considered
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEMOS_DIR = ROOT / "src" / "Demos"

# Any Y at or beyond this is close enough to the bottom of the default 640px
# window to be at risk. Well above the ~538px real limit, so ordinary mid-screen
# layout is not flagged.
RISKY_Y = 520.0

# Vector2(<x>, <y>) with a literal y, e.g. Vector2(60.0f, 560.0f).
VECTOR2_LITERAL_RE = re.compile(r"Vector2\(\s*[^,()]+,\s*(\d+(?:\.\d+)?)f?\s*\)")

# A line is considered safe if the clamp appears in the same statement.
SAFE_MARKERS = ("LabelBaselineLimit", "getHeightProperty")


def statements(text: str):
    """Yield (line_number, statement_text). Statements may span lines, and demo
    draw calls routinely do, so a naive per-line scan would miss the clamp
    sitting on the previous line."""
    lines = text.splitlines()
    buffer = ""
    start = 1
    for number, line in enumerate(lines, start=1):
        if not buffer:
            start = number
        buffer += " " + line.strip()
        # Flush on braces too, not just on ';'. Otherwise an opening brace line
        # gets glued to the statement that follows it and the reported line
        # number points at the enclosing block instead of the offending call.
        if line.rstrip().endswith((";", "{", "}")):
            yield start, buffer
            buffer = ""
    if buffer:
        yield start, buffer


def main() -> int:
    if not DEMOS_DIR.is_dir():
        print("run this from a cna-examples checkout", file=sys.stderr)
        return 2

    show_all = "--list" in sys.argv
    problems: list[str] = []
    considered = 0

    for path in sorted(DEMOS_DIR.rglob("*.hpp")):
        text = path.read_text(encoding="utf-8")
        for number, statement in statements(text):
            if "DrawString" not in statement and "FillRect" not in statement:
                continue
            for match in VECTOR2_LITERAL_RE.finditer(statement):
                y = float(match.group(1))
                considered += 1
                if show_all:
                    print(f"  {path.relative_to(ROOT)}:{number}  y={y}")
                if y < RISKY_Y:
                    continue
                if any(marker in statement for marker in SAFE_MARKERS):
                    continue
                problems.append(
                    f"{path.relative_to(ROOT)}:{number}: draws at a hardcoded "
                    f"y={y:g}, near the bottom of a 640px window, without "
                    f"clamping to LabelBaselineLimit()"
                )

    if show_all:
        print()

    if problems:
        print(f"{len(problems)} problem(s):")
        for problem in problems:
            print(f"  - {problem}")
        return 1

    print(f"OK: {considered} literal draw position(s) checked, none at risk of "
          f"colliding with the bottom chrome.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
