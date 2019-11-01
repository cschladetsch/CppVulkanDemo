#pragma once

#include "Util.hpp"
#include "ResourceBase.hpp"

VTB_BEGIN

// TODO: return tuple? Or at least, not pass outputs by reference.
void CreateBuffer(
                  VkDevice device,
                  VkPhysicalDevice physicalDevice,
                  VkDeviceSize size,
                  VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties,
                  VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory);

void CopyBuffer(
                VkDevice device,
                VkCommandPool commandPool,
                VkBuffer srcBuffer,
                VkBuffer dstBuffer,
                VkDeviceSize size,
                VkQueue queue);

// A typed buffer of elements that can be read from the GPU
struct Buffer : ResourceBase<VkBuffer>
{
    using Base = ResourceBase<VkBuffer>;

    template <typename Element>
    void Create(
        VkDevice device, 
        VkPhysicalDevice physicalDevice,
        VkCommandPool commandPool,
        VkQueue queue,
        int usage,// e.g. VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        const std::vector<Element> &elements)
    {
        Base::Create(device);

        VkDeviceSize bufferSize = sizeof(Element)*elements.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        CreateBuffer(
            device, 
            physicalDevice, 
            bufferSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            stagingBuffer, 
            stagingBufferMemory);
        
        // fill staging buffer, writable directly by CPU
        MemoryCopy(device_, stagingBufferMemory, bufferSize, (void *)elements.data());
        
        // create device-local buffer - generally not directly accessible by GPU
        CreateBuffer(
            device, 
            physicalDevice, 
            bufferSize, 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            resource_, 
            bufferMemory_);

        CopyBuffer(device, commandPool, stagingBuffer, resource_, bufferSize, queue);

        VTB_CALLV(vkDestroyBuffer(device_, stagingBuffer, nullptr));
        VTB_CALLV(vkFreeMemory(device_, stagingBufferMemory, nullptr));
    }

    void Destroy()
    {
        VTB_CALLV(vkDestroyBuffer(device_, resource_, nullptr));
        VTB_CALLV(vkFreeMemory(device_, bufferMemory_, nullptr));
    }

private:
    VkDeviceMemory bufferMemory_;
};



// see https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator for an optional high-level memory system library.
uint32_t FindMemoryType(
    VkPhysicalDevice device, 
    uint32_t typeFilter, 
    VkMemoryPropertyFlags properties);

VTB_END
