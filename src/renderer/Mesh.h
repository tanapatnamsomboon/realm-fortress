/**
 * @file Mesh.h
 * @brief
 * @date 11/12/2025
 */

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace RealmFortress
{
    class Shader;

    struct Vertex
    {
        glm::vec3 Position{};
        glm::vec3 Normal{};
        glm::vec2 TexCoords{};
    };

    /**
     * @class Mesh
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Mesh
    {
    public:
        Mesh(
            const std::vector<Vertex>&       vertices,
            const std::vector<unsigned int>& indices,
            unsigned int                     diffuse = 0
        );
        ~Mesh();

        void Draw(Shader& shader) const;

    private:
        void SetupMesh();

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<unsigned int> m_Indices;
        unsigned int m_VAO{ 0 }, m_VBO{ 0 }, m_EBO{ 0 };
        unsigned int m_Diffuse{ 0 };
    };
} // namespace RealmFortress
