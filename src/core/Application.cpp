/**
 * @file Application.cpp
 * @brief
 * @date 11/8/2025
 */

#include "Application.h"
#include "core/Window.h"
#include "core/Time.h"
#include "core/Input.h"
#include "renderer/Shader.h"
#include "renderer/Renderer.h"
#include "scene/Scene.h"
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
        m_Scene = std::make_unique<Scene>();

        Renderer::Init();
        Input::Init(m_Window->GetNativeHandle());
    }

    Application::~Application()
    {
        Renderer::Shutdown();
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
            Renderer::Clear(m_ClearColor);

            glm::mat4 vp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix();
            m_Grid->Draw(*m_Shader, vp);
            m_Scene->Render(*m_Shader, vp);

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
} // namespace RealmFortress
