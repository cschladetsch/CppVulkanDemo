cmake_minimum_required(VERSION 3.7)
project(linux_demo)

set(CMAKE_CXX_STANDARD 11)

# Find packages
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../External/glm
    ${Vulkan_INCLUDE_DIRS}
)

# Add executable
add_executable(linux_demo linux_demo.cpp)

# Link libraries
target_link_libraries(linux_demo
    vtb
    ${Vulkan_LIBRARIES}
    glfw
    pthread
    dl
)

# Copy resources
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})