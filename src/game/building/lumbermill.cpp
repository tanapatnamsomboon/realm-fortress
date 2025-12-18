/**
 * @file lumbermill.cpp
 * @brief
 * @date 12/18/2025
 */

#include "core/pch.h"
#include "lumbermill.h"
#include "game/resource/warehouse.h"
#include "game/building/building_manager.h"
#include "game/building/townhall.h"
#include <imgui.h>

namespace RealmFortress
{
    Lumbermill::Lumbermill(const Coordinate& coordinate)
        : Building(BuildingType::LumberMill, coordinate)
        , mTime(0.0f), mBaseCycleTime(10.0f), mEfficiency(1.0f), mProgress(0.0f)
    {
    }

    void Lumbermill::OnUpdate(Timestep ts)
    {
        mTime += ts;

        if (mEfficiency == 0) return;

        f32 actual_cycle_time = mBaseCycleTime / mEfficiency;

        mProgress = std::clamp(mTime / actual_cycle_time, 0.0f, 1.0f);

        if (mTime >= actual_cycle_time)
        {
            mTime -= actual_cycle_time;
            Warehouse::Get().Add(ResourceType::Lumber, 1);
        }
    }

    void Lumbermill::OnInspected()
    {
        // Efficiency Stats
        ImVec4 efficiency_color = (mEfficiency >= 1.0f) ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1);

        ImGui::Text("Efficiency:");
        ImGui::SameLine();
        ImGui::TextColored(efficiency_color, "%.0f%%", mEfficiency * 100.0f);

        ImGui::Text("Progress");
        char overlay[32];
        sprintf(overlay, "%.0f%%", mProgress * 100.0f);
        ImGui::ProgressBar(mProgress, ImVec2(-1, 0), overlay);
    }

    void Lumbermill::OnPlaced(const Map& map)
    {
        UpdateEfficiency(map);
    }

    void Lumbermill::OnDestroyed(const Map& map)
    {
    }

    void Lumbermill::OnMapChanged(const Map& map)
    {
        UpdateEfficiency(map);
    }

    bool Lumbermill::CanPlace(const Coordinate& coord, const Map& map) const
    {
        auto tile = map.GetTile(coord);
        if (tile->GetDecoration() != DecorationType::None) return false;
        if (tile->GetType() == TileType::Water) return false;

        return true;
    }

    void Lumbermill::UpdateEfficiency(const Map& map)
    {
        f32 resource_factor = 1.0f + CalculateResourceBonus(map);

        f32 townhall_factor = 1.0f;

        auto townhalls = BuildingManager::Get().GetBuildingsByType(BuildingType::Townhall);
        RF_CORE_ASSERT(townhalls.size() == 1, "Only one townhall!");

        if (townhalls.empty())
        {
            townhall_factor = 0.1f;
        }
        else
        {
            auto townhall = static_cast<Townhall*>(townhalls.front());

            if (townhall->IsStarving())
            {
                townhall_factor = 0.2f;
            }
            else
            {
                i32 distance = mCoord.DistanceTo(townhall->GetCoordinate());
                townhall_factor = std::max(0.2f, 1.0f - (distance * 0.02f));
            }
        }

        mEfficiency = resource_factor * townhall_factor;
    }

    f32 Lumbermill::CalculateResourceBonus(const Map& map)
    {
        i32 forest_count = 0;
        auto neighbors = map.GetNeighbors(mCoord);
        for (const auto& neighbor : neighbors)
        {
            if (neighbor->GetDecoration() == DecorationType::TreeSmall  ||
                neighbor->GetDecoration() == DecorationType::TreeMedium ||
                neighbor->GetDecoration() == DecorationType::TreeLarge)
            {
                forest_count++;
            }
        }

        return static_cast<f32>(forest_count) * 0.20f;
    }
} // namespace RealmFortress
