#pragma once

#include "ResourceBase.hpp"

VTB_BEGIN

struct CommandPool : ResourceBase<VkCommandPool>
{
    // There are two possible flags for command pools:
    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are rerecorded with new commands very often (may change memory allocation behavior)
    // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command buffers to be rerecorded individually, without this flag they all have to be reset together
    // We will only record the command buffers at the beginning of the program and then execute them many times in
    // the main loop, so we're not going to use either of these flags
    void Create(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPoolCreateFlags = 0);
    void Destroy();
};

VTB_END
