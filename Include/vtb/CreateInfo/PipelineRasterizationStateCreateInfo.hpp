// Created by Christian.
// Copyright © 2018 TwoBulls. All rights reserved.

#pragma once

#include "vtb/Vulkan.hpp"

VTB_BEGIN

struct PipelineRasterizationStateCreateInfo
{
    PipelineRasterizationStateCreateInfo()
    {
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        ci.depthClampEnable = VK_FALSE;
        ci.rasterizerDiscardEnable = VK_FALSE;
        ci.polygonMode = VK_POLYGON_MODE_FILL;
        ci.lineWidth = 1.0f;
        ci.cullMode = VK_CULL_MODE_BACK_BIT;
        ci.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        ci.depthBiasEnable = VK_FALSE;
        ci.depthBiasConstantFactor = 0.0f;
        ci.depthBiasClamp = 0.0f;
        ci.depthBiasSlopeFactor = 0.0f;
    }

    VkPipelineRasterizationStateCreateInfo ci = {};
};

VTB_END
