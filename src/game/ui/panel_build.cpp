/**
 * @file panel_build.cpp
 * @brief
 * @date 12/13/2025
 */

#include "core/pch.h"
#include "panel_build.h"
#include <imgui.h>

namespace RealmFortress
{
    PanelBuild::PanelBuild()
        : PanelBase("Build")
    {
    }

    void PanelBuild::OnRender()
    {
        if (!mVisible) return;

        ImGui::SetNextWindowPos(ImVec2(10, 140), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(220, 400), ImGuiCond_FirstUseEver);

        if (ImGui::Begin(mName.c_str(), &mVisible))
        {
            if (ImGui::CollapsingHeader("Buildings", ImGuiTreeNodeFlags_DefaultOpen))
            {
                RenderBuildingButtons();
            }

            if (ImGui::CollapsingHeader("Structures"))
            {
                RenderStructureButtons();
            }
        }
        ImGui::End();
    }

    void PanelBuild::RenderBuildingButtons()
    {
        constexpr f32 button_width = 200.0f;

        if (ImGui::Button("Town Hall", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::TownHall);

        if (ImGui::Button("House", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::HomeA);

        if (ImGui::Button("Barracks", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::Barracks);

        if (ImGui::Button("Archery Range", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::ArcheryRange);

        if (ImGui::Button("Stables", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::Stables);

        if (ImGui::Button("Blacksmith", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::Blacksmith);

        if (ImGui::Button("Farm", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::LumberMill);

        if (ImGui::Button("Mine", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::Mine);

        if (ImGui::Button("Market", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::Market);

        if (ImGui::Button("Tower", ImVec2(button_width, 0)) && mOnBuildingSelected)
            mOnBuildingSelected(BuildingType::TowerA);
    }

    void PanelBuild::RenderStructureButtons()
    {
        constexpr f32 button_width = 200.0f;

        if (ImGui::Button("Wall", ImVec2(button_width, 0)) && mOnStructureSelected)
            mOnStructureSelected(StructureType::WallStraight);

        if (ImGui::Button("Wall Gate", ImVec2(button_width, 0)) && mOnStructureSelected)
            mOnStructureSelected(StructureType::WallStraightGate);

        if (ImGui::Button("Wood Fence", ImVec2(button_width, 0)) && mOnStructureSelected)
            mOnStructureSelected(StructureType::FenceWoodStraight);

        if (ImGui::Button("Stone Fence", ImVec2(button_width, 0)) && mOnStructureSelected)
            mOnStructureSelected(StructureType::FenceStoneStraight);

        if (ImGui::Button("Bridge", ImVec2(button_width, 0)) && mOnStructureSelected)
            mOnStructureSelected(StructureType::BridgeA);
    }
} // namespace RealmFortress
