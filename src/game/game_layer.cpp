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
#include "game/building/mine.h"
#include <imgui.h>

#include "renderer/model_cache.h"

namespace RealmFortress
{
    GameLayer::GameLayer()
        : Layer("GameLayer")
    {
        mShader = mShaderLibrary.Load("assets/shaders/basic.glsl");
        mHighlightShader = mShaderLibrary.Load("assets/shaders/highlight.glsl");

        auto& window = Application::Get().GetWindow();
        f32 aspect_ratio = static_cast<f32>(window.GetWidth()) / static_cast<f32>(window.GetHeight());
        mCameraController = CreateRef<CameraController>(aspect_ratio);
    }

    void GameLayer::OnAttach()
    {
        RF_CORE_INFO("GameLayer attached");

        Renderer::SetClearColor(glm::vec4(0.53f, 0.81f, 0.92f, 1.0f));

        mCameraController->GetCamera().SetPosition(glm::vec3(0.0f, 15.0f, 15.0f));
        mCameraController->GetCamera().SetRotation(glm::vec3(-45.0f, 0.0f, 0.0f));

        mMap.GenerateHexagon(5, TileType::Grass);

        bool success = Warehouse::Get().Add({
            { ResourceType::Lumber, 100 },
            { ResourceType::Stone, 80 },
            { ResourceType::Wheat, 50 }
        });

        if (!success)
        {
            RF_CORE_WARN("Initial resources exceed warehouse capacity!");
        }

        RF_CORE_INFO("Map generated with {} tiles", mMap.GetTileCount());
        RF_CORE_INFO("Warehouse capacity: {} / {}",
                     Warehouse::Get().GetUsedSpace(),
                     Warehouse::Get().GetCapacity());
    }

    void GameLayer::OnDetach()
    {
        RF_CORE_INFO("GameLayer detached");
        BuildingManager::Get().Clear();
    }

