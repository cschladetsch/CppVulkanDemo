#pragma once

#include <array>

#include "ResourceBase.hpp"

VTB_BEGIN

struct Image : ResourceBase<VkImage>
{
    using Base = ResourceBase<VkImage>;
    static const VkFormat DefaultFormat = VK_FORMAT_R8G8B8A8_UNORM;

    Image() = default;
    Image(VkDevice device, VkImage image, VkDeviceMemory memory);

    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkCommandPool commandPool,
        VkQueue queue,
        const char* fileName,
        VkFormat format = DefaultFormat);

    static Image Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        uint32_t width, uint32_t height,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkFormat format = DefaultFormat);

    void Destroy() override;
    VkDeviceMemory DeviceMemory() const { return imageMemory_; }

private:
    VkDeviceMemory imageMemory_;
};

// TODO: return tuple
void CreateImage(
    VkDevice device, 
    VkPhysicalDevice physicalDevice, 
    uint32_t width, uint32_t height, 
    VkFormat format, 
    VkImageTiling tiling, 
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags properties, 
    VkImage& image, 
    VkDeviceMemory& imageMemory);

struct TransitionImageLayoutArgs
{
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue queue;
    VkImage image; 
    VkFormat format;
    VkImageLayout oldLayout;
    VkImageLayout newLayout;
};

void TransitionImageLayout(const TransitionImageLayoutArgs& args);

void TransitionImageLayout(
    VkDevice device, 
    VkCommandPool commandPool,
    VkQueue queue,
    VkImage image, 
    VkFormat format, 
    VkImageLayout oldLayout,
    VkImageLayout newLayout);

void CopyBufferToImage(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkBuffer buffer, 
    VkImage image, uint32_t width, uint32_t height);

VkFormat FindSupportedFormat(
    VkPhysicalDevice device, 
    const std::vector<VkFormat>& candidates, 
    VkImageTiling tiling, 
    VkFormatFeatureFlags features);

VTB_END
