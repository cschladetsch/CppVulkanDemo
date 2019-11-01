#pragma once

#include "ResourceBase.hpp"

VTB_BEGIN

struct Instance : ResourceBase<VkInstance>
{
    void Create(bool enableValidationLayers);
	void Destroy();

	VkInstance Get() const { return instance_; }

private:
	VkInstance instance_;
};
VTB_END
