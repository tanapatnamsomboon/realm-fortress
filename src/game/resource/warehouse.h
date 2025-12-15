/**
 * @file warehouse.h
 * @brief
 * @date 12/16/2025
 */

#pragma once

#include "core/base.h"
#include "core/timestep.h"
#include "game/resource/resource.h"

namespace RealmFortress
{
    class Warehouse
    {
    public:
        static Warehouse& Get()
        {
            static Warehouse instance;
            return instance;
        }

        Warehouse(const Warehouse&) = delete;
        Warehouse& operator=(const Warehouse&) = delete;

        void OnUpdate(Timestep ts);

        bool Add(ResourceType type, i32 amount);
        bool Add(const ResourceCost& cost);

        bool Consume(ResourceType type, i32 amount);
        bool Consume(const ResourceCost& cost);

        bool Has(ResourceType type, i32 amount) const;
        bool Has(const ResourceCost& cost) const;

        bool CanFit(const ResourceCost& cost) const;

        i32 GetAmount(ResourceType type) const;
        i32 GetUsedSpace() const;
        i32 GetCapacity() const;
        f32 GetUsagePercent() const;

        void IncreaseCapacity(i32 amount);

    private:
        Warehouse();

        void RecalculateUsedSpace();

    private:
        ResourceStorage mStorage;
        i32 mUsedSpace;
        i32 mCapacity;
    };
} // namespace RealmFortress
