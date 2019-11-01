#pragma once

#include "Vulkan.hpp"
#include "Semaphore.hpp"

VTB_BEGIN

struct Render
{
    void Create(
        const Device& device,
        const SwapChain& swapChain,
        const std::vector<VkCommandBuffer>& commandBuffers);
    void Destroy();

    void Draw() const;

private:
    const Device *device_;
    const SwapChain *swapChain_;
    const std::vector<VkCommandBuffer> *commandBuffers_;
    Semaphore imageReadySemaphor_;
    Semaphore renderFinishedSemaphore_;
};

VTB_END
