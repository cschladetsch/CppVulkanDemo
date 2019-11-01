#include "vtb/DescriptorSet.hpp"

VTB_BEGIN

DescriptorSet::Builder DescriptorSet::Build(
    VkDevice device, 
    VkDescriptorPool pool, 
    VkDescriptorSetLayout layout)
{
    return Builder(device, pool, layout, &set_);
}

DescriptorSet::Builder::Builder(
    VkDevice device,
    VkDescriptorPool pool,
    VkDescriptorSetLayout layout,
    VkDescriptorSet *set)
    : device_(device), set_(set)
{
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;
    
    VTB_CALL(vkAllocateDescriptorSets(device, &allocInfo, set_));
}

DescriptorSet::Builder::~Builder()
{
    VTB_CALLV(vkUpdateDescriptorSets(
        device_, 
        static_cast<uint32_t>(writes_.size()), 
        writes_.data(), 
        0, nullptr));
}

VkWriteDescriptorSet DescriptorSet::Builder::NextWrite(VkDescriptorType type) const
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = *set_;
    write.dstBinding = (uint32_t)writes_.size();
    write.dstArrayElement = 0;
    write.descriptorType = type;
    write.descriptorCount = 1;

    return write;
}

DescriptorSet::Builder& DescriptorSet::Builder::AddBuffer(VkBuffer buffer, size_t size)
{
    auto& info = Alloc<VkDescriptorBufferInfo>();
    info.buffer = buffer;
    info.offset = 0;
    info.range = size;

    VkWriteDescriptorSet write = NextWrite(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    write.pBufferInfo = &info;

    writes_.push_back(write);
    return *this;
}

DescriptorSet::Builder& DescriptorSet::Builder::AddSampler(VkImageView view, VkSampler sampler)
{
    auto& info = Alloc<VkDescriptorImageInfo>();
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    info.imageView = view;
    info.sampler = sampler;

    VkWriteDescriptorSet write = NextWrite(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    write.pImageInfo = &info;

    writes_.push_back(write);
    return *this;
}

VTB_END
