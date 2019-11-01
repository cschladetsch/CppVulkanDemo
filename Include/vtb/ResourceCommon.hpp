#pragma once

#include "Util.hpp"
#include "Noncopyable.hpp"

VTB_BEGIN

// Common to all encapsulated Vulkan resources
struct ResourceCommon
{
    virtual void Create(VkDevice device) = 0;
    virtual void Recreate() = 0;
    virtual void Destroy() = 0;

    VkDevice Device() const { return device_; }

protected:
    VkDevice device_ = { 0 };
};

VTB_END