    void GameLayer::OnUpdate(Timestep ts)
    {
        mTime += ts;

        Warehouse::Get().OnUpdate(ts);
        BuildingManager::Get().OnUpdate(ts);

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
        glm::vec3 highlight_color(1.0f, 1.0f, 0.0f);

        if (mGameMode == GameMode::Building)
        {
            if (mSelection.HasHover())
            {
                auto hovered = mSelection.GetHovered().value();
                bool can_place = BuildingManager::Get().CanPlaceBuilding(
                    mSelectedBuildingType, hovered, mMap);

                highlighted_tiles.insert(hovered);
                highlight_color = can_place ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            if (mSelection.HasHover())
            {
                highlighted_tiles.insert(mSelection.GetHovered().value());
                highlight_color = glm::vec3(1.0f, 1.0f, 0.0f);
            }
            if (mSelection.HasSelection())
            {
                highlighted_tiles.insert(mSelection.GetSelected().value());
                highlight_color = glm::vec3(0.0f, 1.0f, 0.5f);
            }
        }

        mMap.DrawWithHighlight(mShader, mHighlightShader, highlighted_tiles, highlight_color, mTime);

        mShader->Bind();
        for (const auto& building : BuildingManager::Get().GetAllBuildings())
        {
            if (building && building->GetModel())
            {
                building->GetModel()->Draw(mShader, building->GetTransform());
            }
        }

        if (mInspectedBuilding && mInspectedBuilding->GetModel())
        {
            mHighlightShader->Bind();
            mHighlightShader->SetMat4("uViewProjection", Renderer::GetViewProjectionMatrix());
            mHighlightShader->SetFloat3("uHighlightColor", glm::vec3(1.0f, 1.0f, 0.0f));
            mHighlightShader->SetFloat("uPulseTime", mTime);
            mHighlightShader->SetFloat("uHighlightIntensity", 0.5f);

            glm::mat4 transform = mInspectedBuilding->GetTransform();
            transform = glm::scale(transform, glm::vec3(1.05f));

            mInspectedBuilding->GetModel()->Draw(mHighlightShader, transform);
        }

        if (mGameMode == GameMode::Building)
        {
            DrawGhostBuilding();
        }

        Renderer::EndScene();
        Renderer::EndFrame();
    }

    void GameLayer::OnImGuiRender()
    {
        DrawResourcePanel();
        DrawBuildingMenu();

        if (mInspectedBuilding)
        {
            DrawBuildingInfo();
        }
        else if (mSelection.HasSelection())
        {
            DrawSelectionInfo();
        }

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Mode: %s", mGameMode == GameMode::Building ? "Building" : "Normal");
        ImGui::Text("Buildings: %zu", BuildingManager::Get().GetAllBuildings().size());

        if (ImGui::Button("Clear All Buildings"))
        {
            BuildingManager::Get().Clear();
        }

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
            if (mGameMode == GameMode::Building)
            {
                PlaceBuildingAtSelection();
            }
            else
            {
                if (mSelection.HasHover())
                {
                    auto coord = mSelection.GetHovered().value();

                    Building* building = BuildingManager::Get().GetBuildingAt(coord);
                    if (building)
                    {
                        mInspectedBuilding = building;
                        RF_CORE_INFO("Inspecting building: {} at ({}, {})",
                                    building->GetDefinition().Name,
                                    coord.Q, coord.R);
                    }
                    else
                    {
                        mSelection.Select(coord);
                        mInspectedBuilding = nullptr;
                        RF_CORE_INFO("Selected tile: ({}, {})", coord.Q, coord.R);
                    }
                }
            }
            return true;
        }

        if (event.GetMouseButton() == Mouse::ButtonRight)
        {
            if (mGameMode == GameMode::Building)
            {
                ExitBuildMode();
            }
            else
            {
                mSelection.ClearSelection();
                mInspectedBuilding = nullptr;
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
            if (mGameMode == GameMode::Building)
            {
                ExitBuildMode();
            }
            else
            {
                mSelection.ClearSelection();
                mInspectedBuilding = nullptr;
            }
            return true;
        }

        if (event.GetKeyCode() == Key::F11)
        {
            Application::Get().GetWindow().ToggleFullscreen();
            return true;
        }

        if (event.GetKeyCode() == Key::D1)
        {
            EnterBuildMode(BuildingType::LumberMill);
            return true;
        }
        if (event.GetKeyCode() == Key::D2)
        {
            EnterBuildMode(BuildingType::Mine);
            return true;
        }
        if (event.GetKeyCode() == Key::D3)
        {
            EnterBuildMode(BuildingType::Farm);
            return true;
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

    void GameLayer::EnterBuildMode(BuildingType type)
    {
        mGameMode = GameMode::Building;
        mSelectedBuildingType = type;
        mInspectedBuilding = nullptr;

        const auto& definition = GetBuildingDefinition(type);
        RF_CORE_INFO("Entering build mode: {}", definition.Name);
    }

    void GameLayer::ExitBuildMode()
    {
        mGameMode = GameMode::Normal;
        RF_CORE_INFO("Exiting build mode");
    }

    void GameLayer::PlaceBuildingAtSelection()
    {
        if (!mSelection.HasHover())
            return;

        auto coord = mSelection.GetHovered().value();

        if (BuildingManager::Get().PlaceBuilding(mSelectedBuildingType, coord, mMap))
        {
            const auto& definition = GetBuildingDefinition(mSelectedBuildingType);
            RF_CORE_INFO("Placed {} at ({}, {})", definition.Name, coord.Q, coord.R);

            ExitBuildMode();
        }
        else
        {
            RF_CORE_WARN("Cannot place building at ({}, {})", coord.Q, coord.R);
        }
    }

    void GameLayer::DrawGhostBuilding()
    {
        if (!mSelection.HasHover())
            return;

        auto coord = mSelection.GetHovered().value();
        bool can_place = BuildingManager::Get().CanPlaceBuilding(mSelectedBuildingType, coord, mMap);

        const auto& definition = GetBuildingDefinition(mSelectedBuildingType);
        auto model = ModelCache::Get(definition.ModelPath);

        if (!model)
        {
            model = ModelCache::Load(definition.ModelPath);
            if (!model)
            {
                model = ModelCache::Get("assets/objects/buildings/blue/building_mine_blue.gltf");
            }
        }

        if (model)
        {
            mHighlightShader->Bind();
            mHighlightShader->SetMat4("uViewProjection", Renderer::GetViewProjectionMatrix());

            glm::vec3 color = can_place ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
            mHighlightShader->SetFloat3("uHighlightColor", color);
            mHighlightShader->SetFloat("uPulseTime", mTime);
            mHighlightShader->SetFloat("uHighlightIntensity", 0.3f);

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, coord.ToWorldPosition());

            model->Draw(mHighlightShader, transform);
        }
    }

    std::unordered_set<Coordinate> GameLayer::GetBuildableTiles() const
    {
        std::unordered_set<Coordinate> buildable;

        for (const auto& coord : mMap.GetTiles() | std::views::keys)
        {
            if (BuildingManager::Get().CanPlaceBuilding(mSelectedBuildingType, coord, mMap))
            {
                buildable.insert(coord);
            }
        }

        return buildable;
    }

    void GameLayer::DrawResourcePanel()
    {
        ImGui::Begin("Resources");

        auto& warehouse = Warehouse::Get();

        ImGui::Text("Lumber: %d", warehouse.GetAmount(ResourceType::Lumber));
        ImGui::Text("Stone: %d", warehouse.GetAmount(ResourceType::Stone));
        ImGui::Text("Food: %d", warehouse.GetAmount(ResourceType::Wheat));

        ImGui::Separator();

        i32 used = warehouse.GetUsedSpace();
        i32 capacity = warehouse.GetCapacity();
        f32 percent = warehouse.GetUsagePercent();

        ImVec4 color = percent < 0.7f ? ImVec4(0, 1, 0, 1) :
                       percent < 0.9f ? ImVec4(1, 1, 0, 1) :
                                        ImVec4(1, 0, 0, 1);

        ImGui::TextColored(color, "Storage: %d / %d (%.1f%%)", used, capacity, percent * 100.0f);
        ImGui::ProgressBar(percent);

        if (percent > 0.9f)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING: Warehouse almost full!");
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Debug Controls"))
        {
            if (ImGui::Button("Add Resources"))
            {
                warehouse.Add({{ResourceType::Lumber, 50}});
                warehouse.Add({{ResourceType::Stone, 30}});
                warehouse.Add({{ResourceType::Wheat, 20}});
            }

            if (ImGui::Button("Clear Resources"))
            {
                warehouse.Consume({{ResourceType::Lumber, warehouse.GetAmount(ResourceType::Lumber)}});
                warehouse.Consume({{ResourceType::Stone, warehouse.GetAmount(ResourceType::Stone)}});
                warehouse.Consume({{ResourceType::Wheat, warehouse.GetAmount(ResourceType::Wheat)}});
            }
        }

        ImGui::End();
    }

    void GameLayer::DrawBuildingMenu()
    {
        ImGui::Begin("Buildings");

        if (mGameMode == GameMode::Building)
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "BUILD MODE ACTIVE");
            ImGui::Text("Selected: %s", GetBuildingDefinition(mSelectedBuildingType).Name);
            ImGui::Text("Right-click or ESC to cancel");
            ImGui::Separator();
        }

        // Group by category
        for (u8 c = 0; c < static_cast<u8>(BuildingCategory::Count); ++c)
        {
            BuildingCategory category = static_cast<BuildingCategory>(c);

            if (ImGui::CollapsingHeader(BuildingCategoryToString(category), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                for (u8 i = 0; i < static_cast<u8>(BuildingType::Count); ++i)
                {
                    BuildingType type = static_cast<BuildingType>(i);
                    const auto& definition = GetBuildingDefinition(type);

                    if (definition.Category != category)
                        continue;

                    // Button with hotkey
                    char label[64];
                    snprintf(label, sizeof(label), "%s [%d]", definition.Name, static_cast<int>(type) + 1);

                    if (ImGui::Button(label, ImVec2(200, 0)))
                    {
                        EnterBuildMode(type);
                    }

                    ImGui::SameLine();
                    ImGui::TextDisabled("- %s", definition.Description);

                    // Cost display
                    ImGui::Indent();
                    ImGui::Text("Cost:");
                    for (const auto& [resType, amount] : definition.ConstructionCost)
                    {
                        bool has_enough = Warehouse::Get().GetAmount(resType) >= amount;
                        ImGui::TextColored(
                            has_enough ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1),
                            "  %s: %d",
                            ResourceTypeToString(resType),
                            amount
                        );
                    }
                    ImGui::Unindent();

                    // Building count
                    i32 count = BuildingManager::Get().GetBuildingCount(type);
                    if (count > 0)
                    {
                        ImGui::Text("Built: %d", count);
                    }

                    ImGui::Separator();
                }

                ImGui::Unindent();
            }
        }

        ImGui::End();
    }

