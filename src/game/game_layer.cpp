/**
 * @file game_layer.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "game_layer.h"
#include "core/application.h"
#include "core/input.h"
#include "renderer/renderer.h"
#include "events/mouse_event.h"
#include "events/key_event.h"
#include "game/hex/hex_utils.h"
#include <imgui.h>

namespace RealmFortress
{
    GameLayer::GameLayer()
        : Layer("GameLayer"), mCameraController(1600.0f / 900.0f)
    {
    }

    void GameLayer::OnAttach()
    {
        RF_INFO("GameLayer attached");

        mShader = Shader::Create("assets/shaders/basic.glsl");
        mHighlightShader = Shader::Create("assets/shaders/hex_highlight.glsl");
        mTranslucentShader = Shader::Create("assets/shaders/translucent.glsl");

        mResourceManager = CreateScope<ResourceManager>();

        mHexMap = CreateScope<HexMap>();
        mHexMap->Generate(15, 15);

        mBuildingManager = CreateScope<BuildingManager>();
        mBuildingManager->Initialize();
        mBuildingManager->SetResourceManager(mResourceManager.get());

        mUnitManager = CreateScope<UnitManager>();
        mUnitManager->Initialize();
        mUnitManager->SetResourceManager(mResourceManager.get());

        mSelectionManager = CreateScope<SelectionManager>();

        try
        {
            mHexHighlightModel = CreateRef<Model>("assets/objects/tiles/base/hex_grass.gltf");
        }
        catch (...)
        {
            RF_WARN("Failed to load hex highlight model");
        }

        mUnitManager->SpawnUnit(UnitType::Worker, HexCoordinate(2, 2), mPlayerFaction, *mResourceManager);
        mUnitManager->SpawnUnit(UnitType::Spearman, HexCoordinate(3, 2), mPlayerFaction, *mResourceManager);
    }

    void GameLayer::OnDetach()
    {
        RF_INFO("GameLayer detached");
    }

    void GameLayer::OnUpdate(Timestep ts)
    {
        mTime += ts;
        mPulseIntensity = 0.7f + 0.3f * std::sin(mTime * 3.0f);

        mCameraController.OnUpdate(ts);
        mResourceManager->Update(ts);
        mBuildingManager->Update(ts);
        mUnitManager->Update(ts, *mHexMap);

        UpdateHoveredHex();

        Renderer::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
        Renderer::Clear();

        Renderer::BeginScene(mCameraController.GetCamera());

        mShader->Bind();
        mShader->SetFloat3("uLightPos", glm::vec3(10.0f, 20.0f, 10.0f));
        mShader->SetFloat3("uViewPos", mCameraController.GetCamera().GetPosition());
        mShader->SetFloat3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        mShader->SetFloat3("uTintColor", glm::vec3(1.0f, 1.0f, 1.0f));

        mHexMap->Draw(mShader);
        mBuildingManager->DrawBuildings(mShader);
        mUnitManager->DrawUnits(mShader);

        if (mHoveredHex.has_value())
        {
            DrawHexHighlight(mHoveredHex.value(), glm::vec3(1.0f, 1.0f, 0.5f), 0.05f);
        }

        if (mSelectionManager->HasSelection())
        {
            if (mSelectionManager->GetSelectionType() == SelectionType::Hex)
            {
                DrawHexHighlight(mSelectionManager->GetSelectedHex(), glm::vec3(0.2f, 0.8f, 1.0f), 0.1f);
            }
        }

        if (mSelectionManager->HasSelectedUnits())
        {
            for (const Unit* unit : mSelectionManager->GetSelectedUnits())
            {
                if (unit)
                {
                    glm::vec3 pos = unit->GetPosition().ToWorldPosition();
                    DrawSelectionRing(pos, 0.8f, glm::vec3(0.2f, 1.0f, 0.2f));

                    if (unit->IsMoving())
                    {
                        DrawUnitPath(unit);
                    }
                }
            }
        }

        if (mSelectionManager->IsInPlacementMode() && mHoveredHex.has_value())
        {
            DrawGhostBuilding();
        }

        Renderer::EndScene();
    }

    void GameLayer::OnImGuiRender()
    {
        RenderMainUI();

        if (mShowResourcePanel)
            RenderResourceDisplay();
        if (mShowBuildingMenu)
            RenderBuildingMenu();
        if (mShowUnitPanel)
            RenderUnitPanel();
        if (mShowDebugInfo)
            RenderDebugInfo();
    }

    void GameLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(RF_BIND_EVENT_FN(OnMouseButtonPressed));
        dispatcher.Dispatch<MouseMovedEvent>(RF_BIND_EVENT_FN(OnMouseMoved));
        dispatcher.Dispatch<KeyPressedEvent>(RF_BIND_EVENT_FN(OnKeyPressed));

        mCameraController.OnEvent(event);
    }

    bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
    {
        if (event.GetMouseButton() == Mouse::ButtonLeft)
        {
            HandleLeftClick();
            return true;
        }
        if (event.GetMouseButton() == Mouse::ButtonRight)
        {
            HandleRightClick();
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
            if (mSelectionManager->IsInPlacementMode())
            {
                mSelectionManager->ClearPlacementMode();
                mGhostBuildingModel.reset();
                return true;
            }
            if (mSelectionManager->HasSelection())
            {
                mSelectionManager->ClearSelection();
                return true;
            }
        }

        if (event.GetKeyCode() == Key::F1)
        {
            mShowDebugInfo = !mShowDebugInfo;
            return true;
        }
        if (event.GetKeyCode() == Key::F2)
        {
            mShowBuildingMenu = !mShowBuildingMenu;
            return true;
        }

        return false;
    }

    void GameLayer::RenderMainUI()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Resources", nullptr, &mShowResourcePanel);
                ImGui::MenuItem("Buildings", nullptr, &mShowBuildingMenu);
                ImGui::MenuItem("Units", nullptr, &mShowUnitPanel);
                ImGui::MenuItem("Debug Info", "F1", &mShowDebugInfo);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Game"))
            {
                if (ImGui::MenuItem("Save Game"))
                {
                    RF_INFO("Save game (not implemented)");
                }
                if (ImGui::MenuItem("Load Game"))
                {
                    RF_INFO("Load game (not implemented)");
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit to Menu"))
                {
                    RF_INFO("Exit to menu (not implemented)");
                }
                ImGui::EndMenu();
            }

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 400.0f) * 0.5f);
            ImGui::Text("Realm Fortress - %s", FactionToString(mPlayerFaction));

            ImGui::EndMainMenuBar();
        }
    }

    void GameLayer::RenderBuildingMenu()
    {
       ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
        ImGui::Begin("Buildings", &mShowBuildingMenu);

        if (mSelectionManager->IsInPlacementMode())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::TextWrapped("Placement Mode: %s",
                              BuildingTypeToString(mSelectionManager->GetPlacementType()));
            ImGui::PopStyleColor();

            ImGui::Text("Left click to place");
            ImGui::Text("ESC to cancel");

            if (mHoveredHex.has_value())
            {
                if (mGhostBuildingValid)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                    ImGui::Text("Valid placement");
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                    ImGui::Text("Cannot place here");
                    ImGui::PopStyleColor();
                }
            }

            ImGui::Separator();

            if (ImGui::Button("Cancel (ESC)", ImVec2(-1, 0)))
            {
                mSelectionManager->ClearPlacementMode();
                mGhostBuildingModel.reset();
            }
        }
        else
        {
            ImGui::Text("Select building to place:");
            ImGui::Separator();

            static const char* categories[] = { "Civilian", "Military", "Economy" };
            static int currentCategory = 0;

            ImGui::Combo("Category", &currentCategory, categories, IM_ARRAYSIZE(categories));
            ImGui::Separator();

            for (const auto& def : mBuildingManager->GetAllDefinitions())
            {
                // Filter
                bool showBuilding = true;
                if (currentCategory == 0) // Civilian
                {
                    showBuilding = (def.Type == BuildingType::Home ||
                                   def.Type == BuildingType::Townhall ||
                                   def.Type == BuildingType::Lumbermill ||
                                   def.Type == BuildingType::Mine ||
                                   def.Type == BuildingType::Farm);
                }
                else if (currentCategory == 1) // Military
                {
                    showBuilding = (def.Type == BuildingType::Barracks ||
                                   def.Type == BuildingType::Tower ||
                                   def.Type == BuildingType::Castle);
                }
                else if (currentCategory == 2) // Economy
                {
                    showBuilding = (def.Type == BuildingType::Market ||
                                   def.Type == BuildingType::Windmill);
                }

                if (!showBuilding)
                    continue;

                ImGui::PushID(static_cast<int>(def.Type));

                bool canAfford = mResourceManager->CanAfford(def.Cost.Resources);

                if (!canAfford)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
                }

                if (ImGui::Button(def.Name.c_str(), ImVec2(200, 0)))
                {
                    if (canAfford)
                    {
                        mSelectionManager->SetPlacementMode(def.Type);
                    }
                }

                if (!canAfford)
                {
                    ImGui::PopStyleColor(2);
                }

                // Show cost
                ImGui::SameLine();
                ImGui::BeginGroup();
                for (const auto& cost : def.Cost.Resources)
                {
                    bool hasEnough = mResourceManager->HasResource(cost.Type, cost.Amount);
                    if (!hasEnough)
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

                    ImGui::Text("%d %s", cost.Amount, ResourceTypeToString(cost.Type));

                    if (!hasEnough)
                        ImGui::PopStyleColor();
                }
                ImGui::EndGroup();

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", def.Description.c_str());
                    if (def.Cost.BuildTime > 0.0f)
                    {
                        ImGui::Text("Build time: %.1fs", def.Cost.BuildTime);
                    }
                    if (def.PopulationProvided > 0)
                    {
                        ImGui::Text("Provides: +%d population cap", def.PopulationProvided);
                    }
                    if (!def.Production.Production.empty())
                    {
                        ImGui::Text("Production:");
                        for (const auto& prod : def.Production.Production)
                        {
                            ImGui::BulletText("%d %s per %.1fs",
                                            prod.Amount, ResourceTypeToString(prod.Type),
                                            def.Production.Interval);
                        }
                    }
                    ImGui::EndTooltip();
                }

                ImGui::PopID();
            }

            ImGui::Separator();
            ImGui::Text("Total buildings: %zu", mBuildingManager->GetAllBuildings().size());
        }

        ImGui::End();
    }

    void GameLayer::RenderUnitPanel()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 570), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Units", &mShowUnitPanel);

        if (mSelectionManager->HasSelectedUnits())
        {
            const auto& selectedUnits = mSelectionManager->GetSelectedUnits();

            if (selectedUnits.size() == 1)
            {
                Unit* unit = selectedUnits[0];
                if (unit)
                {
                    ImGui::Text("Selected: %s", unit->GetName().c_str());
                    ImGui::Text("Faction: %s", FactionToString(unit->GetFaction()));
                    ImGui::Text("Position: (%d, %d)", unit->GetPosition().q, unit->GetPosition().r);

                    // Health bar
                    f32 healthPercent = unit->GetHealthPercent();
                    ImGui::ProgressBar(healthPercent, ImVec2(-1, 0),
                                      std::format("{}/{}", unit->GetCurrentHealth(), unit->GetMaxHealth()).c_str());

                    ImGui::Separator();
                    ImGui::Text("Stats:");
                    ImGui::BulletText("Attack: %d", unit->GetDefinition().Stats.Attack);
                    ImGui::BulletText("Defense: %d", unit->GetDefinition().Stats.Defense);
                    ImGui::BulletText("Move Speed: %.1f", unit->GetDefinition().Stats.MoveSpeed);
                    ImGui::BulletText("Range: %d", unit->GetDefinition().Stats.AttackRange);
                    ImGui::BulletText("Vision: %d", unit->GetDefinition().Stats.VisionRange);

                    if (unit->IsMoving())
                    {
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Moving...");
                        ImGui::Text("Path length: %zu", unit->GetPath().size());
                    }
                }
            }
            else
            {
                ImGui::Text("Selected: %zu units", selectedUnits.size());
                ImGui::Separator();

                std::unordered_map<UnitType, int> unitCounts;
                for (const Unit* unit : selectedUnits)
                {
                    if (unit)
                        unitCounts[unit->GetType()]++;
                }

                for (const auto& [type, count] : unitCounts)
                {
                    ImGui::BulletText("%dx %s", count, UnitTypeToString(type));
                }

                ImGui::Separator();
                ImGui::Text("Group commands:");
                if (ImGui::Button("Stop All"))
                {
                    for (Unit* unit : selectedUnits)
                    {
                        if (unit)
                            unit->Stop();
                    }
                }
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No units selected");
            ImGui::Separator();
            ImGui::Text("Available units:");

            auto playerUnits = mUnitManager->GetUnitsOfFaction(mPlayerFaction);
            if (playerUnits.empty())
            {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "  (none)");
            }
            else
            {
                std::unordered_map<UnitType, int> unitCounts;
                for (const Unit* unit : playerUnits)
                {
                    if (unit)
                        unitCounts[unit->GetType()]++;
                }

                for (const auto& [type, count] : unitCounts)
                {
                    ImGui::BulletText("%dx %s", count, UnitTypeToString(type));
                }
            }
        }

        ImGui::Separator();
        ImGui::Text("Total units: %zu", mUnitManager->GetAllUnits().size());

        // Quick spawn buttons (for testing)
        if (ImGui::CollapsingHeader("Debug: Spawn Units"))
        {
            if (mHoveredHex.has_value())
            {
                if (ImGui::Button("Spawn Worker"))
                {
                    mUnitManager->SpawnUnit(UnitType::Worker, mHoveredHex.value(),
                                          mPlayerFaction, *mResourceManager);
                }
                ImGui::SameLine();
                if (ImGui::Button("Spawn Spearman"))
                {
                    mUnitManager->SpawnUnit(UnitType::Spearman, mHoveredHex.value(),
                                          mPlayerFaction, *mResourceManager);
                }
                if (ImGui::Button("Spawn Archer"))
                {
                    mUnitManager->SpawnUnit(UnitType::Archer, mHoveredHex.value(),
                                          mPlayerFaction, *mResourceManager);
                }
                ImGui::SameLine();
                if (ImGui::Button("Spawn Knight"))
                {
                    mUnitManager->SpawnUnit(UnitType::Knight, mHoveredHex.value(),
                                          mPlayerFaction, *mResourceManager);
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Hover over hex to spawn");
            }
        }

        ImGui::End();
    }

    void GameLayer::RenderResourceDisplay()
    {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 270, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin("Resources", &mShowResourcePanel, ImGuiWindowFlags_AlwaysAutoResize);

        // Main resources
        ImGui::Text("Wood:  %d", mResourceManager->GetResourceAmount(ResourceType::Wood));
        ImGui::SameLine(120);
        i32 woodRate = mResourceManager->GetProductionRate(ResourceType::Wood);
        if (woodRate > 0)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(+%d/s)", woodRate);

        ImGui::Text("Stone: %d", mResourceManager->GetResourceAmount(ResourceType::Stone));
        ImGui::SameLine(120);
        i32 stoneRate = mResourceManager->GetProductionRate(ResourceType::Stone);
        if (stoneRate > 0)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(+%d/s)", stoneRate);

        ImGui::Text("Food:  %d", mResourceManager->GetResourceAmount(ResourceType::Food));
        ImGui::SameLine(120);
        i32 foodRate = mResourceManager->GetProductionRate(ResourceType::Food);
        if (foodRate > 0)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(+%d/s)", foodRate);

        ImGui::Text("Gold:  %d", mResourceManager->GetResourceAmount(ResourceType::Gold));
        ImGui::SameLine(120);
        i32 goldRate = mResourceManager->GetProductionRate(ResourceType::Gold);
        if (goldRate > 0)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(+%d/s)", goldRate);

        ImGui::Text("Iron:  %d", mResourceManager->GetResourceAmount(ResourceType::Iron));
        ImGui::SameLine(120);
        i32 ironRate = mResourceManager->GetProductionRate(ResourceType::Iron);
        if (ironRate > 0)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(+%d/s)", ironRate);

        ImGui::Separator();

        // Population
        i32 pop = mResourceManager->GetPopulation();
        i32 popCap = mResourceManager->GetPopulationCap();
        ImGui::Text("Population: %d / %d", pop, popCap);
        ImGui::ProgressBar(popCap > 0 ? static_cast<f32>(pop) / popCap : 0.0f, ImVec2(-1, 0));

        ImGui::Separator();

        if (ImGui::Button("Add Resources (Cheat)", ImVec2(-1, 0)))
        {
            mResourceManager->AddResource(ResourceType::Wood, 100);
            mResourceManager->AddResource(ResourceType::Stone, 100);
            mResourceManager->AddResource(ResourceType::Food, 100);
            mResourceManager->AddResource(ResourceType::Gold, 50);
            mResourceManager->AddResource(ResourceType::Iron, 50);
        }

        ImGui::End();
    }

    void GameLayer::RenderMinimap()
    {
    }

    void GameLayer::RenderDebugInfo()
    {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 300, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Debug Info", &mShowDebugInfo);

        ImGui::Text("FPS: %.1f (%.2fms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

        auto cameraPos = mCameraController.GetCamera().GetPosition();
        ImGui::Text("Camera: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);

        ImGui::Separator();

        if (mHoveredHex.has_value())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Hover: (%d, %d)", mHoveredHex.value().q, mHoveredHex.value().r);

            // Show what's at this hex
            Unit* unit = mUnitManager->GetUnitAt(mHoveredHex.value());
            if (unit)
            {
                ImGui::BulletText("Unit: %s (%s)", unit->GetName().c_str(),
                                FactionToString(unit->GetFaction()));
            }

            Building* building = mBuildingManager->GetBuildingAt(mHoveredHex.value());
            if (building)
            {
                ImGui::BulletText("Building: %s", building->GetName().c_str());
            }
        }

        if (mSelectionManager->HasSelection())
        {
            if (mSelectionManager->GetSelectionType() == SelectionType::Hex)
            {
                auto selected = mSelectionManager->GetSelectedHex();
                ImGui::Text("Selected: (%d, %d)", selected.q, selected.r);
            }
            else if (mSelectionManager->GetSelectionType() == SelectionType::Unit)
            {
                ImGui::Text("Selected: 1 unit");
            }
            else if (mSelectionManager->GetSelectionType() == SelectionType::MultipleUnits)
            {
                ImGui::Text("Selected: %zu units", mSelectionManager->GetSelectedUnits().size());
            }
        }

        ImGui::Separator();

        ImGui::Text("Map tiles: %d", mHexMap->GetTileCount());
        ImGui::Text("Buildings: %zu", mBuildingManager->GetAllBuildings().size());
        ImGui::Text("Units: %zu", mUnitManager->GetAllUnits().size());

        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Controls:");
        ImGui::BulletText("WASD - Move camera");
        ImGui::BulletText("Q/E - Camera up/down");
        ImGui::BulletText("Right Mouse - Rotate");
        ImGui::BulletText("Left Click - Select");
        ImGui::BulletText("Right Click - Order/Move");
        ImGui::BulletText("ESC - Cancel/Clear");
        ImGui::BulletText("F1 - Toggle debug");
        ImGui::BulletText("F2 - Toggle buildings");

        ImGui::End();
    }

    void GameLayer::UpdateHoveredHex()
    {
        auto& window = Application::Get().GetWindow();
        auto mousePos = Input::GetMousePosition();

        mHoveredHex = mSelectionManager->GetHexAtScreenPosition(
            mousePos.x, mousePos.y,
            window.GetWidth(), window.GetHeight(),
            mCameraController.GetCamera(),
            *mHexMap
        );

        if (mSelectionManager->IsInPlacementMode() && mHoveredHex.has_value())
        {
            mGhostBuildingValid = mBuildingManager->CanPlaceBuilding(
                mSelectionManager->GetPlacementType(),
                mHoveredHex.value()
            );

            auto* def = mBuildingManager->GetBuildingDefinition(mSelectionManager->GetPlacementType());
            if (def)
            {
                if (!mResourceManager->CanAfford(def->Cost.Resources))
                {
                    mGhostBuildingValid = false;
                }

                if (def && (!mGhostBuildingModel || mGhostBuildingModel.use_count() == 0))
                {
                    try
                    {
                        std::string modelPath = def->GetModelPath(mPlayerFaction);
                        mGhostBuildingModel = CreateRef<Model>(modelPath);
                    }
                    catch (...)
                    {
                        RF_WARN("Failed to load ghost building model");
                    }
                }
            }
        }
    }

    void GameLayer::DrawHexHighlight(const HexCoordinate& coord, const glm::vec3& color, f32 elevation)
    {
        if (!mHexHighlightModel) return;

        glm::vec3 worldPos = coord.ToWorldPosition();
        worldPos.y = elevation;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);
        transform = glm::scale(transform, glm::vec3(1.05f, 1.0f, 1.05f)); // Slightly larger

        Renderer::SetBlendMode(true);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mHighlightShader->Bind();
        mHighlightShader->SetFloat3("uHighlightColor", color);
        mHighlightShader->SetFloat("uPulseIntensity", mPulseIntensity);

        mHexHighlightModel->Draw(mHighlightShader, transform);

        Renderer::SetBlendMode(false);
    }

    void GameLayer::DrawSelectionRing(const glm::vec3& worldPos, f32 radius, const glm::vec3& color)
    {
        if (mHexHighlightModel)
        {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);
            transform = glm::scale(transform, glm::vec3(radius, 0.5f, radius));

            Renderer::SetBlendMode(true);
            mHighlightShader->Bind();
            mHighlightShader->SetFloat3("uHighlightColor", color);
            mHighlightShader->SetFloat("uPulseIntensity", mPulseIntensity);
            mHexHighlightModel->Draw(mHighlightShader, transform);
            Renderer::SetBlendMode(false);
        }
    }

    void GameLayer::DrawUnitPath(const Unit* unit)
    {
        if (!unit || !mHexHighlightModel) return;

        const auto& path = unit->GetPath();
        if (path.empty()) return;

        Renderer::SetBlendMode(true);
        mHighlightShader->Bind();
        mHighlightShader->SetFloat3("uHighlightColor", glm::vec3(0.5f, 0.5f, 1.0f));
        mHighlightShader->SetFloat("uPulseIntensity", 0.3f);

        for (const auto& coord : path)
        {
            glm::vec3 worldPos = coord.ToWorldPosition();
            worldPos.y = 0.02f;

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);
            transform = glm::scale(transform, glm::vec3(0.5f, 1.0f, 0.5f));

            mHexHighlightModel->Draw(mHighlightShader, transform);
        }

        Renderer::SetBlendMode(false);
    }

    void GameLayer::DrawGhostBuilding()
    {
        if (!mGhostBuildingModel || !mHoveredHex.has_value()) return;

        glm::vec3 worldPos = mHoveredHex.value().ToWorldPosition();
        worldPos.y = 0.0f;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);

        Renderer::SetBlendMode(true);

        mTranslucentShader->Bind();
        mTranslucentShader->SetFloat3("uLightPos", glm::vec3(10.0f, 20.0f, 10.0f));
        mTranslucentShader->SetFloat3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        if (mGhostBuildingValid)
            mTranslucentShader->SetFloat3("uObjectColor", glm::vec3(0.5f, 1.0f, 0.5f));
        else
            mTranslucentShader->SetFloat3("uObjectColor", glm::vec3(1.0f, 0.3f, 0.3f));

        mTranslucentShader->SetFloat("uAlpha", 0.5f);
        mTranslucentShader->SetInt("uIsValid", mGhostBuildingValid ? 1 : 0);

        mGhostBuildingModel->Draw(mTranslucentShader, transform);

        Renderer::SetBlendMode(false);
    }

    void GameLayer::HandleLeftClick()
    {
        if (!mHoveredHex.has_value())
            return;

        const HexCoordinate& hexCoord = mHoveredHex.value();

        if (mSelectionManager->IsInPlacementMode())
        {
            auto* building = mBuildingManager->PlaceBuilding(
                mSelectionManager->GetPlacementType(),
                hexCoord,
                mPlayerFaction,
                *mResourceManager
            );

            if (building)
            {
                mSelectionManager->ClearPlacementMode();
                mGhostBuildingModel.reset();
            }
            return;
        }

        Unit* unit = mUnitManager->GetUnitAt(hexCoord);
        if (unit)
        {
            if (Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl))
            {
                mSelectionManager->AddUnitToSelection(unit);
            }
            else
            {
                mSelectionManager->SelectUnit(unit);
            }
            return;
        }

        Building* building = mBuildingManager->GetBuildingAt(hexCoord);
        if (building)
        {
            mSelectionManager->SelectHex(hexCoord);
            return;
        }

        mSelectionManager->SelectHex(hexCoord);
    }

    void GameLayer::HandleRightClick()
    {
        if (!mHoveredHex.has_value())
            return;

        const HexCoordinate& hexCoord = mHoveredHex.value();

        if (mSelectionManager->HasSelectedUnits())
        {
            Unit* targetUnit = mUnitManager->GetUnitAt(hexCoord);
            if (targetUnit && targetUnit->GetFaction() != mPlayerFaction)
            {
                mSelectionManager->IssueUnitAttackOrder(targetUnit);
                RF_INFO("Attack order issued");
                return;
            }

            mSelectionManager->IssueUnitMoveOrder(hexCoord, *mHexMap);
            RF_INFO("Move order issued to ({}, {})", hexCoord.q, hexCoord.r);
        }
    }
} // namespace RealmFortress
