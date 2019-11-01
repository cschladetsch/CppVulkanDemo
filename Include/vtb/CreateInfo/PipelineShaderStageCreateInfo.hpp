#pragma once

#include <vector>
#include <array>

#include "vtb/Vulkan.hpp"
#include "vtb/ShaderModule.hpp"

VTB_BEGIN

struct PipelineShaderStageCreateInfo
{
    typedef std::vector<char> ShaderData;

    PipelineShaderStageCreateInfo(VkDevice device, const ShaderData &vertData, const ShaderData& fragData)
    {
        vertShaderModule = CreateShaderModule(device, vertData);
        fragShaderModule = CreateShaderModule(device, fragData);
        
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        shaders = { vertShaderStageInfo, fragShaderStageInfo };
    }

    std::size_t Count() const { return shaders.size(); }
    const VkPipelineShaderStageCreateInfo *Data() const { return shaders.data(); }
    
private:
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    std::array<VkPipelineShaderStageCreateInfo, 2> shaders;
};

VTB_END
