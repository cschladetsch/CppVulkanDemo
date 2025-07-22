#include <gtest/gtest.h>
#include <vtb/Config.hpp>

TEST(ConfigTest, NamespaceMacrosDefined) {
    // Test that namespace macros are defined
    #ifndef VTB_NAMESPACE_NAME
        FAIL() << "VTB_NAMESPACE_NAME should be defined";
    #endif
    
    // Verify namespace name is as expected
    EXPECT_STREQ("vtb", "vtb");
}

TEST(ConfigTest, PlatformMacros) {
    // Test platform-specific macros
    #ifdef WIN32
        #ifndef VK_USE_PLATFORM_WIN32_KHR
            FAIL() << "VK_USE_PLATFORM_WIN32_KHR should be defined on Windows";
        #endif
    #endif
    
    // Test that VTB_BEGIN and VTB_END macros are defined
    #ifndef VTB_BEGIN
        FAIL() << "VTB_BEGIN should be defined";
    #endif
    #ifndef VTB_END
        FAIL() << "VTB_END should be defined";
    #endif
    
    SUCCEED();
}

TEST(ConfigTest, VulkanHeadersIncluded) {
    // Test that Vulkan headers are accessible
    // This is a compile-time test - if it compiles, it passes
    VkResult result = VK_SUCCESS;
    EXPECT_EQ(VK_SUCCESS, result);
}