#pragma once

#ifdef WIN32
#   define GLFW_INCLUDE_VULKAN
#    define GLFW_EXPOSE_NATIVE_WIN32
#   include <GLFW/glfw3.h>
#   include <GLFW/glfw3native.h>
#endif
