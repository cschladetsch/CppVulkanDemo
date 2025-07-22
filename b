#!/bin/bash

# Build script for CppVulkanDemo
# Pass any arguments directly to cmake/make

BUILD_DIR="build"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Change to build directory
cd "$BUILD_DIR"

# Run cmake with any passed arguments
cmake .. "$@"

# Build the project
make -j$(nproc)