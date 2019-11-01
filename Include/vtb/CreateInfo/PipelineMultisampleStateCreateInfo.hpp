#pragma once

#include "vtb/Vulkan.hpp"

VTB_BEGIN

struct PipelineMultisampleStateCreateInfo
{
    PipelineMultisampleStateCreateInfo()
    {
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        ci.sampleShadingEnable = VK_FALSE;
        ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        ci.minSampleShading = 1.0f;
        ci.pSampleMask = nullptr;
        ci.alphaToCoverageEnable = VK_FALSE;
        ci.alphaToOneEnable = VK_FALSE;
    }

    VkPipelineMultisampleStateCreateInfo ci = {};
};

VTB_END
