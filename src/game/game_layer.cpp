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
    #define IMGUI_DEFINE_MATH_OPERATORS
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
            ImVec2 action_bar_pos, action_bar_size;
            ImVec2 building_panel_pos, building_panel_size;

            DrawTimeHUD();
            DrawActionBar(&action_bar_pos, &action_bar_size);
            if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelBuilding))
            {
                DrawBuildingPanel(action_bar_pos, action_bar_size, &building_panel_pos, &building_panel_size);

                if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelBuildConfirm))
                {
                    DrawBuildConfirmPanel(building_panel_pos, building_panel_size);
                }
            }
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

        // UI constant
        constexpr f32 ActionBar_ButtonSize{ 110.0f };

        constexpr f32 BuildingPanel_Height{ 285.0f };
        constexpr f32 BuildingPanel_ThumbnailSize{ 80.0f };

        constexpr f32 BuildConfirmPanel_Height{ 250.0f };

        void GameLayer::DrawTimeHUD()
        {
        }

        void GameLayer::DrawActionBar(ImVec2* out_pos, ImVec2* out_size)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 pos(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y);
            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav
                                   | ImGuiWindowFlags_NoTitleBar
                                   | ImGuiWindowFlags_NoResize
                                   | ImGuiWindowFlags_NoMove
                                   | ImGuiWindowFlags_NoSavedSettings
                                   | ImGuiWindowFlags_NoBackground
                                   | ImGuiWindowFlags_NoDecoration
                                   | ImGuiWindowFlags_NoBringToFrontOnFocus;

            ImGui::Begin("ActionBar", nullptr, flags);
            {
                *out_pos = ImGui::GetWindowPos();
                *out_size = ImGui::GetWindowSize();

                if (ImGui::Button("Building", ImVec2(ActionBar_ButtonSize, ActionBar_ButtonSize)))
                {
                    UI_PANEL_TOGGLE(mUIPanelFlags, UIPanelBuilding);
                    if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelBuilding)) UI_PANEL_OPEN_ONLY(mUIPanelFlags, UIPanelBuilding);
                }

                ImGui::SameLine();

                if (ImGui::Button("Economy", ImVec2(ActionBar_ButtonSize, ActionBar_ButtonSize)))
                {
                    UI_PANEL_TOGGLE(mUIPanelFlags, UIPanelEconomy);
                    if (UI_PANEL_IS_OPEN(mUIPanelFlags, UIPanelEconomy)) UI_PANEL_OPEN_ONLY(mUIPanelFlags, UIPanelEconomy);
                }
            }
            ImGui::End();
        }

        void GameLayer::DrawBuildingPanel(ImVec2 action_bar_pos, ImVec2 action_bar_size, ImVec2* out_pos, ImVec2* out_size)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImVec2 pos(action_bar_pos.x, action_bar_pos.y + action_bar_size.y);
            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));

            ImVec2 size(action_bar_pos.x - viewport->Pos.x, BuildingPanel_Height);
            ImGui::SetNextWindowSize(size, ImGuiCond_Always);

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoNav
                                          | ImGuiWindowFlags_NoTitleBar
                                          | ImGuiWindowFlags_NoResize
                                          | ImGuiWindowFlags_NoMove
                                          | ImGuiWindowFlags_NoSavedSettings
                                          | ImGuiWindowFlags_NoBackground
                                          | ImGuiWindowFlags_NoDecoration
                                          | ImGuiWindowFlags_NoBringToFrontOnFocus;

            ImGui::Begin("BuildingPanel", nullptr, window_flags);
            {
                *out_pos = ImGui::GetWindowPos();
                *out_size = ImGui::GetWindowSize();

                ImGuiChildFlags child_flags = ImGuiChildFlags_AlwaysUseWindowPadding;

                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(51, 50, 33, 255));
                ImGui::BeginChild("BuildingPanelBG", ImVec2(0.0f, 0.0f), child_flags);
                {
                    f32 button_size = BuildingPanel_ThumbnailSize;
                    f32 spacing = ImGui::GetStyle().ItemSpacing.x;
                    f32 avail_width = ImGui::GetContentRegionAvail().x;
                    int columns = static_cast<int>((avail_width + spacing) / (button_size + spacing));
                    columns = std::max(3, columns);

                    ImGuiTableFlags table_flags = ImGuiTableFlags_SizingStretchSame;

                    if (ImGui::BeginTable("BuildGrid", columns, table_flags))
                    {
                        i32 cell_index = 0;

                        for (u8 i = 0; i < static_cast<u8>(BuildingType::Count); ++i)
                        {
                            BuildingType type = static_cast<BuildingType>(i);
                            const auto& definition = GetBuildingDefinition(type);

                            if (cell_index % columns == 0)
                                ImGui::TableNextRow();

                            ImGui::TableNextColumn();

                            // Thumbnail
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

                            // Label
                            f32 name_length = ImGui::CalcTextSize(definition.Name).x;
                            f32 offset = ((button_size - name_length) / 2.0f);
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
                            ImGui::Text("%s", definition.Name);

                            cell_index++;
                        }

                        ImGui::EndTable(); // BuildGrid
                    }
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();
            }
            ImGui::End(); // BuildPanel
        }

        // =========== UI Design idea ===========
        // ┌────────────────────────────────────┐
        // │ ┌───────────┐                      │
        // │ │           │  Name                │
        // │ │ Thumbnail │  Description         │
        // │ │           │                      │
        // │ └───────────┘                      │
        // │                                    │
        // │ ┌────┐ ┌────┐          ┌───────┐   │
        // │ │ R1 │ │ R2 │          │ BUILD │   │
        // │ └────┘ └────┘          │  BTN  │   │
        // │   L1     L2            └───────┘   │
        // └────────────────────────────────────┘
        void GameLayer::DrawBuildConfirmPanel(ImVec2 building_panel_pos, ImVec2 building_panel_size)
        {
            if (!mSelectedBuildingToConfirm.has_value()) return;

            ImVec2 pos(building_panel_pos.x, building_panel_pos.y);
            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.0f, 1.0f));

            ImVec2 size(building_panel_size.x * 0.65f, BuildConfirmPanel_Height);
            ImGui::SetNextWindowSize(size, ImGuiCond_Always);

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoNav
                                          | ImGuiWindowFlags_NoTitleBar
                                          | ImGuiWindowFlags_NoResize
                                          | ImGuiWindowFlags_NoMove
                                          | ImGuiWindowFlags_NoSavedSettings
                                          | ImGuiWindowFlags_NoBackground
                                          | ImGuiWindowFlags_NoDecoration
                                          | ImGuiWindowFlags_NoBringToFrontOnFocus;

            ImGui::Begin("BuildConfirmPanel", nullptr, window_flags);
            {
                ImVec2 build_confirm_panel_size = ImGui::GetWindowSize();
                ImVec2 build_confirm_panel_pos = ImGui::GetWindowPos();

                ImGuiChildFlags child_flags = ImGuiChildFlags_AlwaysUseWindowPadding;

                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(51, 50, 33, 255));
                ImGui::BeginChild("BuildConfirmPanelBG", ImVec2(0.0f, 0.0f), child_flags);
                {
                    ImVec2 confirm_button_size(build_confirm_panel_size.x * 0.15f, build_confirm_panel_size.x * 0.15f);
                    ImVec2 resource_icon_size(build_confirm_panel_size.x * 0.125f, build_confirm_panel_size.x * 0.125f);
                    ImVec2 thumbnail_size(92.0f, 92.0f);

                    const auto& definition = GetBuildingDefinition(mSelectedBuildingToConfirm.value());

                    // 1 Header
                    {
                        // 1.1 Thumbnail
                        u32 thumbnail_id = ThumbnailGenerator::GetThumbnail(definition.ModelPath);
                        ImVec2 cursor = ImGui::GetCursorScreenPos();

                        ImGui::GetWindowDrawList()->AddRectFilled(
                            cursor,
                            ImVec2(cursor.x + thumbnail_size.x, cursor.y + thumbnail_size.y),
                            IM_COL32(125, 127, 102, 255),
                            3.0f
                        );
                        ImGui::Image(thumbnail_id, thumbnail_size, ImVec2(0, 1), ImVec2(1, 0));

                        ImGui::SameLine();

                        // 1.2 Info
                        ImGui::BeginChild("BuildConfirmInfo", ImVec2(0, thumbnail_size.y));
                        {
                            ImGui::SetWindowFontScale(2.0f);
                            ImGui::Text("%s", definition.Name);
                            ImGui::SetWindowFontScale(1.0f);

                            ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
                            ImGui::Text("%s", definition.Description);
                            ImGui::PopTextWrapPos();
                        }
                        ImGui::EndChild();
                    }

                    // 2 Footer
                    if (ImGui::BeginTable("FooterLayout", 2, ImGuiTableFlags_SizingStretchProp))
                    {
                        ImGui::TableSetupColumn("Costs", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Button", ImGuiTableColumnFlags_WidthFixed, confirm_button_size.x);

                        ImGui::TableNextRow();

                        // 2.1 Resource
                        ImGui::TableSetColumnIndex(0);
                        {
                            if (ImGui::BeginTable("CostList", static_cast<int>(ResourceType::Count), ImGuiTableFlags_SizingFixedFit))
                            {
                                auto cost = definition.ConstructionCost;

                                for (auto [resource, amount] : cost)
                                {
                                    if (amount <= 0) continue;

                                    ImGui::TableNextColumn();

                                    ImVec2 cursor = ImGui::GetCursorScreenPos();
                                    ImVec2 p_max(cursor + resource_icon_size);

                                    // Icon
                                    u32 res_tex_id = ThumbnailGenerator::GetThumbnail(ResourceTypeToThumbnailModelPath(resource), 2.5f);
                                    ImGui::GetWindowDrawList()->AddRectFilled(
                                        cursor,
                                        p_max,
                                        IM_COL32(22, 29, 18, 255),
                                        5.0f
                                    );
                                    ImGui::Image(res_tex_id, resource_icon_size, ImVec2(0, 1), ImVec2(1, 0));

                                    // Amount
                                    char amount_str[16];
                                    sprintf(amount_str, "%d", amount);
                                    ImGui::SetWindowFontScale(0.9f);
                                    ImVec2 text_size = ImGui::CalcTextSize(amount_str);
                                    f32 padding = 4.0f;
                                    ImVec2 text_pos = ImVec2(p_max.x - text_size.x - padding, p_max.y - text_size.y - padding);
                                    ImGui::GetWindowDrawList()->AddText(text_pos, IM_COL32(255, 255, 255, 255), amount_str);

                                    // Label
                                    ImGui::SetWindowFontScale(0.8f);
                                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f); // Move up a little bit
                                    f32 name_length = ImGui::CalcTextSize(ResourceTypeToString(resource)).x;
                                    f32 offset = ((resource_icon_size.x - name_length) / 2.0f);
                                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
                                    ImGui::Text("%s", ResourceTypeToString(resource));
                                    ImGui::SetWindowFontScale(1.0f);
                                }

                                ImGui::EndTable();
                            }
                        }

                        // 2.2 Build
                        ImGui::TableSetColumnIndex(1);
                        {
                            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(87, 89, 41, 255));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(107, 109, 51, 255));
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(67, 69, 31, 255));

                            if (ImGui::Button("BUILD", confirm_button_size))
                            {
                                UI_PANEL_CLOSE_ALL(mUIPanelFlags);
                                EnterBuildMode(mSelectedBuildingToConfirm.value());
                            }

                            ImGui::PopStyleColor(3);
                        }

                        ImGui::EndTable();
                    }
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();
            }
            ImGui::End();
        }

        void GameLayer::DrawEconomyPanel()
        {
        }
    } // namespace RealmFortress
