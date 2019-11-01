#pragma once

#include "Util.hpp"

VTB_BEGIN

struct SwapChain
{
    struct SupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    static SupportDetails QuerySupport(VkPhysicalDevice device, VkSurfaceKHR surface);

    void Create(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    void CreateFrameBuffers(VkImageView depthImageView, VkRenderPass renderPass);
    void Destroy();

    VkSwapchainKHR Get() const { return swapChain_; }
    VkFormat ImageFormat() const { return imageFormat_; }
    VkExtent2D Extent() const { return extent_; }

    const std::vector<VkImage>& Images() const { return images_; }
    const std::vector<VkImageView>& ImageViews() const { return imageViews_; }
    const std::vector<VkFramebuffer>& FrameBuffers() const { return frameBuffers_; }

    VkFramebuffer FrameBuffer(int n) const { return frameBuffers_[n]; }
    
    uint32_t Width() const { return extent_.width; }
    uint32_t Height() const { return extent_.height; }

    uint32_t Next(VkSemaphore semaphore) const;

private:
    void CreateImageViews();

private:
    VkDevice device_;
    VkSwapchainKHR swapChain_;
    VkFormat imageFormat_;
    VkExtent2D extent_;
    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    std::vector<VkFramebuffer> frameBuffers_;
};

VTB_END
