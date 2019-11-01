#pragma once

#include "ResourceBase.hpp"
#include "SwapChain.hpp"
#include "CreatePipeline.hpp"

VTB_BEGIN

template <typename Vertex>
struct GraphicsPipeline : ResourceBase<VkPipeline>
{
    void Create(
        VkDevice device,
        VkDescriptorSetLayout descriptorSetLayout,
        VkExtent2D extent,
        VkRenderPass renderPass,
        const ShaderData& vertData,
        const ShaderData& fragData,
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    {
        ResourceBase<VkPipeline>::Create(device);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = 0;
        VTB_CALL(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_));

        // TODO: move to PipelineStateCreateInfo::Type ?
        PipelineViewportStateCreateInfo viewportState(extent);
        PipelineDepthStencilStateCreateInfo depthStencil;
        PipelineColorBlendStateCreateInfo colorBlending;
        PipelineRasterizationStateCreateInfo rasterizer;
        PipelineMultisampleStateCreateInfo multisampling;
        PipelineVertexInputStateCreateInfo<Vertex> vertexInputInfo;
        PipelineShaderStageCreateInfo shaderStage(device_, vertData, fragData);

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkGraphicsPipelineCreateInfo ci = {};
        ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        ci.stageCount = shaderStage.Count();
        ci.pStages = shaderStage.Data();
        ci.pVertexInputState = &vertexInputInfo.ci;
        ci.pInputAssemblyState = &inputAssembly;
        ci.pViewportState = &viewportState.ci;
        ci.pRasterizationState = &rasterizer.ci;
        ci.pMultisampleState = &multisampling.ci;
        ci.pDepthStencilState = &depthStencil.ci;
        ci.pColorBlendState = &colorBlending.ci;
        ci.pDynamicState = nullptr;
        ci.layout = pipelineLayout_;
        ci.renderPass = renderPass;
        ci.subpass = 0;
        ci.basePipelineHandle = VK_NULL_HANDLE;
        ci.basePipelineIndex = -1;

        VTB_CALL(vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &ci, nullptr, &resource_));
    }

    void Destroy() override
    {
        VTB_CALLV(vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr));
        VTB_CALLV(vkDestroyPipeline(device_, resource_, nullptr));
        resource_ = 0;
        pipelineLayout_ = 0;
    }

    VkPipelineLayout PipelineLayout() const { return pipelineLayout_; }

private:
    VkPipelineLayout pipelineLayout_;
};

VTB_END
