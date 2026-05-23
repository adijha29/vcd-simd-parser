#!/usr/bin/env bash
set -e

FILEPATH=${1:-"split_chunk_1.vcd"}

if [ ! -f "$FILEPATH" ]; then
    echo "CRITICAL ERROR: Target file '$FILEPATH' does not exist."
    exit 1
fi

echo "=========================================="
echo "    EXECUTING UNIX CMAKE BUILD ENGINE     "
echo "=========================================="

mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd ..

echo -e "\n\033[32mLAUNCHING CONSOLIDATED MATRIX VIA LIBRARY IMPLEMENTATIONS...\033[0m\n"

# Capture OS resource metrics via time utility binary wrapper
/usr/bin/time -v ./build/apps/benchmark_runner "$FILEPATH"