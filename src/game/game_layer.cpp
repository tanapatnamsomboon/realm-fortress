/**
 * @file game_layer.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "game_layer.h"
#include "core/logger.h"
#include "core/application.h"
#include "core/input.h"
#include "renderer/renderer.h"
#include <imgui.h>

namespace RealmFortress
{
    GameLayer::GameLayer()
        : Layer("GameLayer")
    {
        mShader = mShaderLibrary.Load("assets/shaders/basic.glsl");
        mHighlightShader = mShaderLibrary.Load("assets/shaders/highlight.glsl");

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

        mTime += ts;

        Renderer::BeginFrame();
        Renderer::Clear();

        mCameraController->OnUpdate(ts);

        UpdateSelection();

        Renderer::BeginScene(mCameraController->GetCamera());

        mShader->Bind();
        mShader->SetMat4("uViewProjection", Renderer::GetViewProjectionMatrix());

        mHighlightShader->Bind();
        mHighlightShader->SetMat4("uViewProjection", Renderer::GetViewProjectionMatrix());

        std::unordered_set<Coordinate> highlighted_tiles;
        if (mSelection.HasHover())
        {
            highlighted_tiles.insert(mSelection.GetHovered().value());
        }
        if (mSelection.HasSelection())
        {
            highlighted_tiles.insert(mSelection.GetSelected().value());
        }

        glm::vec3 highlight_color(1.0f, 1.0f, 0.0f);

        if (mSelection.HasSelection())
        {
            highlight_color = glm::vec3(0.0f, 1.0f, 0.5f);
        }

        mMap.DrawWithHighlight(mShader, mHighlightShader, highlighted_tiles, highlight_color, mTime);

        mBuildingManager.Draw(mShader);

        if (mBuildMode && mSelection.HasHover())
        {
            auto coord = mSelection.GetHovered().value();
            mBuildingManager.SetPreview(mSelectedBuildingType, mPlayerFaction, coord);
            mBuildingManager.DrawPreview(mHighlightShader, mMap, mTime);
        }

        Renderer::EndScene();
        Renderer::EndFrame();
    }

    void GameLayer::OnImGuiRender()
    {
            ImGui::Begin("Game Info");

            // Camera info
            auto cam_pos = mCameraController->GetCamera().GetPosition();
            auto cam_pot = mCameraController->GetCamera().GetRotation();

            ImGui::Text("Camera");
            ImGui::Text("  Position: (%.2f, %.2f, %.2f)", cam_pos.x, cam_pos.y, cam_pos.z);
            ImGui::Text("  Rotation: (%.2f, %.2f, %.2f)", cam_pot.x, cam_pot.y, cam_pot.z);

        ImGui::Separator();

            // Selection info
            ImGui::Text("Selection");

            if (mSelection.HasHover())
            {
                auto hovered = mSelection.GetHovered().value();
                ImGui::Text("  Hovered: (%d, %d)", hovered.Q, hovered.R);

                if (auto* tile = mMap.GetTile(hovered))
                {
                    ImGui::Text("  Type: %s", TileTypeToString(tile->GetType()));
                    ImGui::Text("  Elevation: %d", tile->GetElevation());
                }
            }
            else
            {
                ImGui::Text("  Hovered: None");
            }

            if (mSelection.HasSelection())
            {
                auto selected = mSelection.GetSelected().value();
                ImGui::Text("  Selected: (%d, %d)", selected.Q, selected.R);
            }
            else
            {
                ImGui::Text("  Selected: None");
            }

        ImGui::Separator();

            ImGui::Text("Buildings");
            ImGui::Text("  Count: %zu", mBuildingManager.GetBuildingCount());
            ImGui::Text("  Build Mode: %s", mBuildMode ? "ON" : "OFF");

            if (mBuildMode)
            {
                ImGui::Text("  Type: %s", BuildingTypeToString(mSelectedBuildingType));
                ImGui::Text("  Faction: %s", FactionToString(mPlayerFaction));
            }

        ImGui::Separator();

            // Controls info
            ImGui::Text("Controls");
            ImGui::Text("  WASD - Move camera");
            ImGui::Text("  Right-click + Drag - Rotate");
            ImGui::Text("  Mouse Wheel - Zoom");
            ImGui::Text("  Q/E - Rotate around map");
            ImGui::Text("  Left-click - Select / Place");
            ImGui::Text("  B - Toggle build mode");
            ImGui::Text("  1-5 - Select building (in build)");
            ImGui::Text("  Escape - Exit build / Clear sel");

        ImGui::Separator();

            // Map info
            ImGui::Text("Map");
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
        if (event.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (mBuildMode && mSelection.HasHover())
            {
                auto coord = mSelection.GetHovered().value();
                if (mBuildingManager.PlaceBuilding(mSelectedBuildingType, mPlayerFaction, coord, mMap))
                {
                    RF_CORE_INFO("Building placed at ({}, {})", coord.Q, coord.R);
                }
                return true;
            }

            if (mSelection.HasHover())
            {
                mSelection.Select(mSelection.GetHovered().value());
                RF_CORE_INFO("Selected: ({}, {})",
                    mSelection.GetSelected().value().Q,
                    mSelection.GetSelected().value().R);
            }
            return true;
        }

        return false;
    }

    bool GameLayer::OnMouseMoved(MouseMovedEvent& event)
    {
        return false;
    }

    bool GameLayer::OnKeyPressed(KeyPressedEvent& event)
    {
        if (event.GetKeyCode() == Key::Escape)
        {
            if (mBuildMode)
            {
                mBuildMode = false;
                mBuildingManager.ClearPreview();
                RF_CORE_INFO("Exited build mode");
            }
            else
            {
                mSelection.ClearSelection();
            }
            return true;
        }

        if (event.GetKeyCode() == Key::B)
        {
            mBuildMode = !mBuildMode;
            if (!mBuildMode)
                mBuildingManager.ClearPreview();
            RF_CORE_INFO("Build mode: {}", mBuildMode ? "ON" : "OFF");
            return true;
        }

        if (mBuildMode)
        {
            if (event.GetKeyCode() == Key::D1) mSelectedBuildingType = BuildingType::TownHall;
            else if (event.GetKeyCode() == Key::D2) mSelectedBuildingType = BuildingType::HomeA;
            else if (event.GetKeyCode() == Key::D3) mSelectedBuildingType = BuildingType::Barracks;
        }

        return false;
    }

    void GameLayer::UpdateSelection()
    {
        glm::vec2 mouse_pos = Input::GetMousePosition();

        auto& window = Application::Get().GetWindow();
        u32 width = window.GetWidth();
        u32 height = window.GetHeight();

        auto picked_tile = mPicker.Pick(mouse_pos.x, mouse_pos.y, width, height, mCameraController->GetCamera());

        if (picked_tile.has_value() && mMap.HasTile(picked_tile.value()))
        {
            mSelection.SetHovered(picked_tile.value());
        }
        else
        {
            mSelection.ClearHover();
        }
    }
} // namespace RealmFortress
