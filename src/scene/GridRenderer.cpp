/**
 * @file GridRenderer.cpp
 * @brief
 * @date 11/8/2025
 */

#include "GridRenderer.h"
#include "renderer/Shader.h"
#include <glad/gl.h>
#include <vector>

namespace RealmFortress
{
    GridRenderer::GridRenderer()
    {
        std::vector<float> lines;
        const int range = 50;
        for (int i = -range; i <= range; ++i)
        {
            // vertical
            lines.push_back(static_cast<float>(i)); lines.push_back(0.0f); lines.push_back(static_cast<float>(-range));
            lines.push_back(static_cast<float>(i)); lines.push_back(0.0f); lines.push_back(static_cast<float>(range));
            // horizontal
            lines.push_back(static_cast<float>(-range)); lines.push_back(0.0f); lines.push_back(static_cast<float>(i));
            lines.push_back(static_cast<float>(range)); lines.push_back(0.0f); lines.push_back(static_cast<float>(i));
        }
        m_VertexCount = static_cast<int>(lines.size()) / 3;

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(float), lines.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glBindVertexArray(0);
    }

    GridRenderer::~GridRenderer()
    {
        if (m_VBO) glDeleteBuffers(1, &m_VBO);
        if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    }

    void GridRenderer::Draw(const Shader& shader, const glm::mat4& vp)
    {
        shader.Use();
        shader.SetMat4("uVP", vp);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_LINES, 0, m_VertexCount);
        glBindVertexArray(0);
    }
} // namespace RealmFortress
