/**
 * @file game_layer.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/layer.h"
#include "game/hex_map.h"
#include "game/building.h"
#include "game/resource_type.h"
#include "game/selection.h"
#include "scene/camera_controller.h"
#include "renderer/shader.h"

namespace RF
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

        void RenderBuildingMenu();
        void RenderResourceDisplay();
        void RenderDebugInfo();

        void UpdateHoveredHex();
        void DrawHexHighlight(const HexCoordinate& coord, const glm::vec3& color, f32 elevation = 0.05f);
        void DrawGhostBuilding();

    private:
        CameraController mCameraController;

        Ref<Shader> mShader;
        Ref<Shader> mHighlightShader;
        Ref<Shader> mTranslucentShader;

        Scope<HexMap> mHexMap;
        Scope<BuildingManager> mBuildingManager;
        Scope<ResourceManager> mResourceManager;
        Scope<SelectionManager> mSelectionManager;

        std::optional<HexCoordinate> mHoveredHex;
        Ref<Model> mHexHighlightModel;

        Ref<Model> mGhostBuildingModel;
        bool mGhostBuildingValid;

        f32 mTime;
        f32 mPulseIntensity;
    };
}
