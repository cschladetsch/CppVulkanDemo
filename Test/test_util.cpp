#include <gtest/gtest.h>
#include <vtb/Util.hpp>

// Mock Vulkan functions for testing
namespace {
    // Mock data for extension enumeration
    std::vector<VkExtensionProperties> mockExtensions = {
        {"VK_KHR_surface", 25},
        {"VK_KHR_swapchain", 70},
        {"VK_EXT_debug_utils", 1}
    };
    
    // Mock implementation of vkEnumerateInstanceExtensionProperties
    VkResult mockEnumerateInstanceExtensionProperties(const char*, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
        if (!pPropertyCount) return VK_ERROR_INITIALIZATION_FAILED;
        
        if (!pProperties) {
            *pPropertyCount = static_cast<uint32_t>(mockExtensions.size());
            return VK_SUCCESS;
        }
        
        if (*pPropertyCount < mockExtensions.size()) {
            return VK_INCOMPLETE;
        }
        
        for (size_t i = 0; i < mockExtensions.size(); ++i) {
            pProperties[i] = mockExtensions[i];
        }
        *pPropertyCount = static_cast<uint32_t>(mockExtensions.size());
        return VK_SUCCESS;
    }
}

TEST(UtilTest, DeviceExtensionsConstant) {
    // Test that deviceExtensions is properly defined
    EXPECT_FALSE(vtb::deviceExtensions.empty());
    EXPECT_EQ(1, vtb::deviceExtensions.size());
    EXPECT_STREQ(VK_KHR_SWAPCHAIN_EXTENSION_NAME, vtb::deviceExtensions[0]);
}

TEST(UtilTest, NamespaceScope) {
    // Test that functions are in the correct namespace
    // This is a compile-time test - if it compiles, the namespace is correct
    using namespace vtb;
    
    // These should be accessible
    const auto& exts = deviceExtensions;
    EXPECT_FALSE(exts.empty());
}