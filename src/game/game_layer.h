/**
 * @file game_layer.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/layer.h"
#include "system/camera_controller.h"
#include "renderer/shader.h"
#include "events/event.h"
#include "events/mouse_event.h"
#include "events/key_event.h"
#include "game/system/map.h"
#include "game/system/picker.h"
#include "game/system/selection.h"
#include "game/resource/warehouse.h"
#include "game/building/building_manager.h"

namespace RealmFortress
{
    enum class GameMode
    {
        Normal,
        Building
    };

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
        bool OnMouseMoved(MouseMovedEvent& event);
        bool OnKeyPressed(KeyPressedEvent& event);

        void UpdateSelection();

        void EnterBuildMode(BuildingType type);
        void ExitBuildMode();
        void PlaceBuildingAtSelection();
        void DrawGhostBuilding();
        std::unordered_set<Coordinate> GetBuildableTiles() const;

        void DrawResourcePanel();
        void DrawBuildingMenu();
        void DrawBuildingInfo();
        void DrawSelectionInfo();

    private:
        ShaderLibrary mShaderLibrary;
        Ref<Shader> mShader;
        Ref<Shader> mHighlightShader;

        Ref<CameraController> mCameraController;

        Map mMap;

        Picker mPicker;
        Selection mSelection;
        f32 mTime{ 0.0f };

        GameMode mGameMode{ GameMode::Normal };
        BuildingType mSelectedBuildingType{ BuildingType::Mine };
        Building* mInspectedBuilding{ nullptr };
    };
}
