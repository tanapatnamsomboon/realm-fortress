/**
 * @file game_layer.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/base.h"
#include "core/layer.h"
#include "renderer/shader.h"
#include "renderer/vertex_array.h"
#include "events/event.h"
#include "events/mouse_event.h"
#include "events/key_event.h"
#include "game/system/camera_controller.h"
#include "game/system/picker.h"
#include "game/system/selection.h"
#include "game/resource/warehouse.h"
#include "game/building/building_manager.h"
#include "map/map.h"

struct ImVec2;

namespace RealmFortress
{
    enum class GameMode
    {
        Normal,
        Building
    };

    enum UIPanelFlag : u16
    {
        UIPanelNone          = 0,
        UIPanelBuilding      = BIT(0),
        UIPanelEconomy       = BIT(1),
        UIPanelInspect       = BIT(2),
        UIPanelBuildConfirm  = BIT(3),
    };

#   define UI_PANEL_IS_OPEN(flags, panel)   ((flags) & (panel))
#   define UI_PANEL_OPEN(flags, panel)      ((flags) |= (panel))
#   define UI_PANEL_OPEN_ONLY(flags, panel) ((flags) &= (panel))
#   define UI_PANEL_CLOSE(flags, panel)     ((flags) &= ~(panel))
#   define UI_PANEL_TOGGLE(flags, panel)    ((flags) ^= (panel))
#   define UI_PANEL_CLOSE_ALL(flags)        ((flags) = 0)

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

        // UI
        void SetupTheme();

        void DrawTimeHUD();
        void DrawActionBar(ImVec2* out_pos, ImVec2* out_size);
        void DrawBuildingPanel(ImVec2 action_bar_pos, ImVec2 action_bar_size, ImVec2* out_pos, ImVec2* out_size);
        void DrawBuildConfirmPanel(ImVec2 building_panel_pos, ImVec2 building_panel_size);
        void DrawEconomyPanel();
        void DrawInspectPanel();

    private:
        ShaderLibrary mShaderLibrary;
        Ref<Shader> mBasicShader;
        Ref<Shader> mInstanceShader;
        Ref<Shader> mGhostBuildingShader;

        Ref<CameraController> mCameraController;

        Map mMap;

        Picker mPicker;
        Selection mSelection;
        f32 mTime{ 0.0f };

        i16 mUIPanelFlags{ 0 };
        std::optional<BuildingType> mSelectedBuildingToConfirm;

        f32 mGameTime{ 0.0f };
        i32 mGameDay{ 1 };
        f32 mTimeSpeed{ 1.0f };

        GameMode mGameMode{ GameMode::Normal };
        BuildingType mSelectedBuildingType{ BuildingType::Mine };
        Building* mInspectedBuilding{ nullptr };

        Ref<VertexArray> mSelectionRingVA;
    };
}
