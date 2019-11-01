#include "vtb/ImageView.hpp"

VTB_BEGIN

void ImageView::Create(VkDevice device, VkImage image)
{
    Base::Create(device);

    VkImageViewCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.image = image;
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ci.format = VK_FORMAT_R8G8B8A8_UNORM;
    ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ci.subresourceRange.baseMipLevel = 0;
    ci.subresourceRange.levelCount = 1;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount = 1;
    
    VTB_CALL(vkCreateImageView(device_, &ci, nullptr, &resource_));
}

void ImageView::Destroy()
{
    VTB_CALLV(vkDestroyImageView(device_, resource_, 0));
    resource_ = 0;
}

VkImageView CreateImageView(
    VkDevice device,
    VkImage image, 
    VkFormat format, 
    VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.image = image;
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ci.format = format;
    ci.subresourceRange.aspectMask = aspectFlags;
    ci.subresourceRange.baseMipLevel = 0;
    ci.subresourceRange.levelCount = 1;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount = 1;
    
    VkImageView imageView;
    VTB_CALL(vkCreateImageView(device, &ci, nullptr, &imageView));
    
    return imageView;
}

VTB_END
