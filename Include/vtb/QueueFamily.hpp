#pragma once

#include "Vulkan.hpp"

VTB_BEGIN

struct QueueFamily
{
    struct Indices
    {
        int graphicsFamily = -1;
        int presentFamily = -1;
        
        bool isComplete() const
        {
            return graphicsFamily >= 0 && presentFamily >= 0;
        }
    };

    static Indices Find(VkPhysicalDevice device, VkSurfaceKHR surface);
};

VTB_END

