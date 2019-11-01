#pragma once

#include "vtb/Vulkan.hpp"

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
		ci.minDepthBounds = 0.0f; // Optional
		ci.maxDepthBounds = 1.0f; // Optional
		ci.stencilTestEnable = VK_FALSE;
		ci.front = {};
		ci.back = {};
	}

    VkPipelineDepthStencilStateCreateInfo ci = {};
};

VTB_END
