#!/bin/bash
# snapshot.sh -- regenerate docs/screenshots/*.png from the REAL lesson code, on Linux or macOS.
#
# EasyX only exists on Windows, so include/emu.h is a tiny stand-in that records every
# drawing call as SVG instead of painting a window. Each game runs with a scripted
# keyboard (KEYS) and stops at a chosen frame (SNAP), or at the Nth _getch() (GETCH_SNAP).
# The SVG is then turned into a PNG with headless Chromium.
#
# Usage (from the repository root):   tools/easyx-snapshot/snapshot.sh
# Needs: g++, python3 with Pillow, and a Chromium binary (set CHROMIUM=/path/to/chromium).
set -e
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
INC="$ROOT/tools/easyx-snapshot/include"
OUT="$ROOT/docs/screenshots"
TMP=$(mktemp -d)
CHROMIUM=${CHROMIUM:-chromium}

shot() {   # shot NAME SOURCE WIDTH HEIGHT [VAR=VALUE ...]
    local name=$1 src=$2 w=$3 h=$4; shift 4
    g++ -std=gnu++17 -w -I"$INC" "$ROOT/$src" -o "$TMP/$name"
    env "$@" OUT="$TMP/$name.svg" "$TMP/$name" > /dev/null
    printf '<html><body style="margin:0;overflow:hidden">%s</body></html>' "$(cat "$TMP/$name.svg")" > "$TMP/$name.html"
    "$CHROMIUM" --headless --no-sandbox --hide-scrollbars --screenshot="$TMP/$name.raw.png" \
        --window-size=$((w + 50)),$((h + 250)) "file://$TMP/$name.html" > /dev/null 2>&1
    python3 -c "from PIL import Image; Image.open('$TMP/$name.raw.png').crop((0,0,$w,$h)).save('$OUT/$name.png', optimize=True)"
    echo "  $name.png"
}

echo "Rendering screenshots into docs/screenshots ..."
shot hello_shapes lessons/01-drawing-on-screen/hello_shapes.cpp         640 480 GETCH_SNAP=1
shot runner       lessons/02-game-loop-runner/runner_jump.cpp            800 400 SNAP=174 SEED=5 KEYS=SPACE:160-160
shot archery      lessons/03-many-objects-archery/balloon_archery.cpp    800 400 SNAP=30  SEED=4 KEYS=SPACE:5-5,SPACE:12-12,SPACE:19-19,SPACE:26-26
shot cannon       lessons/04-projectile-cannon/cannon_projectile.cpp     800 400 SNAP=124 SEED=2 KEYS=SPACE:98-98
shot step1        lessons/05-bomberman/step1_concrete_map.cpp            850 850 GETCH_SNAP=1
shot step2        lessons/05-bomberman/step2_random_bricks.cpp           850 850 GETCH_SNAP=1 SEED=3
shot step3        lessons/05-bomberman/step3_player_control.cpp          850 850 SNAP=36  SEED=3 KEYS=RIGHT:1-20,DOWN:21-35
shot triangle     lessons/06-collision-lab/triangle_bounce.cpp           850 850 SNAP=60
shot circle_rect  lessons/06-collision-lab/circle_rect_collision.cpp     850 850 SNAP=158 SEED=1
rm -rf "$TMP"
echo "Done."
