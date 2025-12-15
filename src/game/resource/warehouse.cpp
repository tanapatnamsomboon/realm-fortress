/**
 * @file warehouse.cpp
 * @brief
 * @date 12/16/2025
 */

#include "core/pch.h"
#include "warehouse.h"

namespace RealmFortress
{
    void Warehouse::OnUpdate(Timestep ts)
    {
    }

    bool Warehouse::Add(ResourceType type, i32 amount)
    {
        ResourceCost cost;
        cost[type] = amount;

        return Add(cost);
    }

    bool Warehouse::Add(const ResourceCost& cost)
    {
        if (!CanFit(cost))
        {
            return false;
        }

        mStorage.Add(cost);
        RecalculateUsedSpace();
        return true;
    }

    bool Warehouse::Consume(ResourceType type, i32 amount)
    {
        bool success = mStorage.Consume(type, amount);
        if (success)
            RecalculateUsedSpace();

        return success;
    }

    bool Warehouse::Consume(const ResourceCost& cost)
    {
        bool success = mStorage.Consume(cost);
        if (success)
            RecalculateUsedSpace();

        return success;
    }

    bool Warehouse::Has(ResourceType type, i32 amount) const
    {
        return mStorage.Has(type, amount);
    }

    bool Warehouse::Has(const ResourceCost& cost) const
    {
        return mStorage.Has(cost);
    }

    bool Warehouse::CanFit(const ResourceCost& cost) const
    {
        i32 space_needed = 0;
        for (const auto& [type, amount] : cost)
        {
            space_needed += ResourceTypeToSpace(type) * amount;
        }

        i32 remaining_space = mCapacity - mUsedSpace;
        return space_needed <= remaining_space;
    }

    i32 Warehouse::GetAmount(ResourceType type) const
    {
        return mStorage.GetAmount(type);
    }

    i32 Warehouse::GetUsedSpace() const
    {
        return mUsedSpace;
    }

    i32 Warehouse::GetCapacity() const
    {
        return mCapacity;
    }

    f32 Warehouse::GetUsagePercent() const
    {
        return mCapacity > 0 ? static_cast<f32>(mUsedSpace) / mCapacity : 0.0f;
    }

    void Warehouse::IncreaseCapacity(i32 amount)
    {
        mCapacity += amount;
    }

    Warehouse::Warehouse()
        : mUsedSpace(0), mCapacity(500)
    {
    }

    void Warehouse::RecalculateUsedSpace()
    {
        mUsedSpace = mStorage.GetUsedSpace();
    }
} // namespace RealmFortress
