/**
 * @file HighlightRenderer.cpp
 * @brief
 * @date 11/12/2025
 */

#include "HighlightRenderer.h"
#include "renderer/Shader.h"
#include "renderer/Mesh.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

namespace RealmFortress
{
    HighlightRenderer::HighlightRenderer()
    {
        m_Shader = std::make_unique<Shader>(
            "assets/shaders/highlight.vert",
            "assets/shaders/highlight.frag"
        );

        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        constexpr int N = 6;
        constexpr float ROT_OFFSET = glm::radians(30.0f);
        for (int i = 0; i < N; ++i)
        {
            constexpr float R = 1.1f;
            float angle = ROT_OFFSET + glm::radians(60.0f * i);
            vertices.push_back({ { R * cos(angle), 0.0f, R * sin(angle) }, { 0, 1, 0 }, { 0, 0 }});
        }

        vertices.push_back({{ 0, 0, 0 }, { 0, 1, 0 }, { 0, 0 }});
        unsigned centerIdx = static_cast<unsigned>(vertices.size()) - 1;
        for (int i = 0; i < N; ++i)
            indices.insert(indices.end(), { centerIdx, static_cast<unsigned>(i), static_cast<unsigned>((i + 1) % N) });

        m_HexMesh = std::make_unique<Mesh>(vertices, indices);
    }

    HighlightRenderer::~HighlightRenderer() = default;

    void HighlightRenderer::Draw(const glm::vec3& position, const glm::mat4& vp)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position + glm::vec3(0, 0.05f, 0));
        glm::mat4 mvp = vp * model;

        m_Shader->Use();
        m_Shader->SetMat4("uMVP", mvp);
        m_Shader->SetVec3("uColor", {1.0f, 0.9f, 0.2f});

        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        m_HexMesh->Draw(*m_Shader);
        m_Shader->SetVec3("uColor", { 1.0f, 1.0f, 0.0f });
        m_HexMesh->Draw(*m_Shader);

        glEnable(GL_CULL_FACE);
    }
} // namespace RealmFortress
