/**
 * @file Renderer.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include <glm/glm.hpp>

namespace RealmFortress
{
    /**
     * @class Renderer
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Renderer
    {
    public:
        static void Init();
        static void Clear(const glm::vec3& color);
        static void Shutdown();
    };
} // namespace RealmFortress
