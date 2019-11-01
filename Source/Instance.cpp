#include "vtb/Instance.hpp"
#include "vtb/Debug.hpp"
#include "vtb/Glfw.hpp"

VTB_BEGIN

void Instance::Create(bool enableValidationLayers)
{
    EnumExtensions();
    
    std::vector<const char *> extensions = 
    {
    };

    // Enable surface extensions depending on os
#if defined(WIN32)
    uint32_t count = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    for (uint32_t n = 0; n < count; ++n)
        extensions.push_back(glfwExtensions[n]);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
    extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_DIRECT2DISPLAY)
    extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    extensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "TwoBullsVulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
#if !APPLE
    if (enableValidationLayers)
    {
        const char *layers[] = { "VK_LAYER_LUNARG_standard_validation", };
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = layers;
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
#endif

    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    VTB_CALL(vkCreateInstance(&createInfo, nullptr, &instance_));
}

void Instance::Destroy()
{
    VTB_CALLV(vkDestroyInstance(instance_, nullptr));
}

VTB_END
