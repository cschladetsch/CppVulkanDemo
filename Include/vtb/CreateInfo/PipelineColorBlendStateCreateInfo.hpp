// Created by Christian.
// Copyright © 2018 TwoBulls. All rights reserved.

#pragma once

#include "vtb/Vulkan.hpp"

VTB_BEGIN

struct PipelineColorBlendStateCreateInfo
{
    PipelineColorBlendStateCreateInfo()
    {
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        ci.logicOpEnable = VK_FALSE;
        ci.logicOp = VK_LOGIC_OP_COPY;
        ci.attachmentCount = 1;
        ci.pAttachments = &colorBlendAttachment;
        ci.blendConstants[0] = 0.0f;
        ci.blendConstants[1] = 0.0f;
        ci.blendConstants[2] = 0.0f;
        ci.blendConstants[3] = 0.0f;
    }

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    VkPipelineColorBlendStateCreateInfo ci = {};
};

VTB_END
