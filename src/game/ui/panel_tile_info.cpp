/**
 * @file panel_tile_info.cpp
 * @brief
 * @date 12/13/2025
 */

#include "core/pch.h"
#include "panel_tile_info.h"
#include <imgui.h>

namespace RealmFortress
{
    PanelTileInfo::PanelTileInfo()
        : PanelBase("Tile Info")
    {
    }

    void PanelTileInfo::OnRender()
    {
        if (!mVisible) return;

        ImGui::SetNextWindowPos(ImVec2(240, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_FirstUseEver);

        if (ImGui::Begin(mName.c_str(), &mVisible))
        {
            if (mSelectedTile)
            {
                auto coord = mSelectedTile->GetCoordinate();
                ImGui::Text("Coordinate: (%d, %d)", coord.Q, coord.R);
                ImGui::Text("Type: %s", TileTypeToString(mSelectedTile->GetType()));
                ImGui::Text("Elevation: %d", mSelectedTile->GetElevation());
                ImGui::Text("Walkable: %s", mSelectedTile->IsWalkable() ? "Yes" : "No");

                ImGui::Separator();

                if (mSelectedBuilding)
                {
                    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Building");
                    ImGui::Text("  Type: %s", BuildingTypeToString(mSelectedBuilding->GetType()));
                    ImGui::Text("  Faction: %s", FactionToString(mSelectedBuilding->GetFaction()));
                    ImGui::Text("  Status: %s", mSelectedBuilding->IsConstructed() ? "Complete" : "Building");
                }
                else if (mSelectedStructure)
                {
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.4f, 1.0f), "Structure");
                    ImGui::Text("  Type: Neutral");
                    ImGui::Text("  Rotation: %.0f°", mSelectedStructure->GetRotation());
                }
                else
                {
                    ImGui::TextDisabled("No building/structure");
                }
            }
            else
            {
                ImGui::TextDisabled("No tile selected");
                ImGui::TextDisabled("Click on a hex to view info");
            }
        }
        ImGui::End();
    }

    void PanelTileInfo::Clear()
    {
        mSelectedTile = nullptr;
        mSelectedBuilding = nullptr;
        mSelectedStructure = nullptr;
    }
} // namespace RealmFortress