    void GameLayer::DrawBuildingInfo()
    {
        if (!mInspectedBuilding)
            return;

        ImGui::Begin("Building Info");

        const auto& definition = mInspectedBuilding->GetDefinition();
        const Coordinate& coord = mInspectedBuilding->GetCoordinate();

        ImGui::Text("Name: %s", definition.Name);
        ImGui::Text("Category: %s", BuildingCategoryToString(definition.Category));
        ImGui::Text("Location: (%d, %d)", coord.Q, coord.R);
        ImGui::Text("Active: %s", mInspectedBuilding->IsActive() ? "Yes" : "No");

        ImGui::Separator();

        switch (mInspectedBuilding->GetType())
        {
        case BuildingType::Mine:
        {
            auto* mine = static_cast<Mine*>(mInspectedBuilding);
            ImGui::Text("Production: Stone");
            /*
            ImGui::Text("Progress:");
            ImGui::ProgressBar(mine->GetProductionProgress());
            ImGui::Text("Buffer: %d / %d",
                       mine->GetBufferAmount(),
                       mine->GetBufferCapacity());

            if (mine->GetBufferAmount() > 0 && ImGui::Button("Flush Buffer"))
            {
                i32 flushed = mine->FlushBuffer();
                RF_CORE_INFO("Flushed {} stone from mine", flushed);
            }
            */
            break;
        }
        default: break;
        }


        ImGui::Separator();

        // Controls
        if (ImGui::Button(mInspectedBuilding->IsActive() ? "Deactivate" : "Activate"))
        {
            mInspectedBuilding->SetActive(!mInspectedBuilding->IsActive());
        }

        ImGui::SameLine();

        if (ImGui::Button("Destroy"))
        {
            BuildingManager::Get().RemoveBuilding(coord);
            mInspectedBuilding = nullptr;
        }

        ImGui::End();
    }

    void GameLayer::DrawSelectionInfo()
    {
        if (!mSelection.HasSelection())
            return;

        ImGui::Begin("Tile Info");

        auto coord = mSelection.GetSelected().value();
        const Tile* tile = mMap.GetTile(coord);

        if (tile)
        {
            ImGui::Text("Coordinate: (%d, %d)", coord.Q, coord.R);
            ImGui::Text("Type: %s", TileTypeToString(tile->GetType()));
            ImGui::Text("Elevation: %d", tile->GetElevation());

            // Check if the building can be placed here
            ImGui::Separator();
            ImGui::Text("Building Placement:");

            for (u8 i = 0; i < static_cast<u8>(BuildingType::Count); ++i)
            {
                BuildingType type = static_cast<BuildingType>(i);
                bool can_place = BuildingManager::Get().CanPlaceBuilding(type, coord, mMap);

                ImGui::TextColored(
                    can_place ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1),
                    "%s: %s",
                    GetBuildingDefinition(type).Name,
                    can_place ? "Yes" : "No"
                );
            }
        }

        ImGui::End();
    }
} // namespace RealmFortress
