#include "vtb/Depth.hpp"
#include "vtb/Image.hpp"
#include "vtb/ImageView.hpp"
#include "vtb/SwapChain.hpp"

VTB_BEGIN

void DepthBuffer::Create(
    VkDevice device, 
    VkPhysicalDevice physicalDevice, 
    VkCommandPool pool, 
    VkQueue queue, 
    VkExtent2D extent)
{
    ResourceBase<VkImage>::Create(device);

    VkFormat depthFormat = FindDepthFormat(physicalDevice);
    CreateImage(device_, physicalDevice, 
        extent.width, extent.height,
        depthFormat, VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        resource_,
        memory_);
    
    imageView_ = CreateImageView(device_, resource_, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    TransitionImageLayout(device, pool, queue, resource_, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void DepthBuffer::Destroy()
{
    VTB_CALLV(vkDestroyImageView(device_, imageView_, nullptr));
    VTB_CALLV(vkDestroyImage(device_, resource_, nullptr));
    VTB_CALLV(vkFreeMemory(device_, memory_, nullptr));

    imageView_ = 0;
    resource_ = 0;
    memory_ = 0;
}

VkFormat FindDepthFormat(VkPhysicalDevice device)
{
    // Make sure to use the VK_FORMAT_FEATURE_ flag instead of VK_IMAGE_USAGE_ in this case.
    return FindSupportedFormat(
        device,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VTB_END
