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
        bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

        void RenderBuildingMenu();
        void RenderResourceDisplay();
        void RenderDebugInfo();

    private:
        CameraController mCameraController;

        Ref<Shader> mShader;
        Scope<HexMap> mHexMap;
        Scope<BuildingManager> mBuildingManager;
        Scope<ResourceManager> mResourceManager;
        Scope<SelectionManager> mSelectionManager;
    };
}
