/**
 * @file Model.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace RealmFortress
{
    struct Vertex
    {
        glm::vec3 Position{};
        glm::vec3 Normal{};
        glm::vec2 UV{};
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
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, unsigned int diffuse);
        ~Mesh();

        void Draw() const;

    private:
        unsigned int m_VAO{ 0 }, m_VBO{ 0 }, m_EBO{ 0 };
        unsigned int m_Diffuse{ 0 };
        int m_IndexCount{ 0 };
    };

    /**
     * @class Model
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Model
    {
    public:
        explicit Model(const std::string& path);
        ~Model() = default;

        void Draw() const;
        unsigned int GetDiffuse() const { return m_DefaultDiffuse; }

    private:
        void Load(const std::string& path);

    private:
        std::vector<Mesh> m_Meshes;
        std::string m_Directory;
        unsigned int m_DefaultDiffuse{ 0 };
    };

    unsigned int LoadTexture2D(const std::string& path, bool srgb = false);
} // namespace RealmFortress
