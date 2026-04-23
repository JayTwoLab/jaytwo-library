#!/usr/bin/env bash
set -euo pipefail

# Base install directory (no surrounding quotes in value)
INSTALL_BASE="/home/jaytwo/dev/lib/j2_library"
INSTALL_DEBUG="$INSTALL_BASE/debug"
INSTALL_RELEASE="$INSTALL_BASE/release"

# Remove previous install tree if it exists
if [ -e "$INSTALL_BASE" ]; then
    rm -rf "$INSTALL_BASE"
fi

# Configure, build and install Debug (separate build dir for single-config generator)
cmake -S . -B build-debug -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="$INSTALL_DEBUG"
cmake --build build-debug -j"$(nproc)"
cmake --install build-debug

# Configure, build and install Release
cmake -S . -B build-release -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_RELEASE"
cmake --build build-release -j"$(nproc)"
cmake --install build-release
