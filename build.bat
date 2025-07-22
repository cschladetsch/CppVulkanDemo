@echo off
setlocal enabledelayedexpansion

REM Build script for CppVulkanDemo - Windows

set SCRIPT_DIR=%~dp0
set BUILD_DIR=%SCRIPT_DIR%build
set BUILD_TYPE=Release
set GENERATOR="Visual Studio 16 2019"
set ARCH=x64
set AUTO_DOWNLOAD_VULKAN=0
set VULKAN_INSTALL_DIR=

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :check_requirements
if /i "%~1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto :parse_args
)
if /i "%~1"=="--release" (
    set BUILD_TYPE=Release
    shift
    goto :parse_args
)
if /i "%~1"=="--clean" (
    echo [INFO] Cleaning build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    echo [INFO] Build directory cleaned.
    exit /b 0
)
if /i "%~1"=="--vs2022" (
    set GENERATOR="Visual Studio 17 2022"
    shift
    goto :parse_args
)
if /i "%~1"=="--vs2019" (
    set GENERATOR="Visual Studio 16 2019"
    shift
    goto :parse_args
)
if /i "%~1"=="--vs2017" (
    set GENERATOR="Visual Studio 15 2017"
    set ARCH=Win64
    shift
    goto :parse_args
)
if /i "%~1"=="--auto-download-vulkan" (
    set AUTO_DOWNLOAD_VULKAN=1
    shift
    goto :parse_args
)
if /i "%~1"=="--vulkan-install-dir" (
    set VULKAN_INSTALL_DIR=%~2
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="-h" goto :show_help
if /i "%~1"=="--help" goto :show_help
echo [ERROR] Unknown option: %~1
exit /b 1

:show_help
echo Usage: %~nx0 [options]
echo Options:
echo   --debug                  Build in Debug mode (default: Release)
echo   --release                Build in Release mode (default)
echo   --clean                  Clean build directory and exit
echo   --vs2022                 Use Visual Studio 2022 generator
echo   --vs2019                 Use Visual Studio 2019 generator (default)
echo   --vs2017                 Use Visual Studio 2017 generator
echo   --auto-download-vulkan   Automatically download and install Vulkan SDK if not found
echo   --vulkan-install-dir DIR Directory to install Vulkan SDK (default: C:\VulkanSDK)
echo   -h, --help               Show this help message
exit /b 0

:check_requirements
echo [INFO] Checking for required tools...

REM Check for CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] CMake is not installed or not in PATH.
    echo [ERROR] Please install CMake 3.7 or higher from https://cmake.org/
    exit /b 1
)

REM Check CMake version
for /f "tokens=3" %%i in ('cmake --version ^| findstr /i version') do set CMAKE_VERSION=%%i
echo [INFO] Found CMake version: %CMAKE_VERSION%

REM Check for Git
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] Git is not installed or not in PATH.
    echo [ERROR] Please install Git from https://git-scm.com/
    exit /b 1
)

REM Check for Vulkan SDK
echo [INFO] Checking for Vulkan SDK...

if not defined VULKAN_SDK (
    echo [WARNING] VULKAN_SDK environment variable is not set.
    
    REM Try common locations
    if exist "C:\VulkanSDK" (
        for /f "delims=" %%i in ('dir /b /ad "C:\VulkanSDK\*" 2^>nul ^| sort /r') do (
            set VULKAN_SDK=C:\VulkanSDK\%%i
            goto :found_sdk
        )
    )
    
    if "%AUTO_DOWNLOAD_VULKAN%"=="1" (
        echo [WARNING] Vulkan SDK not found. Delegating to Python build script for automatic download...
        
        REM Build Python command
        set PYTHON_CMD=python "%SCRIPT_DIR%build.py" --auto-download-vulkan
        
        REM Add optional install directory if specified
        if not "%VULKAN_INSTALL_DIR%"=="" (
            set PYTHON_CMD=!PYTHON_CMD! --vulkan-install-dir "%VULKAN_INSTALL_DIR%"
        )
        
        REM Add other options
        if "%BUILD_TYPE%"=="Debug" (
            set PYTHON_CMD=!PYTHON_CMD! --debug
        )
        
        REM Execute Python build script
        !PYTHON_CMD!
        exit /b !errorlevel!
    ) else (
        echo [ERROR] Vulkan SDK not found. Please install it from https://www.lunarg.com/vulkan-sdk/
        echo [ERROR] After installation, the VULKAN_SDK environment variable should be set automatically.
        echo [ERROR] Or run with --auto-download-vulkan to download and install automatically.
        exit /b 1
    )
)

:found_sdk
echo [INFO] Using Vulkan SDK: %VULKAN_SDK%

REM Initialize submodules
echo [INFO] Initializing git submodules...
cd /d "%SCRIPT_DIR%"
git submodule update --init --recursive
if %errorlevel% neq 0 (
    echo [ERROR] Failed to initialize git submodules.
    exit /b 1
)

REM Create build directory
echo [INFO] Creating build directory...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Configure with CMake
echo [INFO] Configuring with CMake (Generator: %GENERATOR%, Architecture: %ARCH%)...

if "%ARCH%"=="Win64" (
    REM For VS2017 and older
    cmake .. -G %GENERATOR% -A %ARCH% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
) else (
    REM For VS2019 and newer
    cmake .. -G %GENERATOR% -A %ARCH% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
)

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

REM Build
echo [INFO] Building %BUILD_TYPE% configuration...
cmake --build . --config %BUILD_TYPE%

if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    exit /b 1
)

REM Summary
echo.
echo [INFO] Build completed successfully!
echo [INFO] Build type: %BUILD_TYPE%
echo [INFO] Build directory: %BUILD_DIR%
echo [INFO] Solution file: %BUILD_DIR%\vtb.sln

REM Check for built library
if exist "%BUILD_DIR%\%BUILD_TYPE%\vtb.lib" (
    echo [INFO] Library 'vtb.lib' built successfully.
) else if exist "%BUILD_DIR%\vtb.lib" (
    echo [INFO] Library 'vtb.lib' built successfully.
) else (
    echo [WARNING] Could not find built library. Check build output for errors.
)

echo.
echo [INFO] To open the project in Visual Studio, run: start %BUILD_DIR%\vtb.sln
echo [INFO] To enable example builds, uncomment the subdirectories in Example\CMakeLists.txt

endlocal