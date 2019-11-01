#pragma once

#include "ResourceBase.hpp"

VTB_BEGIN

struct Semaphore : ResourceBase<VkSemaphore>
{
    void Create(VkDevice device) override
    {
        ResourceBase<VkSemaphore>::Create(device);
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VTB_CALL(vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &resource_));
    }

    void Destroy() override
    {
        VTB_CALLV(vkDestroySemaphore(device_, resource_, nullptr));
        resource_ = 0;
    }
};

VTB_END
