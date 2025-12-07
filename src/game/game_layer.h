/**
 * @file game_layer.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/layer.h"
#include "game/hex/hex_map.h"
#include "game/building/building.h"
#include "game/unit/unit.h"
#include "game/resource/resource_type.h"
#include "game/selection.h"
#include "game/faction.h"
#include "scene/camera_controller.h"
#include "renderer/shader.h"

namespace RealmFortress
{
    class GameLayer final : public Layer
    {
    public:
        GameLayer();
        ~GameLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(Timestep ts) override;
        void OnImGuiRender() override;
        void OnEvent(Event& event) override;

    private:
        bool OnMouseButtonPressed(class MouseButtonPressedEvent& event);
        bool OnMouseMoved(class MouseMovedEvent& event);
        bool OnKeyPressed(class KeyPressedEvent& event);

        void RenderMainUI();
        void RenderBuildingMenu();
        void RenderUnitPanel();
        void RenderResourceDisplay();
        void RenderMinimap();
        void RenderDebugInfo();

        void UpdateHoveredHex();
        void DrawHexHighlight(const HexCoordinate& coord, const glm::vec3& color, f32 elevation = 0.05f);
        void DrawSelectionRing(const glm::vec3& worldPos, f32 radius, const glm::vec3& color);
        void DrawUnitPath(const Unit* unit);
        void DrawGhostBuilding();

        void HandleLeftClick();
        void HandleRightClick();

    private:
        CameraController mCameraController;
        Faction mPlayerFaction = Faction::Player;

        Ref<Shader> mShader;
        Ref<Shader> mHighlightShader;
        Ref<Shader> mTranslucentShader;

        Scope<HexMap> mHexMap;
        Scope<BuildingManager> mBuildingManager;
        Scope<UnitManager> mUnitManager;
        Scope<ResourceManager> mResourceManager;
        Scope<SelectionManager> mSelectionManager;

        std::optional<HexCoordinate> mHoveredHex;
        Ref<Model> mHexHighlightModel;
        Ref<Model> mGhostBuildingModel;
        bool mGhostBuildingValid = false;

        f32 mTime = 0.0f;
        f32 mPulseIntensity = 1.0f;

        bool mShowBuildingMenu = true;
        bool mShowUnitPanel = true;
        bool mShowResourcePanel = true;
        bool mShowDebugInfo = true;
    };
}
