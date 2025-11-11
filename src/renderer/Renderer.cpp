/**
 * @file Renderer.cpp
 * @brief
 * @date 11/9/2025
 */

#include "Renderer.h"
#include <glad/gl.h>

namespace RealmFortress
{
    void Renderer::Init()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    void Renderer::Clear(const glm::vec3& color)
    {
        glClearColor(color.r, color.g, color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::OnWindowResize(unsigned int width, unsigned int height)
    {
        glViewport(0, 0, width, height);
    }
} // namespace RealmFortress
