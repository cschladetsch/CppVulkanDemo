#pragma once

#include "BuilderBase.hpp"
#include "ResourceBase.hpp"

VTB_BEGIN

struct DescriptorSet
{
    struct Builder : BuilderBase
    {
        Builder() = default;
        Builder(
            VkDevice device,
            VkDescriptorPool pool,
            VkDescriptorSetLayout layout,
            VkDescriptorSet *set);
        ~Builder();

        Builder &AddBuffer(VkBuffer buffer, size_t size);
        Builder &AddSampler(VkImageView view, VkSampler sampler);

    private:
        VkWriteDescriptorSet NextWrite(VkDescriptorType) const;

        VkDevice device_;
        VkDescriptorSet *set_;
        std::vector<VkWriteDescriptorSet> writes_;
    };

    Builder Build(VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout);
    VkDescriptorSet Get() const { return set_; }

private:
    VkDescriptorSet set_ = {};
};

VTB_END
