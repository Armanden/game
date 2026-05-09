#!/bin/bash
set -e

cd "$(dirname "$0")"

gcc -o game main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lasound -lX11 -lXrandr -lXinerama -lXcursor

echo "Build complete: ./game"
