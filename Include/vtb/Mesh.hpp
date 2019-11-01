#pragma once

#include <vector>
#include <unordered_map>

#include "tiny_obj_loader.h"

#include "Config.hpp"

VTB_BEGIN

template <class Vertex, class Index = uint32_t>
struct Mesh
{
    using Vertices = std::vector<Vertex>;
    using Indices = std::vector<Index>;

    bool Load(const char *fileName)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName))
            throw std::runtime_error(err);

        std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex = {};
                vertex.pos =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord =
                {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]    // account for u-v flip
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices_.size());
                    vertices_.push_back(vertex);
                }

                indices_.push_back(uniqueVertices[vertex]);
            }
        }

        return true;
    }

    const Vertices& GetVertices() const { return vertices_; }
    const Indices& GetIndices() const { return indices_; }

    size_t NumIndices() const { return indices_.size(); }
    size_t NumVertices() const { return vertices_.size(); }

private:
    Vertices vertices_;
    Indices indices_;
};

VTB_END
