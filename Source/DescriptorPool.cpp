#include <iostream>

#include "vtb/DescriptorPool.hpp"

VTB_BEGIN

DescriptorPool::Builder DescriptorPool::Build(VkDevice device)
{
    return Builder(device, &resource_);
}

void DescriptorPool::Destroy()
{
    VTB_CALLV(vkDestroyDescriptorPool(device_, resource_, nullptr));
    resource_ = 0;
}

DescriptorPool::Builder::Builder(VkDevice device, VkDescriptorPool* pool)
    : device_(device), pool_(pool)
{
}

DescriptorPool::Builder::~Builder()
{
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(sizes_.size());
    poolInfo.pPoolSizes = sizes_.data();
    poolInfo.maxSets = 1;

    VTB_CALL(vkCreateDescriptorPool(device_, &poolInfo, nullptr, pool_));
}

DescriptorPool::Builder& DescriptorPool::Builder::AddPool(VkDescriptorType type)
{
    VkDescriptorPoolSize size = {};
    size.type = type;
    size.descriptorCount = 1;
    sizes_.push_back(size);
    return *this;
}

VTB_END
