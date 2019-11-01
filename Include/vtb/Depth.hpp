#pragma once

#include <vector>

#include "ResourceBase.hpp"

VTB_BEGIN

struct DepthBuffer : ResourceBase<VkImage>
{
    void Create(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool pool, VkQueue queue, VkExtent2D extent);
    void Destroy() override;

    VkImageView ImageView() const { return imageView_; }
    VkDeviceMemory Memory() const { return memory_; }

private:
    VkImageView imageView_;
    VkDeviceMemory memory_;
};

VkFormat FindDepthFormat(VkPhysicalDevice device);
inline bool HasStencilComponent(VkFormat format) { return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT; }

VTB_END
