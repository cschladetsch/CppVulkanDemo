#include <array>

#include "vtb/SwapChain.hpp"
#include "vtb/QueueFamily.hpp"
#include "vtb/ImageView.hpp"

VTB_BEGIN

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

SwapChain::SupportDetails SwapChain::QuerySupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SupportDetails details;
    VTB_CALLV(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities));

    uint32_t formatCount;
    VTB_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        VTB_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data()));
    }

    uint32_t presentModeCount;
    VTB_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        VTB_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data()));
    }

    return details;
}

void SwapChain::CreateFrameBuffers(VkImageView depthImageView, VkRenderPass renderPass)
{
    frameBuffers_.resize(imageViews_.size());
    
    for (size_t i = 0; i < imageViews_.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            imageViews_[i],
            depthImageView
        };
        
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = Width();
        framebufferInfo.height = Height();
        framebufferInfo.layers = 1;    // can be greater than 1 for, say, stereographic rendering
        
        VTB_CALL(vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &frameBuffers_[i]));
    }
}

void SwapChain::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    device_ = device;

    SupportDetails swapChainSupport = QuerySupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    // The implementation specifies the minimum amount of images to function properly and we'll try to have one more
    // than that to properly implement triple buffering.
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // A value of 0 for maxImageCount means that there is no limit besides memory requirements.
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // render directly to them, which means that they're used as color attachment

    QueueFamily::Indices indices = QueueFamily::Find(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

    // need to specify how to handle swap chain images that will be used across multiple queue families.
    // That will be the case in our application if the graphics queue family is different from the
    // presentation queue. We'll be drawing on the images in the swap chain from the graphics queue and
    // then submitting them on the presentation queue. There are two ways to handle images that are accessed from multiple queues:
    //      VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transfered
    //          before using it in another queue family. This option offers the best performance.
    //      VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers.
    //
    // If the queue families differ, then we'll be using the concurrent mode. Concurrent mode requires you to specify in advance
    // between which queue families ownership will be shared using the queueFamilyIndexCount and pQueueFamilyIndices parameters.
    // If the graphics queue family and presentation queue family are the same, which will be the case on most hardware, then we
    // should stick to exclusive mode, because concurrent mode requires you to specify at least two distinct queue families
    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // we can transform images in the swap chain. for pass-through, use currentTransform
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    // ignore alpha
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    // when creating a new swapchain (after the window is resized for example), we need to specify the old
    // chain here. for the moment, assume only ever create one swapchain.
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VTB_CALL(vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapChain_));

    // get references to the images in the swapchain. note that there cab be more images
    // than we requested.
    vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, images_.data());

    imageFormat_ = surfaceFormat.format;
    extent_ = extent;

    CreateImageViews();
}

void SwapChain::CreateImageViews()
{
    imageViews_.resize(images_.size());
    for (size_t i = 0; i < images_.size(); i++)
    {
        imageViews_[i] = CreateImageView(device_, images_[i], imageFormat_, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void SwapChain::Destroy()
{
    // TODO

    //for (size_t i = 0; i < swapChainFrameBuffers_.size(); i++)
    //    vkDestroyFramebuffer(device_, swapChainFrameBuffers_[i], nullptr);

    //vkFreeCommandBuffers(device_, commandPool_, static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
    //vkDestroyPipeline(device_, graphicsPipeline_, nullptr);
    //vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);

    //for (size_t i = 0; i < swapChainImageViews_.size(); i++)
    //    vkDestroyImageView(device_, swapChainImageViews_[i], nullptr);

    //vkDestroySwapchainKHR(device_, swapChain_, nullptr);
}

uint32_t SwapChain::Next(VkSemaphore semaphore) const
{
    uint32_t imageIndex = 0;
    VTB_CALL(vkAcquireNextImageKHR(
        device_, swapChain_, 
        std::numeric_limits<uint64_t>::max(), 
        semaphore, 
        VK_NULL_HANDLE, 
        &imageIndex));

    return imageIndex;
}

// Queue families are groups of queues that share the same capabilities (flags, timestamp bits, minImageTransferGranularity):
// * NVIDIA GPUs generally have 16 identical Queues (one queue family) that can do anything.
//      These queues are multiplexed by the driver onto one actual hardware queue at drawcall boundaries.
// * AMD GPUs have 4+ queues of various families (one graphics/compute queue, 1-4 compute only queues and two transfer queues),
// * Intel GPUs only have a single queue.
//
// Multiple queues will run concurrently to each other, and you can assign priorities to them.
QueueFamily::Indices QueueFamily::Find(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    Indices indices;
    int i = 0;
    for (const auto& queueFamily : GetVector(device, vkGetPhysicalDeviceQueueFamilyProperties))
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;
        
        VkBool32 presentSupport = false;
        VTB_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport));
        
        if (queueFamily.queueCount > 0 && presentSupport)
            indices.presentFamily = i;
        
        if (indices.isComplete())
            break;
        
        ++i;
    }
    
    return indices;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    // The best case scenario is that the surface has no preferred format, which Vulkan indicates by only returning one
    // VkSurfaceFormatKHR entry which has its format member set to VK_FORMAT_UNDEFINED.
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    
    // go through the list and see if the preferred combination is available
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }
    
    // settle for first available
    return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    // default desired mode
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    
    // prefer VK_PRESENT_MODE_IMMEDIATE_KHR if VK_PRESENT_MODE_MAILBOX_KHR is not available
    for (const auto& mode : availablePresentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
        
        if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            bestMode = mode;
    }
    
    return bestMode;
}

// The swap extent is the resolution of the swap chain images and it's almost always exactly equal to the resolution
// of the window that we're drawing to
VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    
    VkExtent2D actualExtent = { 800, 600 };
    
    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
    
    return actualExtent;
}

VTB_END

