#pragma once

#include "vtb/Vulkan.hpp"

VTB_BEGIN

template <typename Vertex>
struct PipelineVertexInputStateCreateInfo
{
    PipelineVertexInputStateCreateInfo()
    {
        bindingDescription = Vertex::GetBindingDescription();
        attributeDescriptions = Vertex::GetAttributeDescriptions();
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        ci.vertexBindingDescriptionCount = 1;
        ci.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        ci.pVertexBindingDescriptions = &bindingDescription;
        ci.pVertexAttributeDescriptions = attributeDescriptions.data();
    }
    
    VkVertexInputBindingDescription bindingDescription;
    typename Vertex::Attributes attributeDescriptions;
    VkPipelineVertexInputStateCreateInfo ci = {};
};

VTB_END
