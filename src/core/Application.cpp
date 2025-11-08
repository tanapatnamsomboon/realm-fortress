/**
 * @file Application.cpp
 * @brief
 * @date 11/8/2025
 */

#include "Application.h"
#include "core/Window.h"
#include "renderer/Shader.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

namespace RealmFortress
{
    Application::Application()
    {
        m_Window = std::make_unique<Window>(1280, 720, "Realm Fortress");
        m_Shader = std::make_unique<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
        InitTriangle();
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
    }

    void Application::InitTriangle()
    {
    }

    void Application::DrawTriangle()
    {
    }
} // namespace RealmFortress
