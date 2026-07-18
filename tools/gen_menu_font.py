#!/usr/bin/env python3
"""Bakes Content/menufont.png + Content/menufont.cnj (a CNA SpriteFont .cnj
descriptor, see cna's cnj.md / ContentManager.cpp SpriteFontTypeReader) from a
system TrueType font, and writes a small Content/blank.png (a single white
pixel, used by ScreenManager::FadeBackBufferToBlack).

Requires Pillow (`pip install pillow`) and a TrueType font on the system
(DejaVu Sans, permissively licensed, is present on most Linux distros).

Usage:
    python3 tools/gen_menu_font.py [--font /path/to/font.ttf] [--size 24]

Run from the repo root. Regenerate whenever the covered character set or
point size needs to change; the output is checked into Content/ so a normal
build does not need Pillow or a font installed.
"""

from __future__ import annotations

import argparse
import json
import os

from PIL import Image, ImageDraw, ImageFont

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
CONTENT_DIR = os.path.join(REPO_ROOT, "Content")

DEFAULT_FONT_CANDIDATES = [
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
]

FIRST_CHAR = 32
LAST_CHAR = 126
PADDING = 2


def find_default_font() -> str:
    for candidate in DEFAULT_FONT_CANDIDATES:
        if os.path.isfile(candidate):
            return candidate
    raise SystemExit("No default font found; pass --font explicitly.")


def bake(font_path: str, size: int) -> None:
    font = ImageFont.truetype(font_path, size)
    ascent, descent = font.getmetrics()
    line_spacing = ascent + descent

    chars = [chr(c) for c in range(FIRST_CHAR, LAST_CHAR + 1)]

    # Measure every glyph first so we can lay out a reasonably compact atlas.
    metrics = {}
    for ch in chars:
        bbox = font.getbbox(ch)  # (x0, y0, x1, y1) or None-ish for space
        advance = font.getlength(ch)
        if bbox is None:
            bbox = (0, 0, 0, 0)
        metrics[ch] = (bbox, advance)

    # Simple row-packed atlas: fixed cell size = max glyph box, N columns.
    max_w = max((b[2] - b[0]) for b, _ in metrics.values() if b[2] > b[0]) if any(
        b[2] > b[0] for b, _ in metrics.values()) else size
    cols = 16
    rows = (len(chars) + cols - 1) // cols
    cell_w = max_w + PADDING * 2
    cell_h = line_spacing + PADDING * 2
    atlas_w = cell_w * cols
    atlas_h = cell_h * rows

    atlas = Image.new("RGBA", (atlas_w, atlas_h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(atlas)

    glyphs = []
    for i, ch in enumerate(chars):
        col = i % cols
        row = i // cols
        cell_x = col * cell_w
        cell_y = row * cell_h

        bbox, advance = metrics[ch]
        gw = bbox[2] - bbox[0]
        gh = bbox[3] - bbox[1]

        if ch == " " or gw <= 0 or gh <= 0:
            glyphs.append({
                "char": ord(ch),
                "source": [0, 0, 0, 0],
                "crop": [0, 0, 0, 0],
                "kerning": [0.0, float(round(advance)), 0.0],
            })
            continue

        # Draw the glyph so its bbox top-left lands at (cell_x+PADDING, cell_y+PADDING).
        draw.text((cell_x + PADDING - bbox[0], cell_y + PADDING - bbox[1]),
                   ch, font=font, fill=(255, 255, 255, 255))

        glyphs.append({
            "char": ord(ch),
            "source": [cell_x + PADDING, cell_y + PADDING, gw, gh],
            # crop.Y is the vertical offset from the pen's baseline-relative origin;
            # XNA convention: offset from the top of the line to the glyph's bbox.
            "crop": [0, bbox[1] + ascent, gw, gh],
            "kerning": [0.0, float(round(advance)), 0.0],
        })

    os.makedirs(CONTENT_DIR, exist_ok=True)
    # Deliberately a different base name than the .cnj descriptor itself:
    # ContentManager::ResolveAssetPath always tries "<name>.cnj" before any
    # reader-declared extension (cnj.md's "core rule"), so Load<Texture2D>
    # ("menufont") would otherwise resolve to this SpriteFont's own .cnj
    # file and fail with a Texture2D/SpriteFont type mismatch.
    atlas_path = os.path.join(CONTENT_DIR, "menufont_atlas.png")
    atlas.save(atlas_path)

    cnj = {
        "cnjVersion": 1,
        "type": "SpriteFont",
        "texture": "menufont_atlas",
        "lineSpacing": line_spacing,
        "spacing": 0.0,
        "defaultCharacter": "?",
        "glyphs": glyphs,
    }
    cnj_path = os.path.join(CONTENT_DIR, "menufont.cnj")
    with open(cnj_path, "w", encoding="utf-8") as fh:
        json.dump(cnj, fh, indent=2)
        fh.write("\n")

    blank = Image.new("RGBA", (1, 1), (255, 255, 255, 255))
    blank.save(os.path.join(CONTENT_DIR, "blank.png"))

    print(f"wrote {atlas_path} ({atlas_w}x{atlas_h}), {cnj_path} "
          f"({len(glyphs)} glyphs), and Content/blank.png")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--font", default=None, help="Path to a .ttf file")
    ap.add_argument("--size", type=int, default=24, help="Point size to bake")
    args = ap.parse_args()

    font_path = args.font or find_default_font()
    bake(font_path, args.size)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
