#pragma once

#include "BuilderBase.hpp"
#include "ResourceBase.hpp"

VTB_BEGIN

struct DescriptorPool : ResourceBase<VkDescriptorPool>
{
    struct Builder : BuilderBase
    {
        Builder(VkDevice device, VkDescriptorPool *pool);
        ~Builder();

        Builder& AddPool(VkDescriptorType type);

    private:
        VkDevice device_;
        VkDescriptorPool *pool_;
        std::vector<VkDescriptorPoolSize> sizes_;
    };

    Builder Build(VkDevice device);
    void Destroy();
};

VTB_END
