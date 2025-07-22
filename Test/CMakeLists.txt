# Test configuration for VulkanDemo

# Enable testing
enable_testing()

# Find or fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/Include)

# Test executable
add_executable(vtb_tests
    test_main.cpp
    test_simple.cpp
)

# Link libraries
target_link_libraries(vtb_tests
    GTest::gtest
    GTest::gtest_main
)

# Add tests
add_test(NAME vtb_tests COMMAND vtb_tests)

# Set test properties
set_tests_properties(vtb_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)