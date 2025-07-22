#include <set>
#include <stdexcept>

#include "vtb/Device.hpp"
#include "vtb/QueueFamily.hpp"
#include "vtb/GetVector.hpp"
#include "vtb/SwapChain.hpp"

VTB_BEGIN

void Device::Create(
    VkPhysicalDevice physicalDevice, 
    VkSurfaceKHR surface)
{
    auto indices = vtb::QueueFamily::Find(physicalDevice, surface);
    
    // queue families may overlap; to make them unique we use a set.
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
    
    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo ci = {};
        ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        ci.queueFamilyIndex = queueFamily;
        ci.queueCount = 1;
        ci.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(ci);
    }
    
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    
    VkDeviceCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    ci.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    ci.pQueueCreateInfos = queueCreateInfos.data();
    ci.pEnabledFeatures = &deviceFeatures;
    ci.enabledExtensionCount = 0;
    ci.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    ci.ppEnabledExtensionNames = deviceExtensions.data();
    
    VTB_CALL(vkCreateDevice(physicalDevice, &ci, nullptr, &device_));
    VTB_CALLV(vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_));
    VTB_CALLV(vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_));
}

void Device::Destroy()
{
    VTB_CALLV(vkDestroyDevice(device_, nullptr));
}

VkPhysicalDevice PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
    for (const auto& device : GetVector(instance, vkEnumeratePhysicalDevices))
    {
        if (IsSuitableDevice(device, surface))
        {
            return device;
        }
    }
    
    throw std::runtime_error("Failed to find a suitable GPU");
}

bool IsSuitableDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    vtb::QueueFamily::Indices indices = vtb::QueueFamily::Find(device, surface);
    bool extensionsSupported = vtb::CheckDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChain::SupportDetails swapChainSupport = vtb::SwapChain::QuerySupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

VTB_END
