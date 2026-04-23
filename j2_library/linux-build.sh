#!/usr/bin/env bash
set -euo pipefail

# Initial value: install directory
INSTALL_DIR="/home/j2/dev/lib/j2_library"

# Remove if it exists
if [ -e "$INSTALL_DIR" ]; then
    rm -rf "$INSTALL_DIR"
fi

# CMake configuration (Release, Ninja, set install prefix)
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"

# Build (parallel: number of CPU cores)
cmake --build build -j"$(nproc)"

# Install
cmake --install build
