/**
 * @file panel_resource.cpp
 * @brief
 * @date 12/13/2025
 */

#include "core/pch.h"
#include "panel_resources.h"
#include <imgui.h>

namespace RealmFortress
{
    PanelResources::PanelResources()
        : PanelBase("Resources")
    {
    }

    void PanelResources::OnRender()
    {
        if (!mVisible) return;

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiCond_FirstUseEver);

        if (ImGui::Begin(mName.c_str(), &mVisible, ImGuiWindowFlags_NoResize))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));
            ImGui::Text("Wood:  %d", mWood);
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
            ImGui::Text("Stone: %d", mStone);
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.84f, 0.0f, 1.0f));
            ImGui::Text("Gold:  %d", mGold);
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
            ImGui::Text("Food:  %d", mFood);
            ImGui::PopStyleColor();
        }
        ImGui::End();
    }
} // namespace RealmFortress
