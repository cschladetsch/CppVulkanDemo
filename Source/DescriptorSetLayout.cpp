#include <iostream>

#include "vtb/DescriptorSetLayout.hpp"

VTB_BEGIN

DescriptorSetLayout::Builder DescriptorSetLayout::Build(VkDevice device)
{
    ResourceBase<VkDescriptorSetLayout>::Create(device);
    return Builder(device, &resource_);
}

void DescriptorSetLayout::Destroy()
{
}

DescriptorSetLayout::Builder::Builder(
    VkDevice device,
    VkDescriptorSetLayout *layout)
    : device_(device), layout_(layout)
{
}

DescriptorSetLayout::Builder::~Builder()
{
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = (uint32_t)bindings_.size();
    layoutInfo.pBindings = bindings_.data();
    
    VTB_CALL(vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr, layout_));
}

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddLayoutBinding(VkShaderStageFlags stage, VkDescriptorType type)
{
    auto binding = Next();
    binding.descriptorType = type;
    binding.stageFlags = stage;
    bindings_.push_back(binding);
    return *this;
}

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBuffer()
{
    return AddLayoutBinding(VK_SHADER_STAGE_VERTEX_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddSampler()
{
    return AddLayoutBinding(VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
}

VkDescriptorSetLayoutBinding DescriptorSetLayout::Builder::Next() const
{
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = (uint32_t)bindings_.size();
    binding.descriptorCount = 1;
    binding.pImmutableSamplers = nullptr; 
    return binding;
}

VTB_END
