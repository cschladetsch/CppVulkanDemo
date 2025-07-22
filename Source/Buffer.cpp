#include "vtb/Buffer.hpp"
#include "vtb/Commands.hpp"
#include <stdexcept>

VTB_BEGIN

uint32_t FindMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    VTB_CALLV(vkGetPhysicalDeviceMemoryProperties(device, &memProperties));
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    
    throw std::runtime_error("failed to find suitable memory type");
}

void CreateBuffer(
    VkDevice device, 
    VkPhysicalDevice physicalDevice, 
    VkDeviceSize size, 
    VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer, 
    VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VTB_CALL(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));
    
    VkMemoryRequirements memRequirements;
    VTB_CALLV(vkGetBufferMemoryRequirements(device, buffer, &memRequirements));
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
    
    VTB_CALL(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory));
    
    VTB_CALL(vkBindBufferMemory(device, buffer, bufferMemory, 0));
}

// We could create a separate command pool for these kinds of short-lived buffers, because the implementation may
// be able to apply memory allocation optimizations. Use the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT in that case.
void CopyBuffer(
    VkDevice device, 
    VkCommandPool commandPool, 
    VkBuffer srcBuffer, 
    VkBuffer dstBuffer, 
    VkDeviceSize size, 
    VkQueue queue)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);
    
    VkBufferCopy copyRegion {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    VTB_CALLV(vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion));

    EndSingleTimeCommands(device, commandPool, commandBuffer, queue);
}

VTB_END
