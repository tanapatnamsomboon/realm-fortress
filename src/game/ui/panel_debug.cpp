/**
 * @file panel_debug.cpp
 * @brief
 * @date 12/13/2025
 */

#include "core/pch.h"
#include "panel_debug.h"
#include "renderer/renderer.h"
#include <imgui.h>

namespace RealmFortress
{
    PanelDebug::PanelDebug()
        : PanelBase("Debug")
    {
    }

    void PanelDebug::OnRender()
    {
        if (!mVisible) return;

        ImGui::SetNextWindowPos(ImVec2(10, 550), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

        if (ImGui::Begin(mName.c_str(), &mVisible))
        {
            ImGui::Text("FPS: %.1f (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

            ImGui::Separator();

            if (mCamera)
            {
                auto pos = mCamera->GetPosition();
                ImGui::Text("Camera Position:");
                ImGui::Text("  X: %.2f, Y: %.2f, Z: %.2f", pos.x, pos.y, pos.z);
            }

            ImGui::Separator();

            ImGui::Text("Scene Objects:");
            ImGui::Text("  Tiles: %zu", mTileCount);
            ImGui::Text("  Buildings: %zu", mBuildingCount);
            ImGui::Text("  Structures: %zu", mStructureCount);

            ImGui::Separator();

            auto& stats = Renderer::GetStats();
            ImGui::Text("Renderer:");
            ImGui::Text("  Draw Calls: %u", stats.DrawCalls);
            ImGui::Text("  Triangles: %u", stats.TriangleCount);
        }
        ImGui::End();
    }
} // namespace RealmFortress
