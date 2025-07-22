#include <iostream>
#include <algorithm>
#include <assert.h>
#include <cstring>

#include "vtb/Util.hpp"

VTB_BEGIN

const std::vector<const char *> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char *> EnumExtensions()
{
    auto extensions = GetVector((const char *)nullptr, vkEnumerateInstanceExtensionProperties);
    std::vector<const char *> extensionNames(extensions.size());
    std::cout << "Available extensions:" << std::endl;
    int n = 0;
    for (const auto& ext : extensions)
    {
        std::cout << "\t" << ext.extensionName << std::endl;
        extensionNames[n++] = (char *)ext.extensionName;
    }
    
    return extensionNames;
}

bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    auto availableExtensions = GetVector(device, vkEnumerateDeviceExtensionProperties);
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    
    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);
    
    return requiredExtensions.empty();
}

VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;
    VTB_CALL(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer));
    
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    VTB_CALL(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    
    return commandBuffer;
}

void EndSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer, VkQueue queue)
{
    VTB_CALL(vkEndCommandBuffer(commandBuffer));
    
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    VTB_CALL(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    VTB_CALL(vkQueueWaitIdle(queue));
    
    VTB_CALLV(vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer));
}

void MemoryCopy(VkDevice device, VkDeviceMemory memory, VkDeviceSize size, const void* source)
{
    void *dest = 0;
    VTB_CALL(vkMapMemory(device, memory, 0, size, 0, &dest));
    memcpy(dest, source, (size_t)size);
    VTB_CALLV(vkUnmapMemory(device, memory));
}

VTB_END
