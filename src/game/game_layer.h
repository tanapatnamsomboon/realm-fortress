/**
 * @file game_layer.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/layer.h"
#include "game/hex_map.h"
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
        void OnEvent(Event& event) override;

    private:
        CameraController mCameraController;

        Ref<Shader> mShader;
        Scope<HexMap> mHexMap;
    };
}
