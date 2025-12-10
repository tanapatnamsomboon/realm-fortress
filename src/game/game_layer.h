/**
 * @file game_layer.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/layer.h"
#include "scene/camera_controller.h"
#include "renderer/shader.h"
#include "events/event.h"
#include "events/mouse_event.h"
#include "events/key_event.h"
#include "game/system/map.h"

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

    private:
        ShaderLibrary mShaderLibrary;
        Ref<Shader> mShader;

        Ref<CameraController> mCameraController;

        Map mMap;
    };
}
