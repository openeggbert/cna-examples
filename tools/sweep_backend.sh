#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
#
# Sweep a NON-default build tree, e.g. the SDL_RENDERER one. tools/sweep.sh
# always uses build/; this takes the tree as its first argument so a second
# backend can be verified without disturbing the primary build.
#
# Usage:
#   tools/sweep_backend.sh build-sdlrenderer            # every demo
#   tools/sweep_backend.sh build-sdlrenderer "3D "      # only matching demos
#   OUT=/tmp/shots tools/sweep_backend.sh build-sdlrenderer
#
# Exit status is non-zero if any demo failed to produce a screenshot.

set -uo pipefail

# Same reason as tools/headless.sh: xvfb-run alone is not enough, because SDL3
# prefers Wayland whenever WAYLAND_DISPLAY is set and then ignores DISPLAY.
export SDL_VIDEODRIVER=x11
unset WAYLAND_DISPLAY

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-}"
FILTER="${2:-}"
FRAMES="${FRAMES:-90}"

if [[ -z "$BUILD_DIR" ]]; then
    echo "usage: tools/sweep_backend.sh <build-dir> [filter]" >&2
    exit 2
fi

BUILD="$ROOT/$BUILD_DIR"
OUT="${OUT:-$BUILD/screenshots}"

if [[ ! -x "$BUILD/cna_examples" ]]; then
    echo "no cna_examples in $BUILD -- configure and build it first, e.g.:" >&2
    echo "  cmake -S . -B $BUILD_DIR -DCNA_GRAPHICS_BACKEND=SDL_RENDERER \\" >&2
    echo "    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache" >&2
    echo "  cmake --build $BUILD_DIR -j4 --target cna_examples" >&2
    exit 1
fi

mkdir -p "$OUT"

# Clear stale output first. A renamed or deleted demo otherwise leaves its old
# .png behind forever, so check_shots.py counts more screenshots than there are
# demos and a removed screen looks like it is still passing. Only a full
# (unfiltered) sweep clears, since a filtered one is not authoritative.
if [[ -z "$FILTER" ]]; then
    rm -f "$OUT"/*.png "$OUT"/*.log
fi
cd "$BUILD"

mapfile -t DEMOS < <(./cna_examples --list-demos | { [[ -n "$FILTER" ]] && grep -F "$FILTER" || cat; })
if [[ ${#DEMOS[@]} -eq 0 ]]; then
    echo "no demos matched '$FILTER'" >&2
    exit 1
fi

echo "Sweeping ${#DEMOS[@]} demo(s) from $BUILD_DIR into $OUT"
failures=0

for path in "${DEMOS[@]}"; do
    safe="${path//\//__}"
    safe="${safe// /_}"
    png="$OUT/$safe.png"
    log="$OUT/$safe.log"

    if xvfb-run -a ./cna_examples --demo "$path" --frames "$FRAMES" --screenshot "$png" \
            > "$log" 2>&1 && [[ -s "$png" ]]; then
        printf '  ok   %s\n' "$path"
    else
        printf '  FAIL %s   (see %s)\n' "$path" "$log"
        failures=$((failures + 1))
    fi
done

echo
if [[ $failures -eq 0 ]]; then
    echo "All ${#DEMOS[@]} demo(s) rendered."
else
    echo "$failures of ${#DEMOS[@]} demo(s) failed."
fi
exit $(( failures > 0 ))
