/**
 * @file townhall.cpp
 * @brief
 * @date 12/18/2025
 */

#include "core/pch.h"
#include "townhall.h"
#include "game/resource/warehouse.h"
#include "game/building/building_manager.h"
#include <imgui.h>

namespace RealmFortress
{
    Townhall::Townhall(const Coordinate& coordinate)
        : Building(BuildingType::Townhall, coordinate)
        , mStorageCapacityBonus(200), mTime(0.0f), mFoodCycleTime(15.0f)
        , mFoodNeeded(5), mIsStarving(false)
    {
    }

    void Townhall::OnUpdate(Timestep ts)
    {
        mTime += ts;

        if (mTime >= mFoodCycleTime)
        {
            mTime -= mFoodCycleTime;

            usize total_buildings = BuildingManager::Get().GetAllBuildings().size();

            mFoodNeeded = 5 + (static_cast<i32>(total_buildings) * 2);

            // try to consume food
            if (Warehouse::Get().Consume(ResourceType::Wheat, mFoodNeeded))
            {
                if (mIsStarving)
                {
                    RF_CORE_INFO("Townhall is operational again (Wheat consumed: {})", mFoodNeeded);
                    mIsStarving = false;
                }
            }
            else
            {
                if (!mIsStarving)
                {
                    RF_CORE_WARN("Townhall is STARVING! Not enough Wheat (Needed: {}). Efficiency penalized.", mFoodNeeded);
                    mIsStarving = true;
                }
            }
        }
    }

    void Townhall::OnInspected()
    {
        // Food Consumption Rate
        ImGui::Text("Food Consumption: %.1f", mFoodNeeded / mFoodCycleTime);

        // Well-Being
        char status_text[32];
        ImVec4 status_color;
        if (mIsStarving)
        {
            sprintf(status_text, "Starving");
            status_color = ImColor(225, 25, 25);
        }
        else
        {
            sprintf(status_text, "Comfortable");
            status_color = ImColor(25, 225, 25);
        }
        ImGui::Text("Living Conditions:");
        ImGui::SameLine();
        ImGui::TextColored(status_color, "%s", status_text);
    }

    void Townhall::OnPlaced(const Map& map)
    {
        Warehouse::Get().IncreaseCapacity(mStorageCapacityBonus);
        RF_CORE_INFO("Townhall placed. Warehouse capacity increased by {}", mStorageCapacityBonus);
    }

    void Townhall::OnDestroyed(const Map& map)
    {
        // TODO: implement DecreaseCapacity()
        Warehouse::Get().IncreaseCapacity(-mStorageCapacityBonus);
    }

    void Townhall::OnMapChanged(const Map& map)
    {
    }

    bool Townhall::CanPlace(const Coordinate& coord, const Map& map) const
    {
        auto tile = map.GetTile(coord);

        if (tile->GetDecoration() != DecorationType::None) return false;
        if (tile->GetType() == TileType::Water) return false;

        return true;
    }
}
