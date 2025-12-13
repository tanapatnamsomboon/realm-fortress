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

        mUIManager.GetBuildPanel().SetOnBuildingSelected([this](BuildingType type)
        {
            mSelectedBuildingType = type;
            mBuildMode = true;
            RF_CORE_INFO("Building selected: {}", BuildingTypeToString(type));
        });

        mUIManager.GetBuildPanel().SetOnDecorationSelected([this](DecorationType type)
        {
            mSelectedDecorationType = type;
            mDecorationMode = true;
            RF_CORE_INFO("Decoration selected: {}", DecorationTypeToString(type));
        });

        mUIManager.GetBuildPanel().SetOnStructureSelected([this](StructureType type) {
            mSelectedStructureType = type;
            mStructureMode = true;
        });
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
        else
        {
            if (!mBuildMode)
                mBuildingManager.ClearPreview();
        }

        mDecorationManager.Draw(mShader);
        if (mDecorationMode && mSelection.HasHover())
        {
            auto coord = mSelection.GetHovered().value();
            mDecorationManager.SetPreview(mSelectedDecorationType, coord);
            mDecorationManager.DrawPreview(mHighlightShader, mMap, mTime);
        }

        mStructureManager.Draw(mShader);
        if (mStructureMode && mSelection.HasHover())
        {
            auto coord = mSelection.GetHovered().value();
            mStructureManager.SetPreview(mSelectedStructureType, coord, 0.0f);
            mStructureManager.DrawPreview(mHighlightShader, mMap, mTime);
        }
        else
        {
            if (!mStructureMode)
                mStructureManager.ClearPreview();
        }

        mUnitManager.Update(ts);
        mUnitManager.Draw(mShader);
        if (mUnitManager.HasSelection())
        {
            auto* selected_unit = mUnitManager.GetSelectedUnit();
            if (selected_unit)
            {
                highlighted_tiles.insert(selected_unit->GetCoordinate());
            }
        }

        Renderer::EndScene();
        Renderer::EndFrame();

        auto& debug_panel = mUIManager.GetDebugPanel();
        debug_panel.SetCamera(&mCameraController->GetCamera());
        debug_panel.SetTileCount(mMap.GetTileCount());
        debug_panel.SetBuildingCount(mBuildingManager.GetBuildingCount());
        debug_panel.SetDecorationCount(mDecorationManager.GetDecorationCount());
        debug_panel.SetStructureCount(mStructureManager.GetStructureCount());

        if (mSelection.HasSelection())
        {
            auto coord = mSelection.GetSelected().value();
            auto* tile = mMap.GetTile(coord);
            auto* building = mBuildingManager.GetBuilding(coord);

            mUIManager.GetTileInfoPanel().SetSelectedTile(tile);
            mUIManager.GetTileInfoPanel().SetSelectedBuilding(building);
        }
        else
        {
            mUIManager.GetTileInfoPanel().Clear();
        }
    }

    void GameLayer::OnImGuiRender()
    {
        mUIManager.OnRender();
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

            if (mDecorationMode && mSelection.HasHover())
            {
                auto coord = mSelection.GetHovered().value();
                if (mDecorationManager.PlaceDecoration(mSelectedDecorationType, coord))
                {
                    RF_CORE_INFO("Decoration placed");
                }
                return true;
            }

            if (mStructureMode && mSelection.HasHover())
            {
                auto coord = mSelection.GetHovered().value();
                if (mStructureManager.PlaceStructure(mSelectedStructureType, coord, 0.0f, mMap))
                {
                    RF_CORE_INFO("Structure placed at ({}, {})", coord.Q, coord.R);
                }
                return true;
            }

            if (mUnitSpawnMode && mSelection.HasHover())
            {
                auto coord = mSelection.GetHovered().value();
                mUnitManager.SpawnUnit(mSelectedUnitType, mPlayerFaction, coord);
                RF_CORE_INFO("Unit spawned");
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

        if (event.GetMouseButton() == Mouse::ButtonRight)
        {
            if (mUnitManager.HasSelection() && mSelection.HasHover())
            {
                auto* unit = mUnitManager.GetSelectedUnit();
                auto target = mSelection.GetHovered().value();
                if (unit)
                {
                    unit->MoveTo(target);
                    RF_CORE_INFO("Unit moving to ({}, {})", target.Q, target.R);
                }
                return true;
            }
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
            if (mBuildMode && mStructureMode)
            {
                mStructureMode = false;
                mStructureManager.ClearPreview();
            }
            RF_CORE_INFO("Build mode: {}", mBuildMode ? "ON" : "OFF");
            return true;
        }

        if (event.GetKeyCode() == Key::D)
        {
            mDecorationMode = !mDecorationMode;
            if (!mDecorationMode)
                mDecorationManager.ClearPreview();
            if (mDecorationMode)
            {
                mDecorationMode = false;
                mDecorationManager.ClearPreview();
            }
            RF_CORE_INFO("Decoration mode: {}", mDecorationMode ? "ON" : "OFF");
            return true;
        }

        if (event.GetKeyCode() == Key::V)
        {
            mStructureMode = !mStructureMode;
            if (!mStructureMode)
                mStructureManager.ClearPreview();
            if (mStructureMode && mBuildMode)
            {
                mBuildMode = false;
                mBuildingManager.ClearPreview();
            }
            RF_CORE_INFO("Structure mode: {}", mStructureMode ? "ON" : "OFF");
            return true;
        }

        if (mBuildMode)
        {
            if (event.GetKeyCode() == Key::D1) mSelectedBuildingType = BuildingType::TownHall;
            else if (event.GetKeyCode() == Key::D2) mSelectedBuildingType = BuildingType::HomeA;
            else if (event.GetKeyCode() == Key::D3) mSelectedBuildingType = BuildingType::Barracks;
        }
        else if (mDecorationMode)
        {
            if (event.GetKeyCode() == Key::D1) mSelectedDecorationType = DecorationType::TreeSmall;
            else if (event.GetKeyCode() == Key::D2) mSelectedDecorationType = DecorationType::TreeMedium;
            else if (event.GetKeyCode() == Key::D3) mSelectedDecorationType = DecorationType::TreeLarge;
            else if (event.GetKeyCode() == Key::D4) mSelectedDecorationType = DecorationType::RockA;
            else if (event.GetKeyCode() == Key::D5) mSelectedDecorationType = DecorationType::Barrel;
            else if (event.GetKeyCode() == Key::D6) mSelectedDecorationType = DecorationType::Crate;
        }
        else if (mStructureMode)
        {
            if (event.GetKeyCode() == Key::D1) mSelectedStructureType = StructureType::WallStraight;
            else if (event.GetKeyCode() == Key::D2) mSelectedStructureType = StructureType::FenceWoodStraight;
            else if (event.GetKeyCode() == Key::D3) mSelectedStructureType = StructureType::BridgeA;
        }

        if (event.GetKeyCode() == Key::F1) mUIManager.ToggleResourcesPanel();
        if (event.GetKeyCode() == Key::F2) mUIManager.ToggleBuildPanel();
        if (event.GetKeyCode() == Key::F3) mUIManager.ToggleTileInfoPanel();
        if (event.GetKeyCode() == Key::F4) mUIManager.ToggleDebugPanel();

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
