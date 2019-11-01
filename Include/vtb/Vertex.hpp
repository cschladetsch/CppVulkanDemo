#pragma once

#include <array>

#include "Glm.hpp"

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 mvp;
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    
    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
    
    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        
        // The binding parameter specifies the index of the binding in the array of bindings.
        // The stride parameter specifies the number of bytes from one entry to the next, and the inputRate
        // parameter can have one of the following values:
        // VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
        // VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }
    
    using Attributes = std::array<VkVertexInputAttributeDescription, 3>;
    static Attributes GetAttributeDescriptions()
    {
        Attributes attributeDescriptions = {};
        
        // The binding parameter tells Vulkan from which binding the per-vertex data comes.
        // The location parameter references the location directive of the input in the vertex shader.
        // The input in the vertex shader with location 0 is the position, which has two 32-bit float components.
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        // add the color component
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        
        // texture coord
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        
        return attributeDescriptions;
    }
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}
