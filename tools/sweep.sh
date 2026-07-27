#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
#
# Screenshot every demo (or the ones matching a filter) headlessly under Xvfb,
# using the app's own --demo/--screenshot driver. Used for the verification
# sweeps described in plan.md Phase F1.
#
# Usage:
#   tools/sweep.sh                       # every demo
#   tools/sweep.sh Media                 # only demos whose path contains "Media"
#   OUT=/tmp/shots tools/sweep.sh Input  # choose the output directory
#
# Exit status is non-zero if any demo failed to produce a screenshot.

set -uo pipefail

# Force SDL onto X11 and hide any Wayland session from it. Without this, SDL3
# picks the Wayland video driver whenever WAYLAND_DISPLAY is set and ignores the
# DISPLAY that xvfb-run exports -- so every "headless" run would open a real
# window on the developer's actual desktop. Exported here rather than left to
# each caller, because getting it wrong is silent and extremely annoying.
export SDL_VIDEODRIVER=x11
unset WAYLAND_DISPLAY

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="$ROOT/build"
OUT="${OUT:-$ROOT/build/screenshots}"
FILTER="${1:-}"
FRAMES="${FRAMES:-90}"

if [[ ! -x "$BUILD/cna_examples" ]]; then
    echo "cna_examples not built -- run: cmake --build build -j4 --target cna_examples" >&2
    exit 1
fi

mkdir -p "$OUT"
cd "$BUILD"

mapfile -t DEMOS < <(./cna_examples --list-demos | { [[ -n "$FILTER" ]] && grep -F "$FILTER" || cat; })

if [[ ${#DEMOS[@]} -eq 0 ]]; then
    echo "no demos matched '$FILTER'" >&2
    exit 1
fi

echo "Sweeping ${#DEMOS[@]} demo(s) into $OUT"
failures=0

for path in "${DEMOS[@]}"; do
    # One file per demo, named after its path with separators flattened.
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
