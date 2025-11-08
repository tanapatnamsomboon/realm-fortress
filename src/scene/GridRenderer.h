/**
 * @file GridRenderer.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include <glm/glm.hpp>

namespace RealmFortress
{
    class Shader;

    /**
     * @class GridRenderer
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class GridRenderer
    {
    public:
        GridRenderer();
        ~GridRenderer();

        void Draw(const Shader& shader, const glm::mat4& vp);

    private:
        unsigned int m_VAO{ 0 };
        unsigned int m_VBO{ 0 };
        int m_VertexCount{ 0 };
    };
} // RealmFortress

