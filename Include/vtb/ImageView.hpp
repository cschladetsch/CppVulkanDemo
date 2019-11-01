#pragma once

#include "Image.hpp"

VTB_BEGIN

struct ImageView : ResourceBase<VkImageView>
{
    using Base = ResourceBase<VkImageView>;

    void Create(const Image& texture) { Create(texture.Device(), texture.Get()); }
    void Create(VkDevice device, VkImage image);
    void Destroy() override;
};

VkImageView CreateImageView(
    VkDevice device, 
    VkImage image, 
    VkFormat format, 
    VkImageAspectFlags aspectFlags);

VTB_END
