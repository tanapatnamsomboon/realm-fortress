/**
 * @file Mesh.cpp
 * @brief
 * @date 11/12/2025
 */

#include "Mesh.h"
#include <glad/gl.h>

namespace RealmFortress
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, unsigned int diffuse)
       : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_Diffuse(diffuse)
    {
        SetupMesh();
    }

    Mesh::~Mesh()
    {
        if (m_EBO) glDeleteBuffers(1, &m_EBO);
        if (m_VBO) glDeleteBuffers(1, &m_VBO);
        if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    }

    void Mesh::Draw(Shader& shader) const
    {
        if (m_Diffuse)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_Diffuse);
        }
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::SetupMesh()
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned), m_Indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Position)));

        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

        glEnableVertexAttribArray(2); // texture coordinate
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

        glBindVertexArray(0);
    }
} // namespace RealmFortress
