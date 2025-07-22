#!/usr/bin/env python3
"""
Cross-platform build script for CppVulkanDemo
"""

import os
import sys
import argparse
import subprocess
import platform
import shutil
import urllib.request
import tarfile
import zipfile
from pathlib import Path


class Colors:
    """Terminal colors for output"""
    RED = '\033[0;31m'
    GREEN = '\033[0;32m'
    YELLOW = '\033[0;33m'
    BLUE = '\033[0;34m'
    NC = '\033[0m'  # No Color
    
    @staticmethod
    def disable():
        Colors.RED = ''
        Colors.GREEN = ''
        Colors.YELLOW = ''
        Colors.BLUE = ''
        Colors.NC = ''


def print_info(msg):
    print(f"{Colors.GREEN}[INFO]{Colors.NC} {msg}")


def print_warning(msg):
    print(f"{Colors.YELLOW}[WARNING]{Colors.NC} {msg}")


def print_error(msg):
    print(f"{Colors.RED}[ERROR]{Colors.NC} {msg}")


def print_debug(msg):
    print(f"{Colors.BLUE}[DEBUG]{Colors.NC} {msg}")


def check_command(cmd):
    """Check if a command is available"""
    return shutil.which(cmd) is not None


def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result"""
    print_debug(f"Running: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    result = subprocess.run(cmd, cwd=cwd, shell=isinstance(cmd, str))
    if check and result.returncode != 0:
        print_error(f"Command failed with exit code {result.returncode}")
        sys.exit(1)
    return result


def get_cmake_version():
    """Get CMake version as tuple of integers"""
    try:
        result = subprocess.run(['cmake', '--version'], capture_output=True, text=True)
        version_line = result.stdout.split('\n')[0]
        version_str = version_line.split()[-1]
        version_parts = version_str.split('.')
        return tuple(int(x) for x in version_parts[:2])
    except:
        return (0, 0)


def get_vulkan_download_info():
    """Get Vulkan SDK download URL and version based on platform"""
    system = platform.system()
    
    # Using Vulkan SDK 1.3.268.0 as a stable version
    version = "1.3.268.0"
    
    if system == 'Windows':
        filename = f"VulkanSDK-{version}-Installer.exe"
        url = f"https://sdk.lunarg.com/sdk/download/{version}/windows/{filename}?Human=true"
    elif system == 'Darwin':  # macOS
        filename = f"vulkansdk-macos-{version}.dmg"
        url = f"https://sdk.lunarg.com/sdk/download/{version}/mac/{filename}?Human=true"
    elif system == 'Linux':
        filename = f"vulkansdk-linux-x86_64-{version}.tar.xz"
        url = f"https://sdk.lunarg.com/sdk/download/{version}/linux/{filename}?Human=true"
    else:
        return None, None, None
    
    return url, filename, version


def download_file(url, dest_path, chunk_size=8192):
    """Download a file with progress indication"""
    try:
        # Add User-Agent header to avoid 403 errors
        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
        }
        request = urllib.request.Request(url, headers=headers)
        response = urllib.request.urlopen(request)
        total_size = int(response.headers.get('Content-Length', 0))
        
        downloaded = 0
        with open(dest_path, 'wb') as f:
            while True:
                chunk = response.read(chunk_size)
                if not chunk:
                    break
                f.write(chunk)
                downloaded += len(chunk)
                
                if total_size > 0:
                    percent = (downloaded / total_size) * 100
                    print(f"\rDownloading: {percent:.1f}%", end='', flush=True)
        
        print()  # New line after download
        return True
    except Exception as e:
        print_error(f"Download failed: {e}")
        return False


def install_vulkan_sdk(download_path, install_path):
    """Install Vulkan SDK from downloaded file"""
    system = platform.system()
    
    try:
        if system == 'Windows':
            # For Windows, run the installer silently
            print_info("Running Vulkan SDK installer...")
            cmd = [str(download_path), '/S', f'/D={install_path}']
            run_command(cmd)
        
        elif system == 'Linux':
            # Extract tar.xz file
            print_info("Extracting Vulkan SDK...")
            install_path.mkdir(parents=True, exist_ok=True)
            
            # Extract to temporary location first
            with tarfile.open(download_path, 'r:xz') as tar:
                tar.extractall(install_path.parent)
            
            # The SDK extracts to a versioned directory, rename it
            extracted_dir = install_path.parent / download_path.stem.replace('.tar', '')
            if extracted_dir.exists() and extracted_dir != install_path:
                if install_path.exists():
                    shutil.rmtree(install_path)
                extracted_dir.rename(install_path)
        
        elif system == 'Darwin':
            # For macOS, mount DMG and copy contents
            print_info("Mounting and installing Vulkan SDK...")
            mount_point = Path('/Volumes/VulkanSDK')
            
            # Mount the DMG
            run_command(['hdiutil', 'attach', str(download_path)])
            
            # Copy contents
            if mount_point.exists():
                install_path.parent.mkdir(parents=True, exist_ok=True)
                shutil.copytree(mount_point / 'VulkanSDK', install_path)
                
                # Unmount
                run_command(['hdiutil', 'detach', str(mount_point)])
        
        return True
    
    except Exception as e:
        print_error(f"Installation failed: {e}")
        return False


def find_vulkan_sdk():
    """Find Vulkan SDK location"""
    # First check environment variable
    sdk_path = os.environ.get('VULKAN_SDK')
    if sdk_path and os.path.exists(sdk_path):
        return sdk_path
    
    # Platform-specific search
    system = platform.system()
    
    if system == 'Windows':
        # Check common Windows locations
        vulkan_base = Path('C:/VulkanSDK')
        if vulkan_base.exists():
            versions = sorted([d for d in vulkan_base.iterdir() if d.is_dir()], reverse=True)
            if versions:
                return str(versions[0])
    
    elif system == 'Darwin':  # macOS
        # Check common macOS locations
        locations = [
            Path('/usr/local/vulkan'),
            Path.home() / 'VulkanSDK'
        ]
        for loc in locations:
            if loc.exists():
                if loc.name == 'VulkanSDK':
                    # Find latest version
                    versions = sorted([d for d in loc.iterdir() if d.is_dir()], reverse=True)
                    if versions:
                        return str(versions[0] / 'macOS')
                else:
                    return str(loc)
    
    elif system == 'Linux':
        # Check common Linux locations
        locations = [
            Path('/usr/local/vulkan'),
            Path.home() / 'VulkanSDK'
        ]
        for loc in locations:
            if loc.exists():
                if loc.name == 'VulkanSDK':
                    # Find latest version
                    versions = sorted([d for d in loc.iterdir() if d.is_dir()], reverse=True)
                    if versions:
                        return str(versions[0] / 'x86_64')
                else:
                    return str(loc)
    
    return None


def get_cpu_count():
    """Get number of CPU cores for parallel builds"""
    try:
        return os.cpu_count() or 1
    except:
        return 1


def get_vs_generator():
    """Detect Visual Studio version and return appropriate generator"""
    # Try to detect installed VS versions
    vs_versions = [
        ('2022', 'Visual Studio 17 2022'),
        ('2019', 'Visual Studio 16 2019'),
        ('2017', 'Visual Studio 15 2017'),
    ]
    
    for year, generator in vs_versions:
        # Check if VS is installed (simplified check)
        vswhere_path = Path(os.environ.get('ProgramFiles(x86)', 'C:/Program Files (x86)')) / 'Microsoft Visual Studio/Installer/vswhere.exe'
        if vswhere_path.exists():
            result = subprocess.run([str(vswhere_path), '-version', f'[16.0,18.0)'], capture_output=True)
            if result.returncode == 0 and result.stdout:
                return generator
    
    # Default to VS 2019
    return 'Visual Studio 16 2019'


def main():
    parser = argparse.ArgumentParser(description='Build CppVulkanDemo')
    parser.add_argument('--debug', action='store_true', help='Build in Debug mode')
    parser.add_argument('--release', action='store_true', help='Build in Release mode (default)')
    parser.add_argument('--clean', action='store_true', help='Clean build directory and exit')
    parser.add_argument('--jobs', '-j', type=int, help='Number of parallel jobs')
    parser.add_argument('--generator', '-G', help='CMake generator to use')
    parser.add_argument('--no-color', action='store_true', help='Disable colored output')
    parser.add_argument('--skip-submodules', action='store_true', help='Skip git submodule update')
    parser.add_argument('--auto-download-vulkan', action='store_true', help='Automatically download and install Vulkan SDK if not found')
    parser.add_argument('--vulkan-install-dir', type=str, help='Directory to install Vulkan SDK (default: ~/VulkanSDK)')
    
    args = parser.parse_args()
    
    # Disable colors if requested or not in terminal
    if args.no_color or not sys.stdout.isatty():
        Colors.disable()
    
    # Determine build type
    build_type = 'Debug' if args.debug else 'Release'
    
    # Get script and build directories
    script_dir = Path(__file__).parent.absolute()
    build_dir = script_dir / 'build'
    
    # Handle clean
    if args.clean:
        print_info("Cleaning build directory...")
        if build_dir.exists():
            shutil.rmtree(build_dir)
        print_info("Build directory cleaned.")
        return 0
    
    # Check requirements
    print_info("Checking for required tools...")
    
    # Check CMake
    if not check_command('cmake'):
        print_error("CMake is not installed. Please install CMake 3.7 or higher.")
        print_error("Download from: https://cmake.org/")
        return 1
    
    cmake_version = get_cmake_version()
    if cmake_version < (3, 7):
        print_error(f"CMake 3.7 or higher is required. Found version: {'.'.join(map(str, cmake_version))}")
        return 1
    
    print_info(f"Found CMake version: {'.'.join(map(str, cmake_version))}")
    
    # Check Git
    if not check_command('git'):
        print_error("Git is not installed. Please install Git.")
        print_error("Download from: https://git-scm.com/")
        return 1
    
    # Check Vulkan SDK
    print_info("Checking for Vulkan SDK...")
    vulkan_sdk = find_vulkan_sdk()
    
    if not vulkan_sdk:
        if args.auto_download_vulkan:
            print_warning("Vulkan SDK not found. Attempting to download and install...")
            
            # Get download info
            url, filename, version = get_vulkan_download_info()
            if not url:
                print_error(f"Unsupported platform: {platform.system()}")
                return 1
            
            # Determine install location
            if args.vulkan_install_dir:
                install_base = Path(args.vulkan_install_dir)
            else:
                install_base = Path.home() / 'VulkanSDK'
            
            install_path = install_base / version
            
            # Download SDK
            download_dir = Path.home() / '.vulkan_sdk_download'
            download_dir.mkdir(exist_ok=True)
            download_path = download_dir / filename
            
            if not download_path.exists():
                print_info(f"Downloading Vulkan SDK {version}...")
                print_info(f"URL: {url}")
                if not download_file(url, download_path):
                    return 1
            else:
                print_info(f"Using cached download: {download_path}")
            
            # Install SDK
            print_info(f"Installing Vulkan SDK to {install_path}...")
            if not install_vulkan_sdk(download_path, install_path):
                return 1
            
            # Set up paths based on platform
            system = platform.system()
            if system == 'Windows':
                vulkan_sdk = str(install_path)
            elif system == 'Darwin':
                vulkan_sdk = str(install_path / 'macOS')
            elif system == 'Linux':
                vulkan_sdk = str(install_path / 'x86_64')
            
            # Verify installation
            if not Path(vulkan_sdk).exists():
                print_error(f"Installation completed but SDK not found at expected location: {vulkan_sdk}")
                return 1
            
            print_info("Vulkan SDK installed successfully!")
            
            # Clean up download if installation succeeded
            try:
                download_path.unlink()
            except:
                pass
        else:
            print_error("Vulkan SDK not found.")
            print_error("Please install it from: https://www.lunarg.com/vulkan-sdk/")
            print_error("After installation, set the VULKAN_SDK environment variable.")
            print_error("Or run with --auto-download-vulkan to download and install automatically.")
            return 1
    
    print_info(f"Using Vulkan SDK: {vulkan_sdk}")
    os.environ['VULKAN_SDK'] = vulkan_sdk
    
    # Initialize submodules
    if not args.skip_submodules:
        print_info("Initializing git submodules...")
        run_command(['git', 'submodule', 'update', '--init', '--recursive'], cwd=script_dir)
    
    # Create build directory
    print_info("Creating build directory...")
    build_dir.mkdir(exist_ok=True)
    
    # Prepare CMake command
    cmake_cmd = ['cmake', '..']
    cmake_cmd.extend(['-DCMAKE_BUILD_TYPE=' + build_type])
    
    # Platform-specific configuration
    system = platform.system()
    
    if system == 'Windows':
        generator = args.generator or get_vs_generator()
        cmake_cmd.extend(['-G', generator])
        cmake_cmd.extend(['-A', 'x64'])
        print_info(f"Using generator: {generator}")
    
    elif system == 'Darwin':
        # macOS specific
        cmake_cmd.extend(['-DCMAKE_OSX_DEPLOYMENT_TARGET=10.12'])
        
        # Check for MoltenVK
        if not Path('/usr/local/include/MoltenVK').exists():
            print_warning("MoltenVK not found at /usr/local/include/MoltenVK")
            print_warning("The CMakeLists.txt expects MoltenVK at this location.")
            print_warning("You may need to modify CMakeLists.txt line 11 to point to your MoltenVK installation.")
    
    if args.generator and system != 'Windows':
        cmake_cmd.extend(['-G', args.generator])
    
    # Configure with CMake
    print_info(f"Configuring with CMake (Build type: {build_type})...")
    run_command(cmake_cmd, cwd=build_dir)
    
    # Build
    jobs = args.jobs or get_cpu_count()
    print_info(f"Building with {jobs} parallel jobs...")
    
    build_cmd = ['cmake', '--build', '.', '--config', build_type]
    if jobs > 1:
        build_cmd.extend(['-j', str(jobs)])
    
    run_command(build_cmd, cwd=build_dir)
    
    # Check results
    print_info("Build completed successfully!")
    print_info(f"Build type: {build_type}")
    print_info(f"Build directory: {build_dir}")
    
    # Check for built library
    lib_names = ['libvtb.a', 'vtb.lib']
    lib_found = False
    
    for lib_name in lib_names:
        lib_paths = [
            build_dir / lib_name,
            build_dir / build_type / lib_name,
        ]
        for lib_path in lib_paths:
            if lib_path.exists():
                print_info(f"Library '{lib_name}' built successfully.")
                lib_found = True
                break
        if lib_found:
            break
    
    if not lib_found:
        print_warning("Could not find built library. Check build output for errors.")
    
    if system == 'Windows':
        sln_path = build_dir / 'vtb.sln'
        if sln_path.exists():
            print_info(f"To open in Visual Studio: start {sln_path}")
    
    print_info("To enable example builds, uncomment the subdirectories in Example/CMakeLists.txt")
    
    return 0


if __name__ == '__main__':
    sys.exit(main())