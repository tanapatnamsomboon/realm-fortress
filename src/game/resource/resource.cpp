/**
 * @file resource.cpp
 * @brief
 * @date 12/16/2025
 */

#include "core/base.h"
#include "resource.h"

#include <ranges>

namespace RealmFortress
{
    ResourceStorage::ResourceStorage()
    {
        mStorages[ResourceType::Lumber] = 0;
        mStorages[ResourceType::Stone] = 0;
        mStorages[ResourceType::Wheat] = 0;
    }

    void ResourceStorage::Add(ResourceType type, i32 amount)
    {
        mStorages[type] += amount;
    }

    void ResourceStorage::Add(const ResourceCost& cost)
    {
        for (const auto& [type, amount] : cost)
        {
            mStorages[type] += amount;
        }
    }

    bool ResourceStorage::Consume(ResourceType type, i32 amount)
    {
        if (!Has(type, amount))
        {
            return false;
        }

        mStorages[type] -= amount;
        return true;
    }

    bool ResourceStorage::Consume(const ResourceCost& cost)
    {
        if (!Has(cost))
        {
            return false;
        }

        for (const auto& [type, amount] : cost)
        {
            mStorages[type] -= amount;
        }
        return true;
    }

    bool ResourceStorage::Has(ResourceType type, i32 amount) const
    {
        return GetAmount(type) >= amount;
    }

    bool ResourceStorage::Has(const ResourceCost& cost) const
    {
        for (const auto& [type, amount] : cost)
        {
            if (!Has(type, amount))
            {
                return false;
            }
        }
        return true;
    }

    i32 ResourceStorage::GetAmount(ResourceType type) const
    {
        auto it = mStorages.find(type);
        return it != mStorages.end() ? it->second : 0;
    }

    i32 ResourceStorage::GetUsedSpace() const
    {
        i32 used = 0;
        for (const auto& [type, amount] : mStorages)
        {
            used += ResourceTypeToSpace(type) * amount;
        }
        return used;
    }
} // namespace RealmFortress
