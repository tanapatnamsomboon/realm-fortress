/**
 * @file Application.cpp
 * @brief
 * @date 11/8/2025
 */

#include "Application.h"
#include "core/Window.h"
#include "core/Time.h"
#include "renderer/Shader.h"
#include "scene/Camera.h"
#include "scene/GridRenderer.h"
#include "ui/ImGuiLayer.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

namespace RealmFortress
{
    Application::Application()
    {
        m_Window = std::make_unique<Window>(1280, 720, "Realm Fortress");
        m_Shader = std::make_unique<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
        m_Camera = std::make_unique<Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 500.0f);
        m_Grid = std::make_unique<GridRenderer>();
        m_ImGui = std::make_unique<ImGuiLayer>(m_Window->GetNativeHandle());
        InitTriangle();
    }

    Application::~Application()
    {
        if (m_VBO) glDeleteBuffers(1, &m_VBO);
        if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    }

    void Application::Run()
    {
        while (!m_Window->ShouldClose()) {
            Time::BeginFrame();
            float dt = Time::Delta();

            // input
            auto* native = m_Window->GetNativeHandle();
            if (glfwGetKey(native, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(native, 1);

            // render
            glEnable(GL_DEPTH_TEST);
            glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DrawTriangle();

            m_ImGui->Begin();
            ImGui::Begin("Stats");
            ImGui::Text("FPS: %.1f", 1.0f / dt);
            auto pos = m_Camera->Position();
            ImGui::Text("Camera Pos: %.1f %.1f %.1f", pos.x, pos.y, pos.z);
            ImGui::End();
            m_ImGui->End();

            m_Window->SwapBuffers();
            m_Window->PollEvents();
        }
    }

    void Application::InitTriangle()
    {
        float vb[] = {
            // pos (x,y)   // color (r,g,b)
            -0.6f, -0.5f,  0.9f, 0.2f, 0.2f,
             0.6f, -0.5f,  0.2f, 0.9f, 0.2f,
             0.0f,  0.6f,  0.2f, 0.4f, 0.9f
        };
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vb), vb, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    void Application::DrawTriangle()
    {
        glm::mat4 vp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix();
        m_Grid->Draw(*m_Shader, vp);
    }
} // namespace RealmFortress
