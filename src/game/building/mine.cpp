/**
 * @file mine.cpp
 * @brief
 * @date 12/16/2025
 */

#include "core/pch.h"
#include "mine.h"
#include "game/resource/warehouse.h"

namespace RealmFortress
{
    Mine::Mine(const Coordinate& coordinate)
        : Building(BuildingType::Mine, coordinate), mTime(0.0f), mCycleTime(10.0f)
    {
    }

    void Mine::OnUpdate(Timestep ts)
    {
        mTime += ts;

        if (mTime < mCycleTime)
            return;

        mTime -= mCycleTime;

        Warehouse::Get().Add(ResourceType::Stone, 1);
    }

    void Mine::OnPlaced()
    {
    }

    bool Mine::CanPlace(const Coordinate& coord, const Map& map) const
    {
        // TODO: implement
        return true;
    }
} // namespace RealmFortress
