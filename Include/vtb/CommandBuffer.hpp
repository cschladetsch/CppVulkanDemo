#include <array>
#include <functional>

#include "SwapChain.hpp"
#include "GraphicsPipeline.hpp"

VTB_BEGIN

struct CommandBuffer
{
    template <typename Vertex>
    static std::vector<VkCommandBuffer> CreateBuffers(
        VkDevice device,
        VkRenderPass renderPass,
        const SwapChain &swapChain,
        const GraphicsPipeline<Vertex> &pipeline,
        VkCommandPool commandPool,
        uint32_t numDescriptorSets,
        const VkDescriptorSet *descriptorSets,
        std::function<void (VkCommandBuffer)> draw)
    {
        auto numBuffers = swapChain.FrameBuffers().size();
        std::vector<VkCommandBuffer> commandBuffers{numBuffers};
        
        // The level parameter specifies if the allocated command buffers are primary or secondary command buffers.
        // VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers.
        // VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers.
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)numBuffers;
        
        VTB_CALL(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));

        for (uint32_t i = 0; i < numBuffers; i++)
        {
            CreateBuffer(
                commandBuffers[i],
                renderPass, 
                swapChain.FrameBuffer(i),
                swapChain.Extent(),
                pipeline,
                numDescriptorSets,
                descriptorSets,
                draw);
        }

        return commandBuffers;
    }

    template <typename Vertex>
    static void CreateBuffer(
        VkCommandBuffer buf,
        VkRenderPass renderPass,
        VkFramebuffer frameBuffer,
        VkExtent2D extent,
        const GraphicsPipeline<Vertex> &pipeline,
        uint32_t numDescriptorSets,
        const VkDescriptorSet *descriptorSets,
        std::function<void (VkCommandBuffer)> draw)
    {
        // The flags parameter specifies how we're going to use the command buffer. The following values are available:
        // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
        // VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
        // VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution.
        //
        // The pInheritanceInfo parameter is only relevant for secondary command buffers. It specifies which state to inherit
        // from the calling primary command buffers.
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr;

        // If the command buffer was already recorded once, then a call to vkBeginCommandBuffer will implicitly reset it.
        // It's not possible to append commands to a buffer at a later time.
        VTB_CALL(vkBeginCommandBuffer(buf, &beginInfo));

        // start drawing
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = frameBuffer;

        // The first parameters are the render pass itself and the attachments to bind. We created a framebuffer for
        // each swap chain image that specifies it as color attachment.
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = extent;

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        // The last two parameters define the clear values to use for VK_ATTACHMENT_LOAD_OP_CLEAR, which we used
        // as load operation for the color attachment.
        // VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded in the primary command buffer itself
        //      and no secondary command buffers will be executed.
        // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass commands will be executed from secondary command buffers.
        VTB_CALLV(vkCmdBeginRenderPass(buf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE));
        VTB_CALLV(vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Get()));
        VTB_CALLV(vkCmdBindDescriptorSets(
            buf, 
            VK_PIPELINE_BIND_POINT_GRAPHICS, 
            pipeline.PipelineLayout(), 
            0, 
            numDescriptorSets, 
            descriptorSets,
            0, 
            nullptr));

        draw(buf);

        VTB_CALLV(vkCmdEndRenderPass(buf));

        VTB_CALL(vkEndCommandBuffer(buf));
    }
};

VTB_END
