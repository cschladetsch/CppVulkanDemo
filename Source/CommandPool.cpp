#include "vtb/CommandPool.hpp"
#include "vtb/QueueFamily.hpp"

VTB_BEGIN

void CommandPool::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPoolCreateFlags flags)
{
    ResourceBase<VkCommandPool>::Create(device);

    auto indices = QueueFamily::Find(physicalDevice, surface);
    
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = indices.graphicsFamily;
    poolInfo.flags = flags;
    VTB_CALL(vkCreateCommandPool(device, &poolInfo, nullptr, &resource_));
}

void CommandPool::Destroy()
{
    VTB_CALLV(vkDestroyCommandPool(device_, resource_, nullptr));
    resource_ = 0;
}

VTB_END
