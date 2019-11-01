#pragma once

#include "Vulkan.hpp"

VTB_BEGIN

void MemoryCopy(
    VkDevice device, 
    VkDeviceMemory memory, 
    VkDeviceSize size, 
    const void* source);

VTB_END
