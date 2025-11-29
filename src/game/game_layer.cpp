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
#include <imgui.h>

namespace RF
{
    GameLayer::GameLayer()
        : Layer("GameLayer"), mCameraController(1600.0f / 900.0f), mGhostBuildingValid(false), mTime(0.0f), mPulseIntensity(1.0f)
    {
    }

    void GameLayer::OnAttach()
    {
        RF_INFO("gameLayer attached");

        mShader = Shader::Create("assets/shaders/basic.glsl");
        mHighlightShader = Shader::Create("assets/shaders/hex_highlight.glsl");
        mTranslucentShader = Shader::Create("assets/shaders/translucent.glsl");

        mHexMap = CreateScope<HexMap>();
        mHexMap->Generate(5, 5);

        mBuildingManager = CreateScope<BuildingManager>();
        mBuildingManager->Initialize();

        mResourceManager = CreateScope<ResourceManager>();
        mSelectionManager = CreateScope<SelectionManager>();

        try
        {
            mHexHighlightModel = CreateRef<Model>("assets/objects/tiles/base/hex_grass.gltf");
        }
        catch (...)
        {
            RF_WARN("failed to load hex highlight model");
        }
    }

    void GameLayer::OnDetach()
    {
        RF_INFO("gameLayer detached");
    }

    void GameLayer::OnUpdate(Timestep ts)
    {
        mTime += ts;
        mPulseIntensity = 0.7f + 0.3f * std::sin(mTime * 3.0f);

        mCameraController.OnUpdate(ts);

        UpdateHoveredHex();

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

        if (mHoveredHex.has_value())
        {
            DrawHexHighlight(mHoveredHex.value(), glm::vec3(1.0f, 1.0f, 0.5f), 0.05f);
        }

        if (mSelectionManager->HasSelection())
        {
            DrawHexHighlight(mSelectionManager->GetSelectedHex(), glm::vec3(0.2f, 0.8f, 1.0f), 0.1f);
        }

        if (mSelectionManager->IsInPlacementMode() && mHoveredHex.has_value())
        {
            DrawGhostBuilding();
        }

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
        dispatcher.Dispatch<MouseMovedEvent>(RF_BIND_EVENT_FN(OnMouseMoved));
        dispatcher.Dispatch<KeyPressedEvent>(RF_BIND_EVENT_FN(OnKeyPressed));

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
        }

