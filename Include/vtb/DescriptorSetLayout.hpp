#pragma once

#include "BuilderBase.hpp"
#include "ResourceBase.hpp"

VTB_BEGIN

struct DescriptorSetLayout : ResourceBase<VkDescriptorSetLayout>
{
    struct Builder : BuilderBase
    {
        Builder(VkDevice device, VkDescriptorSetLayout *layout);
        ~Builder();

        Builder& AddLayoutBinding(VkShaderStageFlags stage, VkDescriptorType type);
        Builder& AddBuffer();
        Builder& AddSampler();

    private:
        VkDescriptorSetLayoutBinding Next() const;

    private:
        VkDevice device_;
        VkDescriptorSetLayout *layout_;
        std::vector<VkDescriptorSetLayoutBinding> bindings_;
    };

    Builder Build(VkDevice device);
    void Destroy();
};

VTB_END
