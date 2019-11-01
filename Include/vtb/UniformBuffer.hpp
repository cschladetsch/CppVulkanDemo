#pragma once

#include "Buffer.hpp"
#include "Memory.hpp"

VTB_BEGIN

template <typename Object>
struct UniformBuffer : ResourceBase<VkBuffer>
{
    void Create(VkDevice device, VkPhysicalDevice physicalDevice)
    {
        ResourceBase<VkBuffer>::Create(device);

        CreateBuffer(
            device, physicalDevice,
            sizeof(Object), 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            resource_, 
            memory_);
    }

    void Destroy() override
    {
        VTB_CALLV(vkDestroyBuffer(device_, resource_, nullptr));
        VTB_CALLV(vkFreeMemory(device_, memory_, nullptr));
        resource_ = 0;
        memory_ = 0;
    }

    void Write(const Object& src) const
    {
        MemoryCopy(device_, memory_, sizeof(src), &src);
    }

    VkDeviceMemory Memory() const { return memory_; }

private:
    VkDeviceMemory memory_;
};

VTB_END

