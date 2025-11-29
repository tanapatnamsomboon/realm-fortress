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
#include <imgui.h>

namespace RF
{
    GameLayer::GameLayer()
        : Layer("GameLayer"), mCameraController(1600.0f / 900.0f)
    {
    }

    void GameLayer::OnAttach()
    {
        RF_INFO("GameLayer attached");

        mShader = Shader::Create("assets/shaders/basic.glsl");

        mHexMap = CreateScope<HexMap>();
        mHexMap->Generate(5, 5);

        mBuildingManager = CreateScope<BuildingManager>();
        mBuildingManager->Initialize();

        mResourceManager = CreateScope<ResourceManager>();
        mSelectionManager = CreateScope<SelectionManager>();
    }

    void GameLayer::OnDetach()
    {
        RF_INFO("GameLayer detached");
    }

    void GameLayer::OnUpdate(Timestep ts)
    {
        mCameraController.OnUpdate(ts);

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
        RenderCommand::Clear();

        Renderer::BeginScene(mCameraController.GetCamera());

        mShader->Bind();
        mShader->SetFloat3("uLightPos", glm::vec3(10.0f, 20.0f, 10.0f));
        mShader->SetFloat3("uViewPos", mCameraController.GetCamera().GetPosition());
        mShader->SetFloat3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        mShader->SetFloat3("uTintColor", glm::vec3(1.0f, 1.0f, 1.0f));

        mHexMap->Draw(mShader);

        mBuildingManager->DrawBuildings(mShader);

        Renderer::EndScene();
    }

    void GameLayer::OnImGuiRender()
    {
        RenderResourceDisplay();
        RenderBuildingMenu();
        RenderDebugInfo();
    }

    void GameLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(RF_BIND_EVENT_FN(OnMouseButtonPressed));

        mCameraController.OnEvent(event);
    }

    bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
    {
        if (event.GetMouseButton() == Mouse::ButtonLeft)
        {
            auto& window = Application::Get().GetWindow();
            auto mousePos = Input::GetMousePosition();

            auto hexCoord = mSelectionManager->GetHexAtScreenPosition(
                mousePos.x, mousePos.y,
                window.GetWidth(), window.GetHeight(),
                mCameraController.GetCamera(),
                *mHexMap
            );

            if (hexCoord.has_value())
            {
                if (mSelectionManager->IsInPlacementMode())
                {
                    auto* building = mBuildingManager->PlaceBuilding(
                        mSelectionManager->GetPlacementType(),
                        hexCoord.value(),
                        *mResourceManager
                    );

                    if (building)
                    {
                        mSelectionManager->ClearPlacementMode();
                    }
                }
                else
                {
                    mSelectionManager->SelectHex(hexCoord.value());
                }

                return true;
            }
        }

        return false;
    }

    void GameLayer::RenderBuildingMenu()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 180), ImGuiCond_FirstUseEver);
        ImGui::Begin("Buildings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (mSelectionManager->IsInPlacementMode())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                              "Placement Mode: %s",
                              BuildingTypeToString(mSelectionManager->GetPlacementType()));
            ImGui::Text("Click on a hex to place");

            if (ImGui::Button("Cancel"))
            {
                mSelectionManager->ClearPlacementMode();
            }
        }
        else
        {
            ImGui::Text("Select a building to place:");
            ImGui::Separator();

            for (const auto& def : mBuildingManager->GetAllDefinitions())
            {
                ImGui::PushID(static_cast<int>(def.Type));

                if (ImGui::Button(def.Name.c_str(), ImVec2(150, 0)))
                {
                    mSelectionManager->SetPlacementMode(def.Type);
                }

                ImGui::SameLine();
                ImGui::Text("-");
                ImGui::SameLine();

                std::string costStr;
                for (size_t i = 0; i < def.Cost.Resources.size(); i++)
                {
                    const auto& cost = def.Cost.Resources[i];
                    costStr += std::to_string(cost.Amount) + " " + ResourceTypeToString(cost.Type);
                    if (i < def.Cost.Resources.size() - 1)
                    {
                        costStr += ", ";
                    }
                }
                ImGui::TextDisabled("%s", costStr.c_str());

                ImGui::PopID();
            }
        }

        ImGui::Separator();
        ImGui::Text("Buildings: %zu", mBuildingManager->GetAllBuildings().size());

        ImGui::End();
    }

    void GameLayer::RenderResourceDisplay()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Wood:  %d", mResourceManager->GetResourceAmount(ResourceType::Wood));
        ImGui::Text("Stone: %d", mResourceManager->GetResourceAmount(ResourceType::Stone));
        ImGui::Text("Food:  %d", mResourceManager->GetResourceAmount(ResourceType::Food));
        ImGui::Text("Gold:  %d", mResourceManager->GetResourceAmount(ResourceType::Gold));
        ImGui::Text("Iron:  %d", mResourceManager->GetResourceAmount(ResourceType::Iron));

        ImGui::Separator();

        if (ImGui::Button("Add Resources (Cheat)"))
        {
            mResourceManager->AddResource(ResourceType::Wood, 100);
            mResourceManager->AddResource(ResourceType::Stone, 50);
            mResourceManager->AddResource(ResourceType::Food, 100);
            mResourceManager->AddResource(ResourceType::Gold, 10);
            mResourceManager->AddResource(ResourceType::Iron, 20);
        }

        ImGui::End();
    }

    void GameLayer::RenderDebugInfo()
    {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 250, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Debug Info");

        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        auto cameraPos = mCameraController.GetCamera().GetPosition();
        ImGui::Text("Camera: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);

        ImGui::Separator();

        if (mSelectionManager->HasSelection())
        {
            auto selected = mSelectionManager->GetSelectedHex();
            ImGui::Text("Selected Hex: (%d, %d)", selected.q, selected.r);

            auto* building = mBuildingManager->GetBuildingAt(selected);
            if (building)
            {
                ImGui::Text("Building: %s", building->GetName().c_str());
            }
        }
        else
        {
            ImGui::Text("No selection");
        }

        ImGui::Separator();

        ImGui::Text("Controls:");
        ImGui::BulletText("WASD - Move camera");
        ImGui::BulletText("Q/E - Move up/down");
        ImGui::BulletText("Right Mouse - Rotate");
        ImGui::BulletText("Left Click - Select/Place");

        ImGui::End();
    }
} // namespace RF
