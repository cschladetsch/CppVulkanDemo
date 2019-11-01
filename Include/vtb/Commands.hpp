#pragma once

#include "Vulkan.hpp"

VTB_BEGIN

VkCommandBuffer BeginSingleTimeCommands(
    VkDevice device, 
    VkCommandPool commandPool);

void EndSingleTimeCommands(
    VkDevice device, 
    VkCommandPool commandPool, 
    VkCommandBuffer commandBuffer, 
    VkQueue queue);

VTB_END