        return false;
    }

    void GameLayer::RenderBuildingMenu()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("buildings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (mSelectionManager->IsInPlacementMode())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                              "placement mode: %s",
                              BuildingTypeToString(mSelectionManager->GetPlacementType()));
            ImGui::Text("click on a hex to place");

            if (mHoveredHex.has_value())
            {
                if (mGhostBuildingValid)
                {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ valid placement");
                }
                else
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ cannot place here");
                }
            }

            ImGui::Separator();

            if (ImGui::Button("cancel (ESC)"))
            {
                mSelectionManager->ClearPlacementMode();
                mGhostBuildingModel.reset();
            }
        }
        else
        {
            ImGui::Text("select a building to place:");
            ImGui::Separator();

            for (const auto& def : mBuildingManager->GetAllDefinitions())
            {
                ImGui::PushID(static_cast<int>(def.Type));

                bool canAfford = true;
                for (const auto& cost : def.Cost.Resources)
                {
                    if (!mResourceManager->HasResource(cost.Type, cost.Amount))
                    {
                        canAfford = false;
                        break;
                    }
                }

                if (!canAfford)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
                }

                if (ImGui::Button(def.Name.c_str(), ImVec2(150, 0)))
                {
                    if (canAfford)
                    {
                        mSelectionManager->SetPlacementMode(def.Type);
                    }
                }

                if (!canAfford)
                {
                    ImGui::PopStyleColor();
                }

                ImGui::SameLine();
                ImGui::Text("-");
                ImGui::SameLine();

                // show cost
                std::string costStr;
                for (size_t i = 0; i < def.Cost.Resources.size(); i++)
                {
                    const auto& cost = def.Cost.Resources[i];
                    bool hasEnough = mResourceManager->HasResource(cost.Type, cost.Amount);

                    if (!hasEnough)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                    }

                    ImGui::SameLine();
                    ImGui::Text("%d %s", cost.Amount, ResourceTypeToString(cost.Type));

                    if (!hasEnough)
                    {
                        ImGui::PopStyleColor();
                    }
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("%s", def.Description.c_str());
                }

                ImGui::PopID();
            }
        }

        ImGui::Separator();
        ImGui::Text("buildings: %zu", mBuildingManager->GetAllBuildings().size());

        ImGui::End();
    }

    void GameLayer::RenderResourceDisplay()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("resources", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("wood:  %d", mResourceManager->GetResourceAmount(ResourceType::Wood));
        ImGui::Text("stone: %d", mResourceManager->GetResourceAmount(ResourceType::Stone));
        ImGui::Text("food:  %d", mResourceManager->GetResourceAmount(ResourceType::Food));
        ImGui::Text("gold:  %d", mResourceManager->GetResourceAmount(ResourceType::Gold));
        ImGui::Text("iron:  %d", mResourceManager->GetResourceAmount(ResourceType::Iron));

        ImGui::Separator();

        if (ImGui::Button("add resources (cheat)"))
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
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 280, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Debug Info");

        ImGui::Text("FPS: %.1f (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

        auto cameraPos = mCameraController.GetCamera().GetPosition();
        ImGui::Text("camera: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);

        ImGui::Separator();

        if (mHoveredHex.has_value())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "hover: (%d, %d)", mHoveredHex.value().q, mHoveredHex.value().r);
        }

        if (mSelectionManager->HasSelection())
        {
            auto selected = mSelectionManager->GetSelectedHex();
            ImGui::Text("selected: (%d, %d)", selected.q, selected.r);

            auto* building = mBuildingManager->GetBuildingAt(selected);
            if (building)
            {
                ImGui::Text("building: %s", building->GetName().c_str());
            }
        }

        ImGui::Separator();

        ImGui::Text("controls:");
        ImGui::BulletText("WASD - move camera");
        ImGui::BulletText("Q/E - move up/down");
        ImGui::BulletText("Right Mouse - rotate");
        ImGui::BulletText("Left Click - select/place");
        ImGui::BulletText("ESC - cancel placement");

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
                for (const auto& cost : def->Cost.Resources)
                {
                    if (!mResourceManager->HasResource(cost.Type, cost.Amount))
                    {
                        mGhostBuildingValid = false;
                        break;
                    }
                }
            }

            if (def && (!mGhostBuildingModel || mGhostBuildingModel.use_count() == 0))
            {
                try
                {
                    mGhostBuildingModel = CreateRef<Model>(def->ModelPath);
                }
                catch (...)
                {
                    RF_WARN("failed to load ghost building model");
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

        RenderCommand::SetBlend(true);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mHighlightShader->Bind();
        mHighlightShader->SetFloat3("uHighlightColor", color);
        mHighlightShader->SetFloat("uPulseIntensity", mPulseIntensity);

        mHexHighlightModel->Draw(mHighlightShader, transform);

        RenderCommand::SetBlend(false);
    }

    void GameLayer::DrawGhostBuilding()
    {
        if (!mGhostBuildingModel || !mHoveredHex.has_value()) return;

        glm::vec3 worldPos = mHoveredHex.value().ToWorldPosition();
        worldPos.y = 0.0f;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);

        RenderCommand::SetBlend(true);

        mTranslucentShader->Bind();
        mTranslucentShader->SetFloat3("uLightPos", glm::vec3(10.0f, 20.0f, 10.0f));
        mTranslucentShader->SetFloat3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        mTranslucentShader->SetFloat3("uObjectColor", glm::vec3(0.5f, 0.7f, 1.0f));
        mTranslucentShader->SetFloat("uAlpha", 0.5f);
        mTranslucentShader->SetInt("uIsValid", mGhostBuildingValid ? 1 : 0);

        mGhostBuildingModel->Draw(mTranslucentShader, transform);

        RenderCommand::SetBlend(false);
    }
} // namespace RF
