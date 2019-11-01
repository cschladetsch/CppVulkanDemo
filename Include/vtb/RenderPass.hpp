#pragma once

#include "ResourceBase.hpp"

VTB_BEGIN

struct RenderPass : ResourceBase<VkRenderPass>
{
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkFormat format);

    void Destroy() override;
};

VTB_END
