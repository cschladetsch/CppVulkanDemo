#!/bin/bash

# Build script for CppVulkanDemo - Linux/macOS

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"
BUILD_TYPE="Release"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 1)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse command line arguments
AUTO_DOWNLOAD_VULKAN=0
VULKAN_INSTALL_DIR=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --clean)
            print_info "Cleaning build directory..."
            rm -rf "$BUILD_DIR"
            print_info "Build directory cleaned."
            exit 0
            ;;
        --jobs)
            JOBS="$2"
            shift 2
            ;;
        --auto-download-vulkan)
            AUTO_DOWNLOAD_VULKAN=1
            shift
            ;;
        --vulkan-install-dir)
            VULKAN_INSTALL_DIR="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug                  Build in Debug mode (default: Release)"
            echo "  --release                Build in Release mode (default)"
            echo "  --clean                  Clean build directory and exit"
            echo "  --jobs N                 Number of parallel jobs (default: auto-detect)"
            echo "  --auto-download-vulkan   Automatically download and install Vulkan SDK if not found"
            echo "  --vulkan-install-dir DIR Directory to install Vulkan SDK (default: ~/VulkanSDK)"
            echo "  -h, --help               Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Check for required tools
print_info "Checking for required tools..."

if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed. Please install CMake 3.7 or higher."
    exit 1
fi

if ! command -v git &> /dev/null; then
    print_error "Git is not installed. Please install Git."
    exit 1
fi

# Check CMake version
CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d'.' -f1)
CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d'.' -f2)

if [[ $CMAKE_MAJOR -lt 3 ]] || ([[ $CMAKE_MAJOR -eq 3 ]] && [[ $CMAKE_MINOR -lt 7 ]]); then
    print_error "CMake 3.7 or higher is required. Current version: $CMAKE_VERSION"
    exit 1
fi

# Check for Vulkan SDK
print_info "Checking for Vulkan SDK..."

if [[ -z "$VULKAN_SDK" ]]; then
    print_warning "VULKAN_SDK environment variable is not set."
    print_info "Attempting to find Vulkan SDK..."
    
    # Try common locations
    if [[ -d "/usr/local/vulkan" ]]; then
        export VULKAN_SDK="/usr/local/vulkan"
    elif [[ -d "$HOME/VulkanSDK" ]]; then
        # Find the latest version
        LATEST_SDK=$(ls -d $HOME/VulkanSDK/*/ 2>/dev/null | sort -V | tail -n1)
        if [[ -n "$LATEST_SDK" ]]; then
            export VULKAN_SDK="${LATEST_SDK%/}/x86_64"
        fi
    fi
    
    if [[ -z "$VULKAN_SDK" ]]; then
        if [[ $AUTO_DOWNLOAD_VULKAN -eq 1 ]]; then
            print_warning "Vulkan SDK not found. Delegating to Python build script for automatic download..."
            
            # Build command for Python script
            PYTHON_CMD="python3 $SCRIPT_DIR/build.py --auto-download-vulkan"
            
            # Add optional install directory if specified
            if [[ -n "$VULKAN_INSTALL_DIR" ]]; then
                PYTHON_CMD="$PYTHON_CMD --vulkan-install-dir '$VULKAN_INSTALL_DIR'"
            fi
            
            # Add other options
            if [[ "$BUILD_TYPE" == "Debug" ]]; then
                PYTHON_CMD="$PYTHON_CMD --debug"
            fi
            
            PYTHON_CMD="$PYTHON_CMD --jobs $JOBS"
            
            # Execute Python build script
            eval $PYTHON_CMD
            exit $?
        else
            print_error "Vulkan SDK not found. Please install it from https://www.lunarg.com/vulkan-sdk/"
            print_error "After installation, set the VULKAN_SDK environment variable."
            print_error "Or run with --auto-download-vulkan to download and install automatically."
            exit 1
        fi
    fi
fi

print_info "Using Vulkan SDK: $VULKAN_SDK"

# Initialize submodules
print_info "Initializing git submodules..."
cd "$SCRIPT_DIR"
git submodule update --init --recursive

# Create build directory
print_info "Creating build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
print_info "Configuring with CMake (Build type: $BUILD_TYPE)..."

# Platform-specific CMake options
CMAKE_OPTIONS=""

if [[ "$(uname)" == "Darwin" ]]; then
    print_info "Detected macOS platform"
    # macOS specific options
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_OSX_DEPLOYMENT_TARGET=10.12"
    
    # Check for MoltenVK
    if [[ ! -d "/usr/local/include/MoltenVK" ]]; then
        print_warning "MoltenVK not found at /usr/local/include/MoltenVK"
        print_warning "The CMakeLists.txt expects MoltenVK at this location."
        print_warning "You may need to modify CMakeLists.txt line 11 to point to your MoltenVK installation."
    fi
fi

cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    $CMAKE_OPTIONS

# Build
print_info "Building with $JOBS parallel jobs..."
cmake --build . --config $BUILD_TYPE -j $JOBS

# Summary
print_info "Build completed successfully!"
print_info "Build type: $BUILD_TYPE"
print_info "Build directory: $BUILD_DIR"

# Check for built library
if [[ -f "$BUILD_DIR/libvtb.a" ]] || [[ -f "$BUILD_DIR/vtb.lib" ]] || [[ -f "$BUILD_DIR/$BUILD_TYPE/vtb.lib" ]]; then
    print_info "Library 'vtb' built successfully."
else
    print_warning "Could not find built library. Check build output for errors."
fi

print_info "To enable example builds, uncomment the subdirectories in Example/CMakeLists.txt"