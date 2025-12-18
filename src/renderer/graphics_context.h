/**
 * @file graphics_context.h
 * @brief
 * @date 11/28/2025
 */

#pragma once

#include "core/base.h"

struct GLFWwindow;

namespace RealmFortress
{
    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* window);

        void Init();
        void SwapBuffers();

    private:
        GLFWwindow* mWindow;
    };
} // namespace RealmFortress
