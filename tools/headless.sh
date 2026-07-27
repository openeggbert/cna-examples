#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
#
# Run cna_examples on a virtual display, never the real desktop.
#
#   tools/headless.sh --demo "Media/Pictures/Browse" --frames 90 --screenshot /tmp/a.png
#
# Why this wrapper exists: xvfb-run alone is NOT enough. SDL3 chooses the
# Wayland video driver whenever WAYLAND_DISPLAY is set, and then ignores the
# DISPLAY that xvfb-run exports -- so the window opens on the developer's real
# desktop while the command still looks headless. Forcing SDL_VIDEODRIVER=x11
# and unsetting WAYLAND_DISPLAY is what actually keeps it virtual.

set -uo pipefail

export SDL_VIDEODRIVER=x11
unset WAYLAND_DISPLAY

BUILD="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/build"

if [[ ! -x "$BUILD/cna_examples" ]]; then
    echo "cna_examples not built -- run: cmake --build build -j4 --target cna_examples" >&2
    exit 1
fi

cd "$BUILD"
exec xvfb-run -a ./cna_examples "$@"
