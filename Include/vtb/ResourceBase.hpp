#pragma once

#include "ResourceCommon.hpp"

VTB_BEGIN

// An explicitly typed resource.
template <class Resource>
struct ResourceBase : ResourceCommon
{
    void Create(VkDevice device) override { device_ = device; }
    void Create(VkDevice device, Resource resource)
    {
        Create(device);
        resource_ = resource;
    }

    void Recreate() override { throw new std::runtime_error("Cannot recreate resource"); }
    Resource Get() const { return resource_; }

    operator Resource() const { return resource_; }

protected:
    Resource resource_ = {};
};

VTB_END
