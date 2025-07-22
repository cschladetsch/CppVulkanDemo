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
# The executable is in the Bin directory
if [ -f "Bin/linux_demo" ]; then
    cd Bin && ./linux_demo
else
    echo "Error: Could not find executable"
    echo "Looking for executable in Bin/"
    exit 1
fi