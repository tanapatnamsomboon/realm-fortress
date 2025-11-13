/**
 * @file HighlightRenderer.h
 * @brief
 * @date 11/12/2025
 */

#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace RealmFortress
{
    class Shader;
    class Mesh;

    /**
     * @class HighlightRenderer
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class HighlightRenderer
    {
    public:
        HighlightRenderer();
        ~HighlightRenderer();

        void Draw(const glm::vec3& position, const glm::mat4& vp);

    private:
        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Mesh> m_HexMesh;
    };
} // namespace RealmFortress
