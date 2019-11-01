#include "vtb/ShaderModule.hpp"

VTB_BEGIN

VkShaderModule CreateShaderModule(VkDevice device, const ShaderData &code)
{
    VkShaderModuleCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = code.size();
    ci.pCode = (const uint32_t *)code.data();
    VkShaderModule shaderModule;
    VTB_CALL(vkCreateShaderModule(device, &ci, nullptr, &shaderModule));
    
    return shaderModule;
}

VTB_END
