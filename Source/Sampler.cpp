#include "vtb/Sampler.hpp"

VTB_BEGIN

#define MAX_ANISTROPY 16

void Sampler::Create(VkDevice device)
{
    ResourceBase<VkSampler>::Create(device);

    VkSamplerCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    ci.magFilter = VK_FILTER_LINEAR;
    ci.minFilter = VK_FILTER_LINEAR;
    ci.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ci.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ci.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ci.anisotropyEnable = VK_TRUE;
    ci.maxAnisotropy = MAX_ANISTROPY;
    ci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    ci.unnormalizedCoordinates = VK_FALSE;
    ci.compareEnable = VK_FALSE;
    ci.compareOp = VK_COMPARE_OP_ALWAYS;
    ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    ci.mipLodBias = 0.0f;
    ci.minLod = 0.0f;
    ci.maxLod = 0.0f;

    VTB_CALL(vkCreateSampler(device_, &ci, nullptr, &resource_));
}

void Sampler::Destroy()
{
    VTB_CALLV(vkDestroySampler(device_, resource_, nullptr));
    resource_ = 0;
}

VTB_END
