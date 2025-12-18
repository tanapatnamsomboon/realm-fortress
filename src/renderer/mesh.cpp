/**
 * @file mesh.cpp
 * @brief
 * @date 11/21/2025
 */

#include "mesh.h"
#include "core/logger.h"
#include "renderer/renderer.h"

namespace RealmFortress
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices, const std::vector<Ref<Texture2D>>& textures)
        : mVertices(vertices), mIndices(indices), mTextures(textures)
    {
        SetupMesh();
    }

    void Mesh::Draw(const Ref<Shader>& shader, const glm::mat4& transform)
    {
        for (u32 i = 0; i < mTextures.size(); i++)
        {
            mTextures[i]->Bind(i);
            shader->SetInt("uTexture", i);
        }

        Renderer::DrawMesh(shader, mVertexArray, transform);
    }

    void Mesh::DrawInstanced(const Ref<Shader>& shader, const std::vector<glm::mat4>& transforms)
    {
        for (u32 i = 0; i < mTextures.size(); i++)
        {
            mTextures[i]->Bind(i);
            shader->SetInt("uTexture", i);
        }

        Renderer::DrawInstancedMesh(shader, mVertexArray, transforms);
    }

    void Mesh::SetupMesh()
    {
        mVertexArray = VertexArray::Create();
        mVertexBuffer = VertexBuffer::Create(reinterpret_cast<float*>(mVertices.data()), mVertices.size() * sizeof(Vertex));
        mVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float3, "aNormal" },
            { ShaderDataType::Float2, "aTexCoords" }
        });

        mVertexArray->AddVertexBuffer(mVertexBuffer);

        mIndexBuffer = IndexBuffer::Create(mIndices.data(), mIndices.size());
        mVertexArray->SetIndexBuffer(mIndexBuffer);
    }
} // namespace RealmFortress
