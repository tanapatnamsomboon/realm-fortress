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
#include "renderer/model_cache.h"
#include "game/building/mine.h"
#include "game/thumbnail_generator.h"
#include <imgui.h>

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

        ThumbnailGenerator::Init(512);

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

        SetupTheme();
    }

    void GameLayer::OnDetach()
    {
        RF_CORE_INFO("GameLayer detached");
        BuildingManager::Get().Clear();
        ThumbnailGenerator::Shutdown();
    }

    void GameLayer::OnUpdate(Timestep ts)
    {
        mTime += ts;

        Warehouse::Get().OnUpdate(ts);
        BuildingManager::Get().OnUpdate(ts);

        Renderer::BeginFrame();

        Renderer::SetClearColor(glm::vec4(0.53f, 0.81f, 0.92f, 1.0f));
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
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoMove ;

        ImGui::Begin("MainWindow", nullptr, flags);

        if (ImGui::BeginTable("MainLayout", 3))
        {
            ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Center", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextColumn();
            DrawLeft();

            ImGui::TableNextColumn();
            DrawCenter();

            ImGui::TableNextColumn();
            ImGui::BeginChild("RightWindow", ImVec2(0, 0), 0, ImGuiWindowFlags_NoBackground);
            ImGui::EndChild();

            ImGui::EndTable();
        }

        DrawTimeHUD();

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

    void GameLayer::SetupTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowRounding    = 6.0f;
        style.ChildRounding     = 5.0f;
        style.FrameRounding     = 3.0f;
        style.PopupRounding     = 5.0f;
        style.GrabRounding      = 3.0f;
        style.TabRounding       = 3.0f;

        style.WindowBorderSize  = 0.0f;
        style.FrameBorderSize   = 0.0f;
        style.PopupBorderSize   = 0.0f;
        style.ImageBorderSize   = 0.0f;

        style.WindowPadding     = ImVec2(15, 15);
        style.ItemSpacing       = ImVec2(10, 10);
        style.FramePadding      = ImVec2(5, 5);
    }

    void GameLayer::DrawLeft()
    {
        ImGui::BeginChild("LeftWindow", ImVec2(0, 0), 0, ImGuiWindowFlags_NoBackground);
        if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelBuilding))
        {
            DrawBuildingPanel();

            if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelBuildConfirm))
            {
                DrawBuildConfirmPanel();
            }

        }
        ImGui::EndChild();
    }

    void GameLayer::DrawCenter()
    {
        ImGui::BeginChild("CenterWindow", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoBackground);
        DrawActionBar();
        ImGui::EndChild();
    }

    void GameLayer::DrawTimeHUD()
    {
    }

    void GameLayer::DrawActionBar()
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImGui::GetWindowPos(); // parent position
        ImVec2 at(pos.x, pos.y + avail.y - mUIData.ActionBarButtonSize);
        ImGui::SetNextWindowPos(at);

        ImGuiChildFlags child_flags = ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize;

        ImGui::BeginChild("ActionBar", ImVec2(0.0f, 0.0f), child_flags, ImGuiWindowFlags_NoBackground);

        if (ImGui::Button("Building", ImVec2(mUIData.ActionBarButtonSize, mUIData.ActionBarButtonSize)))
        {
            UI_PANEL_TOGGLE(mUIPanelFlags, UIPanelBuilding);
            if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelBuilding)) UI_PANEL_OPEN_ONLY(mUIPanelFlags, UIPanelBuilding);
        }

        ImGui::SameLine();

        if (ImGui::Button("Economy", ImVec2(mUIData.ActionBarButtonSize, mUIData.ActionBarButtonSize)))
        {
            UI_PANEL_TOGGLE(mUIPanelFlags, UIPanelEconomy);
            if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelEconomy)) UI_PANEL_OPEN_ONLY(mUIPanelFlags, UIPanelEconomy);
        }

        ImGui::EndChild();
    }

    void GameLayer::DrawBuildingPanel()
    {
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImVec2 at(pos.x, pos.y + size.y - mUIData.BuildingPanelHeight);

        ImGui::SetNextWindowPos(at);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(51, 50, 33, 255));
        ImGui::BeginChild("BuildingPanel", ImVec2(size.x, mUIData.BuildingPanelHeight), ImGuiChildFlags_AlwaysUseWindowPadding);
        {
            f32 button_size = mUIData.BuildingPanelThumbnailSize;
            f32 spacing = ImGui::GetStyle().ItemSpacing.x;
            f32 avail_width = ImGui::GetContentRegionAvail().x;
            int columns = static_cast<int>((avail_width + spacing) / (button_size + spacing));
            columns = std::max(3, columns);

            if (ImGui::BeginTable("BuildGrid", columns, ImGuiTableFlags_SizingStretchSame))
            {
                i32 cell_index = 0;

                for (u8 i = 0; i < static_cast<u8>(BuildingType::Count); ++i)
                {
                    BuildingType type = static_cast<BuildingType>(i);
                    const auto& definition = GetBuildingDefinition(type);

                    if (cell_index % columns == 0)
                        ImGui::TableNextRow();

                    ImGui::TableNextColumn();

                    u32 thumbnail_id = ThumbnailGenerator::GetThumbnail(definition.ModelPath);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::ImageButton(
                        definition.Name,
                        thumbnail_id,
                        ImVec2(button_size, button_size),
                        ImVec2(0, 1), ImVec2(1, 0),
                        ImGui::ColorConvertU32ToFloat4(IM_COL32(125, 127, 102, 255))
                        ))
                    {
                        if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelBuildConfirm) && mSelectedBuildingToConfirm == type)
                        {
                            UI_PANEL_CLOSE(mUIPanelFlags, UIPanelBuildConfirm);
                        }
                        else
                        {
                            UI_PANEL_OPEN(mUIPanelFlags, UIPanelBuildConfirm);
                            mSelectedBuildingToConfirm = type;
                        }
                    }
                    ImGui::PopStyleVar();

                    f32 name_length = ImGui::CalcTextSize(definition.Name).x;
                    f32 offset = ((button_size - name_length) / 2.0f);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
                    ImGui::Text("%s", definition.Name);

                    cell_index++;
                }

                ImGui::EndTable(); // BuildGrid
            }
        }
        ImGui::EndChild(); // BuildPanel
        ImGui::PopStyleColor();
    }

    void GameLayer::DrawBuildConfirmPanel()
    {
        if (!mSelectedBuildingToConfirm.has_value()) return;

        ImVec2 size = ImGui::GetWindowSize();
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 at(pos.x, pos.y + size.y - mUIData.BuildingPanelHeight - mUIData.BuildConfirmPanelHeight - ImGui::GetStyle().WindowPadding.y);

        ImGui::SetNextWindowPos(at);

        ImGui::BeginChild("BuildConfirmPanel", ImVec2(mUIData.BuildConfirmPanelWidth, mUIData.BuildConfirmPanelHeight), ImGuiChildFlags_AlwaysUseWindowPadding);
        BuildingDefinition definition = GetBuildingDefinition(mSelectedBuildingType);
        ImGui::Text(definition.Name);
        ImGui::Button("BUILD", ImVec2(50.0f, 50.0f));
        ImGui::EndChild();
    }

    void GameLayer::DrawEconomyPanel()
    {
    }
} // namespace RealmFortress
