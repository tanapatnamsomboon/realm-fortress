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
    class Mesh;
    class Shader;

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

        void Draw(Shader& shader) const;
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
