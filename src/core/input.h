/**
 * @file input.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include "core/base.h"
#include "core/key_codes.h"
#include "core/mouse_codes.h"
#include <glm/glm.hpp>

namespace RF
{
    /**
     * @class Input
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode key);
        static bool IsMouseButtonPressed(MouseCode button);
        static glm::vec2 GetMousePosition();
        static f32 GetMouseX();
        static f32 GetMouseY();
    };
} // namespace RF
