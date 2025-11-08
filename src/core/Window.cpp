/**
 * @file Window.cpp
 * @brief
 * @date 11/8/2025
 */

#include "Window.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace RealmFortress
{
    Window::Window(int width, int height, const std::string& title)
        : m_Width(width), m_Height(height)
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to init GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#   if defined(RF_PLATFORM_MAC)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#   endif

        m_Handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!m_Handle)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_Handle);

        if (!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error("Failed to load GL with glad");

        glfwSwapInterval(m_VSync);

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(m_Handle, FramebufferSizeCallback);
        glfwSetWindowUserPointer(m_Handle, this);
    }

    Window::~Window()
    {
        if (m_Handle)
        {
            glfwDestroyWindow(m_Handle);
            m_Handle = nullptr;
        }
        glfwTerminate();
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Handle);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Handle);
    }

    void Window::SetVSync(bool enabled)
    {
        m_VSync = enabled;
        glfwSwapInterval(m_VSync ? 1 : 0);
    }

    void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        if (auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window)))
        {
            self->m_Width = width;
            self->m_Height = height;
        }
    }
} // namespace RealmFortress
