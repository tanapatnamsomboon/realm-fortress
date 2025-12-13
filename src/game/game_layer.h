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
#include "game/building/building_manager.h"
#include "game/decoration/decoration_manager.h"
#include "game/structure/structure_manager.h"
#include "game/ui/ui_manager.h"
#include "game/unit/unit_manager.h"

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
        bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
        bool OnMouseMoved(MouseMovedEvent& event);
        bool OnKeyPressed(KeyPressedEvent& event);

        void UpdateSelection();

    private:
        ShaderLibrary mShaderLibrary;
        Ref<Shader> mShader;
        Ref<Shader> mHighlightShader;

        Ref<CameraController> mCameraController;

        Map mMap;

        Picker mPicker;
        Selection mSelection;
        f32 mTime{ 0.0f };

        BuildingManager mBuildingManager;
        bool mBuildMode{ false };
        BuildingType mSelectedBuildingType{ BuildingType::None };
        Faction mPlayerFaction{ Faction::Blue };

        DecorationManager mDecorationManager;
        bool mDecorationMode{ false };
        DecorationType mSelectedDecorationType{ DecorationType::None };

        StructureManager mStructureManager;
        bool mStructureMode{ false };
        StructureType mSelectedStructureType{ StructureType::None };

        UnitManager mUnitManager;
        bool mUnitSpawnMode{ false };
        UnitType mSelectedUnitType{ UnitType::None };

        UIManager mUIManager;
    };
}
