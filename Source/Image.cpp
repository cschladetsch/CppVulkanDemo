#include "stb_image.h"

#include "vtb/Image.hpp"
#include "vtb/Buffer.hpp"
#include "vtb/Commands.hpp"
#include "vtb/Memory.hpp"
#include "vtb/Depth.hpp"

VTB_BEGIN

Image::Image(VkDevice device, VkImage image, VkDeviceMemory memory)
{
    Base::Create(device);
    resource_ = image;
    imageMemory_ = memory;
}

void Image::Create(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkCommandPool commandPool,
    VkQueue queue,
    const char* fileName,
    VkFormat format)
{
    Base::Create(device);

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        fileName, 
        &texWidth, 
        &texHeight, 
        &texChannels, 
        STBI_rgb_alpha);

    if (!pixels)
        throw std::runtime_error("Failed to load texture image.");

    VkDeviceSize imageSize = texWidth*texHeight*4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(
        device, physicalDevice, 
        imageSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory);

    MemoryCopy(device, stagingBufferMemory, imageSize, pixels);
    
    stbi_image_free(pixels);

    CreateImage(
        device, physicalDevice,
        texWidth, texHeight, 
        format,
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        resource_, 
        imageMemory_);
    
    // make the texture a good target for buffer copy
    TransitionImageLayout(
        device, commandPool, queue,
        resource_, 
        format,
        VK_IMAGE_LAYOUT_UNDEFINED, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    CopyBufferToImage(
        device, commandPool, queue,
        stagingBuffer, 
        resource_, 
        static_cast<uint32_t>(texWidth), 
        static_cast<uint32_t>(texHeight));
    
    // make the texture usable by a shader.
    TransitionImageLayout(
        device, commandPool, queue,
        resource_, 
        format,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VTB_CALLV(vkDestroyBuffer(device_, stagingBuffer, nullptr));
    VTB_CALLV(vkFreeMemory(device_, stagingBufferMemory, nullptr));
}

Image Image::Create(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t width, uint32_t height,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkFormat format)
{
    VkImage image;
    VkDeviceMemory memory;
    CreateImage(device, physicalDevice, width, height, format, tiling, usage, properties, image, memory);
    return Image(device, image, memory);
}

void Image::Destroy()
{
    VTB_CALLV(vkDestroyImage(device_, resource_, nullptr));
    VTB_CALLV(vkFreeMemory(device_, imageMemory_, nullptr));
    resource_ = 0;
    imageMemory_ = 0;
}

void CreateImage(
    VkDevice device, 
    VkPhysicalDevice physicalDevice, 
    uint32_t width, uint32_t height, 
    VkFormat format, 
    VkImageTiling tiling, 
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags properties, 
    VkImage& image, 
    VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = VK_IMAGE_TYPE_2D;
    ci.extent.width = width;
    ci.extent.height = height;
    ci.extent.depth = 1;
    ci.mipLevels = 1;
    ci.arrayLayers = 1;
    ci.format = format;
    ci.tiling = tiling;
    ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ci.usage = usage;
    ci.samples = VK_SAMPLE_COUNT_1_BIT;
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VTB_CALL(vkCreateImage(device, &ci, nullptr, &image));
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
    
    VTB_CALL(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory));
    
    VTB_CALL(vkBindImageMemory(device, image, imageMemory, 0));
}

void TransitionImageLayout(
    VkDevice device, VkCommandPool commandPool,
    VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, 
    VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);
    
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        
        if (HasStencilComponent(format))
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition");
    }
    
    VTB_CALLV(vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier));
    
    EndSingleTimeCommands(device, commandPool, commandBuffer, queue);
}

void CopyBufferToImage(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkBuffer buffer, VkImage image, 
    uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    
    region.imageOffset = {0, 0, 0};
    region.imageExtent = { width, height, 1 };
    
    VTB_CALLV(vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region));
    
    EndSingleTimeCommands(device, commandPool, commandBuffer, queue);
}

VkFormat FindSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        VTB_CALLV(vkGetPhysicalDeviceFormatProperties(device, format, &props));
        
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }
    
    throw std::runtime_error("failed to find supported format");
}

VTB_END
