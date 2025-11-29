/**
 * @file imgui_layer.h
 * @brief
 * @date 11/29/2025
 */

#pragma once

#include "core/layer.h"
#include "events/application_event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"

namespace RF
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(Event& event) override;

        void Begin();
        void End();

        void BlockEvent(bool block) { mBlockEvent = block; }

    private:
        bool mBlockEvent = true;
        f32 mTime = 0.0f;
    };
} // namespace RF
