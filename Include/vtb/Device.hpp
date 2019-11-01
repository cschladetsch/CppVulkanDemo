#pragma once

#include "Vulkan.hpp"

VTB_BEGIN

struct Device
{
    void Create(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface);
    void Destroy();

    VkDevice Get() const { return device_; }
    VkQueue GraphicsQueue() const { return graphicsQueue_; }
    VkQueue PresentQueue() const { return presentQueue_; }
    
private:
    VkDevice device_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;
};

bool IsSuitableDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
VkPhysicalDevice PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

VTB_END
