cmake_minimum_required(VERSION 3.7)

project(vtb)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	find_package(Vulkan REQUIRED)
else()
	include_directories("/Users/christian/work/MoltenVK/Package/Debug/MoltenVK/include")
	add_definitions(-DAPPLE)
	add_definitions(-DMACOS)
endif()

set(Root ${CMAKE_SOURCE_DIR})
set(External ${Root}/External)

# Set output directories for all binaries to top-level Bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

# Also set for each configuration in multi-config generators (like MSVC)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_SOURCE_DIR}/Bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_SOURCE_DIR}/Bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_SOURCE_DIR}/Bin)
endforeach()

include_directories(${Root}/Include)
include_directories(${External}/glm)
include_directories(${Vulkan_INCLUDE_DIRS})

if(WIN32)
	add_subdirectory(${External}/glfw)
endif()

add_subdirectory(Source)
add_subdirectory(Example)

# Enable testing
enable_testing()
add_subdirectory(Test)

