#!/bin/bash

# Run script for CppVulkanDemo
# First builds the project using ./b, then runs the executable

# Build the project first, passing all arguments to ./b
./b "$@"

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Run the executable (without arguments since they were for the build)
# Assuming the executable is in build directory
if [ -f "build/CppVulkanDemo" ]; then
    ./build/CppVulkanDemo
elif [ -f "build/Example/CppVulkanDemo" ]; then
    ./build/Example/CppVulkanDemo
else
    echo "Error: Could not find executable"
    echo "Looking for executable in build/ and build/Example/"
    exit 1
fi