#include <iostream>
#include <stdexcept>
#include <chrono>

#include "vtb/Glm.hpp"
#include "vtb/Glfw.hpp"
#include "vtb/App.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void VulkanApp::Create(
    void *window, 
    const vtb::ShaderData &vertData, 
    const vtb::ShaderData &fragData, 
    const char *textureFileName, 
    const char *modelFileName)
{
    instance_.Create(true);
    vtb::Trace::Create(instance_.Get());
    surface_.Create(instance_.Get(), window);
    physicalDevice_ = vtb::PickPhysicalDevice(instance_.Get(), surface_.Get());
    device_.Create(physicalDevice_, surface_.Get());
    auto physical = physicalDevice_;
    auto logical = device_.Get();
    auto queue = device_.GraphicsQueue();
    swapChain_.Create(logical, physical, surface_.Get());
    renderPass_.Create(logical, physical, swapChain_.ImageFormat());
    commandPool_.Create(logical, physical, surface_.Get());
    texture_.Create(logical, physical, commandPool_.Get(), queue, textureFileName);
    textureImageView_.Create(texture_);
    sampler_.Create(logical);
    uniformBuffer_.Create(logical, physical);
    descriptorSetLayout_.Build(logical)
        .AddBuffer()
        .AddSampler();
    descriptorPool_.Build(logical)
        .AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        .AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    descriptorSet_.Build(logical, descriptorPool_.Get(), descriptorSetLayout_)
        .AddBuffer(uniformBuffer_.Get(), sizeof(UniformBufferObject))
        .AddSampler(textureImageView_.Get(), sampler_.Get());
    graphicsPipeline_.Create(logical, descriptorSetLayout_, swapChain_.Extent(), renderPass_.Get(), vertData, fragData);
    depthBuffer_.Create(logical, physical, commandPool_, device_.GraphicsQueue(), swapChain_.Extent());
    swapChain_.CreateFrameBuffers(depthBuffer_.ImageView(), renderPass_.Get());
    mesh_.Load(modelFileName);
    vertexBuffer_.Create(logical, physical, commandPool_.Get(), queue, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, mesh_.GetVertices());
    indexBuffer_.Create(logical, physical, commandPool_.Get(), queue, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, mesh_.GetIndices());

    CreateCommandBuffers();
    render_.Create(device_, swapChain_, commandBuffers_);
}

void VulkanApp::Destroy()
{
    renderPass_.Destroy();
    textureImageView_.Destroy();
    texture_.Destroy();
    sampler_.Destroy();
    swapChain_.Destroy();
    descriptorPool_.Destroy();
    descriptorSetLayout_.Destroy();
    uniformBuffer_.Destroy();
    vertexBuffer_.Destroy();
    indexBuffer_.Destroy();
    commandPool_.Destroy();
    graphicsPipeline_.Destroy();
    render_.Destroy();
    surface_.Destroy();
    device_.Destroy();
    vtb::Trace::Destroy();
    instance_.Destroy();
}

void VulkanApp::CreateCommandBuffers()
{
    // the draw commands to add to each buffer
    auto draw = [this](VkCommandBuffer buf)
    {
        VkBuffer vertexBuffers[] = { vertexBuffer_.Get() };
        VkDeviceSize offsets[] = { 0 };
        VTB_CALLV(vkCmdBindVertexBuffers(buf, 0, 1, vertexBuffers, offsets));
        VTB_CALLV(vkCmdBindIndexBuffer(buf, indexBuffer_.Get(), 0, VK_INDEX_TYPE_UINT32));
        VTB_CALLV(vkCmdDrawIndexed(buf, static_cast<uint32_t>(mesh_.NumIndices()), 1, 0, 0, 0));
    };

    VkDescriptorSet sets[] = { descriptorSet_.Get() };
    commandBuffers_ = vtb::CommandBuffer::CreateBuffers(
        device_.Get(), renderPass_.Get(), swapChain_, graphicsPipeline_, commandPool_, 1, sets, draw);
}

void VulkanApp::Render() const
{
    UpdateUniformBuffer();
    render_.Draw();
}

void VulkanApp::UpdateUniformBuffer() const
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    UniformBufferObject ubo = {};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain_.Width() / (float) swapChain_.Height(), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1; // OpenGL inverts the Y coordinate of clip-space. Invert it back.

    ubo.mvp = ubo.proj*ubo.view*ubo.model;
    uniformBuffer_.Write(ubo);
}
