#pragma once

#include "Vulkan.hpp"

VTB_BEGIN

struct Surface
{
    void Create(VkInstance instance, void *window);
    void Destroy();

    VkSurfaceKHR Get() const { return surface_; }

private:
    VkInstance instance_;
    VkSurfaceKHR surface_;
};

VTB_END
