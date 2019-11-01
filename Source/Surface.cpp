#include <stdexcept>

#include "vtb/Surface.hpp"

VTB_BEGIN

void Surface::Create(VkInstance instance, void *window)
{
    instance_ = instance;

#ifdef WIN32
    VkWin32SurfaceCreateInfoKHR ci = {};
    ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    ci.hwnd = (HWND)window;
    VTB_CALL(vkCreateWin32SurfaceKHR(instance_, &ci, 0, &surface_));
#elif defined(APPLE)
#   ifdef IOS
#       define CREATE_INFO VkIOSSurfaceCreateInfoMVK
#       define CREATE_SURFACE vkCreateIOSSurfaceMVK
#   elif defined(MACOS)
#       define CREATE_INFO VkMacOSSurfaceCreateInfoMVK
#       define CREATE_SURFACE vkCreateMacOSSurfaceMVK
#   endif

    CREATE_INFO ci = {};
    //TODO: Change structure type. Doesn't seem to matter?
    ci.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
    ci.pNext = NULL;
    ci.flags = 0;
    ci.pView = window;

    if (CREATE_SURFACE(instance_, &ci, NULL, &surface_) != VK_SUCCESS)
        throw new std::runtime_error("Failed to make surface");
#else
#   error("Unsupported platform")
#endif
}

void Surface::Destroy()
{
    VTB_CALLV(vkDestroySurfaceKHR(instance_, surface_, nullptr));
}

VTB_END
