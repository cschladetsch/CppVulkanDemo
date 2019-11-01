#pragma once

#include "ResourceBase.hpp"

VTB_BEGIN

struct Sampler : ResourceBase<VkSampler>
{
    void Create(VkDevice device) override;
    void Destroy() override;
};

VTB_END
