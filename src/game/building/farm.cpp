/**
 * @file farm.cpp
 * @brief
 * @date 12/18/2025
 */

#include "core/pch.h"
#include "farm.h"
#include "game/resource/warehouse.h"

namespace RealmFortress
{
    Farm::Farm(const Coordinate& coordinate)
        : Building(BuildingType::Farm, coordinate), mTime(0.0f), mCycleTime(12.0f)
    {
    }

    void Farm::OnUpdate(Timestep ts)
    {
        mTime += ts;

        if (mTime < mCycleTime)
            return;

        mTime -= mCycleTime;

        Warehouse::Get().Add(ResourceType::Wheat, 10);
    }

    void Farm::OnInspected()
    {
    }

    void Farm::OnPlaced(const Map& map)
    {
    }

    void Farm::OnDestroyed(const Map& map)
    {
    }

    void Farm::OnMapChanged(const Map& map)
    {
    }

    bool Farm::CanPlace(const Coordinate& coord, const Map& map) const
    {
        auto tile = map.GetTile(coord);
        if (tile->GetDecoration() != DecorationType::None) return false;
        if (tile->GetType() == TileType::Water) return false;

        bool riverside = false;
        for (auto neighbor : map.GetNeighbors(coord))
        {
            if (neighbor->GetType() == TileType::Water)
            {
                riverside = true;
                break;
            }
        }
        if (!riverside) return false;

        return true;
    }
} // namespace RealmFortress
