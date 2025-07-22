#pragma once

#ifdef WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef __APPLE__
#   include "MoltenVK/mvk_vulkan.h"
#else
#   include <vulkan/vulkan.h>
#endif

#ifndef VTB_NAMESPACE_NAME
#    define VTB_NAMESPACE_NAME vtb
#endif

#define VTB_BEGIN namespace VTB_NAMESPACE_NAME {
#define VTB_END }
