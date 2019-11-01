#include "vtb/Render.hpp"
#include "vtb/Device.hpp"
#include "vtb/SwapChain.hpp"
#include "vtb/Semaphore.hpp"

VTB_BEGIN

void Render::Create(
    const Device& device,
    const SwapChain& swapChain,
    const std::vector<VkCommandBuffer>& buffers)
{
    device_ = &device;
    swapChain_ = &swapChain;
    commandBuffers_ = &buffers;
    imageReadySemaphor_.Create(device.Get());
    renderFinishedSemaphore_.Create(device.Get());
}

void Render::Destroy()
{
    imageReadySemaphor_.Destroy();
    renderFinishedSemaphore_.Destroy();
}

void Render::Draw() const
{
    // wait for last frame
    VTB_CALL(vkQueueWaitIdle(device_->PresentQueue()));

    // get next frame
    uint32_t imageIndex = swapChain_->Next(imageReadySemaphor_.Get());
    
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = { imageReadySemaphor_.Get()};
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers_->at(imageIndex);
    
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore_.Get() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // The last parameter references an optional fence that will be signaled when the command buffers finish execution.
    // We're using semaphores for synchronization, so we'll just pass a VK_NULL_HANDLE.
    VTB_CALL(vkQueueSubmit(device_->GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));

    // Submit the result back to the swap chain to have it eventually show up on the screen.
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    // specify the swap chains to present images to and the index of the image for each swap chain.
    VkSwapchainKHR swapChains[] = { swapChain_->Get() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    VTB_CALL(vkQueuePresentKHR(device_->PresentQueue(), &presentInfo));
}

VTB_END

