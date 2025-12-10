/**
 * @file game_layer.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "game_layer.h"
#include "core/logger.h"
#include "core/application.h"
#include "renderer/renderer.h"
#include <imgui.h>

namespace RealmFortress
{
    GameLayer::GameLayer()
        : Layer("GameLayer")
    {
        mShader = mShaderLibrary.Load("assets/shaders/basic.glsl");

        auto& window = Application::Get().GetWindow();
        f32 aspectRatio = static_cast<f32>(window.GetWidth()) / static_cast<f32>(window.GetHeight());
        mCameraController = CreateRef<CameraController>(aspectRatio);
    }

    void GameLayer::OnAttach()
    {
        RF_CORE_INFO("GameLayer attached");

        Renderer::SetClearColor(glm::vec4(0.53f, 0.81f, 0.92f, 1.0f));

        mCameraController->GetCamera().SetPosition(glm::vec3(0.0f, 15.0f, 15.0f));
        mCameraController->GetCamera().SetRotation(glm::vec3(-45.0f, 0.0f, 0.0f));

        mMap.GenerateHexagon(5, TileType::Grass);

        RF_CORE_INFO("Map generated with {} tiles", mMap.GetTileCount());
    }

    void GameLayer::OnDetach()
    {
        RF_CORE_INFO("GameLayer detached");
    }

    void GameLayer::OnUpdate(Timestep ts)
    {
        Layer::OnUpdate(ts);

        Renderer::BeginFrame();

        Renderer::Clear();

        mCameraController->OnUpdate(ts);

        Renderer::BeginScene(mCameraController->GetCamera());

        mShader->Bind();
        mShader->SetMat4("uViewProjection", Renderer::GetViewProjectionMatrix());

        mMap.Draw(mShader);

        Renderer::EndScene();

        Renderer::EndFrame();
    }

    void GameLayer::OnImGuiRender()
    {
        ImGui::Begin("Game Info");

        // Camera info
        auto camPos = mCameraController->GetCamera().GetPosition();
        auto camRot = mCameraController->GetCamera().GetRotation();

        ImGui::Text("Camera");
        ImGui::Text("  Position: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
        ImGui::Text("  Rotation: (%.2f, %.2f, %.2f)", camRot.x, camRot.y, camRot.z);

        ImGui::Separator();

        // Map info
        ImGui::Text("Hex Map");
        ImGui::Text("  Tiles: %zu", mMap.GetTileCount());

        ImGui::Separator();

        // Renderer stats
        auto& stats = Renderer::GetStats();
        ImGui::Text("Renderer Stats");
        ImGui::Text("  Draw Calls: %u", stats.DrawCalls);
        ImGui::Text("  Vertices: %u", stats.VertexCount);
        ImGui::Text("  Triangles: %u", stats.TriangleCount);

        ImGui::End();
    }

    void GameLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(RF_BIND_EVENT_FN(OnMouseButtonPressed));
        dispatcher.Dispatch<MouseMovedEvent>(RF_BIND_EVENT_FN(OnMouseMoved));
        dispatcher.Dispatch<KeyPressedEvent>(RF_BIND_EVENT_FN(OnKeyPressed));

        mCameraController->OnEvent(event);
    }

    bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
    {
        return false;
    }

    bool GameLayer::OnMouseMoved(MouseMovedEvent& event)
    {
        return false;
    }

    bool GameLayer::OnKeyPressed(KeyPressedEvent& event)
    {
        return false;
    }
} // namespace RealmFortress
