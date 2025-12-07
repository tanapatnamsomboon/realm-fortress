/**
 * @file graphics_context.cpp
 * @brief
 * @date 11/28/2025
 */

#include "core/pch.h"
#include "graphics_context.h"
#include "core/logger.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace RealmFortress
{
    GraphicsContext::GraphicsContext(GLFWwindow* window)
        : mWindow(window)
    {
        RF_CORE_ASSERT(window, "Window handle is null!");
    }

    void GraphicsContext::Init()
    {
        glfwMakeContextCurrent(mWindow);

        int status = gladLoadGL(glfwGetProcAddress);
        RF_CORE_ASSERT(status, "Failed to initialize GLAD!");

        RF_CORE_INFO("OpenGL info:");
        RF_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        RF_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        RF_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        int major;
        int minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        RF_CORE_ASSERT(major > 4 || (major == 4 && minor >= 6), "Realm Fortress requires at least OpenGL version 4.6!");
    }

    void GraphicsContext::SwapBuffers()
    {
        glfwSwapBuffers(mWindow);
    }
} // namespace RealmFortress
