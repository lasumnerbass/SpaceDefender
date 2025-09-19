#!/bin/bash

# Unified build script for CMake and Xcode compatibility
# This script ensures the project builds correctly in all environments

set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_NAME="SpaceDefender"

echo "🔨 Building ${PROJECT_NAME}..."

# Build with CMake
echo "📦 CMake build..."
cd "${PROJECT_DIR}"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "✅ Build complete!"
echo "🎮 Run game: ./build/${PROJECT_NAME}"
echo "🔧 Open Xcode: open ${PROJECT_NAME}.xcodeproj"
