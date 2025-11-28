/**
 * @file imgui_layer.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

struct GLFWwindow;

namespace RF
{
    /**
     * @class ImGuiLayer
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class ImGuiLayer
    {
    public:
        ImGuiLayer(GLFWwindow* window);
        ~ImGuiLayer();

        void Begin();
        void End();
    };
} // namespace RF
