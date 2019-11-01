// Created by Christian.
// Copyright © 2018 TwoBulls. All rights reserved.

#pragma once

#include "Vulkan.hpp"

VTB_BEGIN

struct PipelineDepthStencilStateCreateInfo
{
	PipelineDepthStencilStateCreateInfo()
	{
		ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		ci.depthTestEnable = VK_TRUE;
		ci.depthWriteEnable = VK_TRUE;
		ci.depthCompareOp = VK_COMPARE_OP_LESS;
		ci.depthBoundsTestEnable = VK_FALSE;
		ci.minDepthBounds = 0.0f;
		ci.maxDepthBounds = 1.0f;
		ci.stencilTestEnable = VK_FALSE;
		ci.front = {};
		ci.back = {};
	}

    VkPipelineDepthStencilStateCreateInfo ci = {};
};

VTB_END
