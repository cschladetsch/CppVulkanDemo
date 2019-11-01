#include <iostream>
#include <string>
#include <stdexcept>
#include <assert.h>

#include "vtb/Debug.hpp"
#include "vtb/GetVector.hpp"

VTB_BEGIN

namespace Trace
{
    VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char *layerPrefix, const char *msg, void *userData);

    PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
    PFN_vkDebugReportMessageEXT dbgBreakCallback;
    VkDebugReportCallbackEXT msgCallback;
    VkInstance instance;
    static const char *src_file;
    static const char *src_function;
    static int src_line;
    VkResult LastResult;

    void Create(VkInstance instance, Level level, Severity severity)
    {
        VkDebugReportFlagsEXT flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
        DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
        dbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));

        if (!CreateDebugReportCallback)
        {
            std::cerr << "Debug reporting not available\n";
            return;
        }
        
        VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
        dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)VulkanDebugCallback;
        dbgCreateInfo.flags = flags;

        VTB_CALL(CreateDebugReportCallback(
            instance,
            &dbgCreateInfo,
            nullptr,
            &msgCallback));
    }

    void Destroy()
    {
        if (DestroyDebugReportCallback != nullptr)
            DestroyDebugReportCallback(instance, msgCallback, nullptr);
    }

    void SetSourcePoint(const char *file, int line, const char *function, const char *call)
    {
        src_file = file;
        src_line = line;
        src_function = function;
    }

    bool Succeeded()
    {
        return LastResult == VK_SUCCESS;
    }

    const char *ResultToString(VkResult errorCode)
    {
        switch (errorCode)
        {
            #define STR(r) case VK_ ##r: return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
            #undef STR
        default:
            return "UNKNOWN_ERROR";
        }
    }

    VkBool32 VKAPI_CALL VulkanDebugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char *layerPrefix,
        const char *msg,
        void *userData)
    {
        // Select prefix depending on flags passed to the callback
        // Note that multiple flags may be set for a single validation message
        std::string prefix("");

        // Error that may result in undefined behaviour
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            prefix += "ERROR:";
        };
        // Warnings may hint at unexpected / non-spec API usage
        if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            prefix += "WARNING:";
        };
        // May indicate sub-optimal usage of the API
        if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            prefix += "PERFORMANCE:";
        };
        // Informal messages that may become handy during debugging
        if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        {
            prefix += "INFO:";
        }
        // Diagnostic info from the Vulkan loader and layers
        // Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
        if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        {
            prefix += "DEBUG:";
        }

        if (src_file)
            std::cout << src_file << "(" << src_line << "): " << src_function << ": ";
        std::cout << prefix << " layer: " << layerPrefix << ": " << msg << std::endl << std::endl;

        src_file = src_function = 0;
        src_line = 0;

        return VK_FALSE;
    }
}

// Check that all the requested validation layers exist within Vulkan
bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    auto availableLayers = GetVector(vkEnumerateInstanceLayerProperties);
    std::cout << "Available validation layers:" << std::endl;
    for (const auto &layer : availableLayers)
        std::cout << "\t" << layer.layerName << std::endl;

    for (const char *layerName : validationLayers)
    {
        bool found = false;
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cerr << "Failed to find validation layer '" << layerName << "'\n";
            return false;
        }
    }

    return true;
}

VTB_END
