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
#include "renderer/Model.h"
#include "renderer/Mesh.h"
#include "scene/Scene.h"
#include "scene/Camera.h"
#include "scene/GridRenderer.h"
#include "ui/ImGuiLayer.h"
#include "events/Event.h"
#include "events/ApplicationEvent.h"
#include "events/MouseEvent.h"
#include "events/KeyEvent.h"
#include "gameplay/HexGrid.h"
#include "gameplay/TileSelector.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

namespace RealmFortress
{
    Application::Application()
    {
        m_Window = std::make_unique<Window>(WindowProps());
        m_Window->SetEventCallback([this](Event& event){ OnEvent(event); });

        Renderer::Init();
        Input::Init(m_Window->GetNativeHandle());

        m_Camera = std::make_unique<Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 500.0f);
        m_Grid = std::make_unique<GridRenderer>();
        m_Scene = std::make_unique<Scene>();
        m_ImGui = std::make_unique<ImGuiLayer>(m_Window->GetNativeHandle());

        m_Shader = std::make_unique<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
        m_TerrainShader = std::make_unique<Shader>("assets/shaders/basic_terrain.vert", "assets/shaders/basic_terrain.frag");

        m_GrassModel = std::make_unique<Model>("assets/objects/tiles/base/hex_grass.gltf");

        m_HexGrid = std::make_unique<HexGrid>(20, 20, 1.0f);
        m_HexGrid->SetModel(TileKind::Grass, m_GrassModel.get(), 1.0f);
        m_HexGrid->GenerateFlat(TileKind::Grass);

        m_TileSelector = std::make_unique<TileSelector>();
    }

    Application::~Application()
    {
        Renderer::Shutdown();
    }

    void Application::Run()
    {
        while (!m_Window->ShouldClose())
        {
            Time::BeginFrame();
            Input::Update();

            float dt = Time::Delta();

            m_Camera->SetViewport(static_cast<float>(m_Window->GetWidth()), static_cast<float>(m_Window->GetHeight()));
            m_Camera->Update(dt);

            auto [mx, my] = Input::GetMousePosition();
            m_TileSelector->Update(*m_Camera, *m_HexGrid, mx, my, m_Window->GetWidth(), m_Window->GetHeight());

            Renderer::Clear(m_ClearColor);

            glm::mat4 vp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix();

            // m_Grid->Draw(*m_Shader, vp);

            m_Scene->Render(*m_Shader, vp);

            m_TerrainShader->Use();
            m_HexGrid->Render(*m_TerrainShader, vp);

            m_TileSelector->RenderHighlight(vp);

            m_ImGui->Begin();
            ImGui::Begin("Stats");
            ImGui::Text("FPS: %.1f", 1.0f / dt);
            glm::vec3 camPos = m_Camera->Position();
            ImGui::Text("Camera Pos: %.2f, %.2f, %.2f", camPos.x, camPos.y, camPos.z);
            if (auto select = m_TileSelector->GetSelected())
            {
                const HexTile* tile = *select;
                ImGui::Separator();
                ImGui::Text("Selected: q=%d r=%d", tile->Coord.Q, tile->Coord.R);
                ImGui::Text("Kind: %d", static_cast<int>(tile->Kind));
            }
            ImGui::End();
            m_ImGui->End();

            m_Window->SwapBuffers();
            m_Window->PollEvents();
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e){ return OnWindowResize(e); });

        m_Camera->OnEvent(event);
    }

    bool Application::OnWindowResize(WindowResizeEvent& event)
    {
        Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());
        return false;
    }
} // namespace RealmFortress
