#pragma once

#include <vector>
#include "ResourceBase.hpp"

VTB_BEGIN

VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char> &code);

VTB_END
