#!/bin/bash

BINARY="main"
BUILD_DIR="build"
OPTIMIZATION="-O0"
FLAGS="-Wall -Wextra"
BUILD_TYPE="Debug"

PASSTHROUGH=()
RUN_DEFAULT=true

# Parse args
# ./build.sh            → run default (no args to binary)
# ./build.sh other      → run with "other" as arg
# ./build.sh -- --help  → pass --help to binary
# ./build.sh pass --help→ same

for arg in "$@"; do
    if [[ "$arg" == "--" ]]; then
        shift
        PASSTHROUGH=("$@")
        break
    fi
    PASSTHROUGH+=("$arg")
    shift
done

mkdir -p "$BUILD_DIR"

# Configure (skip if cache exists)
if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    cmake_out=$(cmake -B "$BUILD_DIR"                        \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"                     \
        -DCMAKE_CXX_FLAGS="$OPTIMIZATION $FLAGS"             \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON                   \
        2>&1)

    if [[ $? -ne 0 ]]; then
        echo "$cmake_out"
        exit 1
    fi

    # keep compile_commands.json in project root for clangd
    ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json
fi

# Build
cmake --build "$BUILD_DIR" -j"$(nproc)"

if [[ $? -ne 0 ]]; then
    echo "Build failed."
    exit 1
fi

clear
"$BUILD_DIR/$BINARY" "${PASSTHROUGH[@]}"
