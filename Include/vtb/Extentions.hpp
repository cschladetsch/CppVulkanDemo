#pragma once

#include <vector>
#include "Vulkan.hpp"

VTB_BEGIN

// TODO: not use global
extern const std::vector<const char*> deviceExtensions;

std::vector<const char*> EnumExtensions();

bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

VTB_END
