#pragma once

#include "vtb/Vulkan.hpp"

VTB_BEGIN

struct PipelineViewportStateCreateInfo
{
    PipelineViewportStateCreateInfo(VkExtent2D extent)
    {
        // use entire swapchain image size for viewport extent
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)extent.width;
        viewport.height = (float)extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        // draw to entire framebuffer
        scissor.offset = {0, 0};
        scissor.extent = extent;
        
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        ci.viewportCount = 1;
        ci.pViewports = &viewport;
        ci.scissorCount = 1;
        ci.pScissors = &scissor;
    }

    VkViewport viewport = {};
    VkRect2D scissor = {};
    VkPipelineViewportStateCreateInfo ci = {};
};

VTB_END
