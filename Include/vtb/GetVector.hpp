#pragma once

#include <vector>
#include "Vulkan.hpp"
#include "Debug.hpp"

VTB_BEGIN

template <class Device, class Object>
std::vector<Object> GetVector(Device device, void (VKAPI_CALL* Fun)(Device, uint32_t*, Object*))
{
    uint32_t count = 0;
    Fun(device, &count, nullptr);
    std::vector<Object> data(count);
    Fun(device, &count, data.data());
    return data;
}

template <class Device, class Object>
std::vector<Object> GetVector(Device device, VkResult (VKAPI_CALL* Fun)(Device, uint32_t*, Object*))
{
    uint32_t count = 0;
    VTB_CALL(Fun(device, &count, nullptr));
    
    std::vector<Object> data(count);
    VTB_CALL(Fun(device, &count, data.data()));
    
    return data;
}

template <class Device, class Object>
std::vector<Object> GetVector(Device device, VkResult (VKAPI_CALL* Fun)(Device, const char* name, uint32_t*, Object*), const char* name = nullptr)
{
    uint32_t count = 0;
    VTB_CALL(Fun(device, name, &count, nullptr));
    
    std::vector<Object> data(count);
    VTB_CALL(Fun(device, name, &count, data.data()));
    
    return data;
}

template <class Object>
std::vector<Object> GetVector(VkResult (VKAPI_CALL* Fun)(uint32_t*, Object*))
{
    uint32_t count = 0;
    VTB_CALL(Fun(&count, nullptr));
    
    std::vector<Object> data(count);
    VTB_CALL(Fun(&count, data.data()));
    
    return data;
}

VTB_END
