#!/bin/bash
cd "$(dirname "$0")"
mkdir -p build
cd build
cmake ..
make
echo "Build complete. Run ./build/SpaceDefender to play!"
