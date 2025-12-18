/**
 * @file mesh.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/vertex_array.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include <glm/glm.hpp>
#include <vector>

namespace RealmFortress
{
    struct Vertex
    {
        glm::vec3 mPosition;
        glm::vec3 mNormal;
        glm::vec2 mTexCoords;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices, const std::vector<Ref<Texture2D>>& textures);

        void Draw(const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

        const std::vector<Vertex>& GetVertices() const { return mVertices; }
        const std::vector<u32>& GetIndices() const { return mIndices; }
        const std::vector<Ref<Texture2D>>& GetTextures() const { return mTextures; }
        Ref<VertexArray> GetVertexArray() const { return mVertexArray; }

    private:
        void SetupMesh();

    private:
        std::vector<Vertex> mVertices;
        std::vector<u32> mIndices;
        std::vector<Ref<Texture2D>> mTextures;

        Ref<VertexArray> mVertexArray;
        Ref<VertexBuffer> mVertexBuffer;
        Ref<IndexBuffer> mIndexBuffer;
    };
}
