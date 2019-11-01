#pragma once

#include "VulkanLocal.hpp"
#include "Vertex.hpp"

struct VulkanApp : Noncopyable
{
    VulkanApp() = default;
    virtual ~VulkanApp() = default;

    void Create(void *window, const vtb::ShaderData &vertData, const vtb::ShaderData &fragData, const char *textureFileName, const char *modelFileName);
    virtual void Render() const;
    virtual void Destroy();
    
protected:
    void SetupDebug();
    void CreateCommandBuffers();
    void UpdateUniformBuffer() const;

private:
    VkDebugReportCallbackEXT debugCallback_;
    VkPhysicalDevice physicalDevice_;
    vtb::Instance instance_;
    vtb::Surface surface_;
    vtb::Device device_;
    vtb::DepthBuffer depthBuffer_;
    vtb::SwapChain swapChain_;
    vtb::DescriptorSetLayout descriptorSetLayout_;
    vtb::DescriptorPool descriptorPool_;
    vtb::DescriptorSet descriptorSet_;
    vtb::RenderPass renderPass_;
    vtb::GraphicsPipeline<Vertex> graphicsPipeline_;
    vtb::Mesh<Vertex> mesh_;
    vtb::Buffer vertexBuffer_;
    vtb::Buffer indexBuffer_;
    vtb::UniformBuffer<UniformBufferObject> uniformBuffer_;
    vtb::CommandPool commandPool_;
    std::vector<VkCommandBuffer> commandBuffers_;
    vtb::Render render_;
    vtb::Image texture_;
    vtb::ImageView textureImageView_;
    vtb::Sampler sampler_;
};
