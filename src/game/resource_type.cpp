/**
 * @file resource_type.cpp
 * @brief
 * @date 11/29/2025
 */

#include "core/pch.h"
#include "resource_type.h"
#include "core/logger.h"

namespace RF
{
    ResourceManager::ResourceManager()
    {
        mResources[ResourceType::Wood] = 100;
        mResources[ResourceType::Stone] = 50;
        mResources[ResourceType::Food] = 200;
        mResources[ResourceType::Gold] = 10;
        mResources[ResourceType::Iron] = 20;
    }

    void ResourceManager::AddResource(ResourceType type, i32 amount)
    {
        if (type == ResourceType::None) return;

        mResources[type] += amount;
        RF_TRACE("Added {0} {1}. Total: {2}", amount, ResourceTypeToString(type), mResources[type]);
    }

    bool ResourceManager::RemoveResource(ResourceType type, i32 amount)
    {
        if (type == ResourceType::None) return false;

        if (!HasResource(type, amount))
        {
            RF_WARN("Not enough {0}! Need: {1}, Have: {2}", ResourceTypeToString(type), amount, mResources[type]);
            return false;
        }

        mResources[type] -= amount;
        RF_TRACE("Removed {0} {1}. Remaining: {2}", amount, ResourceTypeToString(type), mResources[type]);
        return true;
    }

    bool ResourceManager::HasResource(ResourceType type, i32 amount) const
    {
        if (type == ResourceType::None) return true;

        auto it = mResources.find(type);
        if (it == mResources.end()) return false;

        return it->second >= amount;
    }

    i32 ResourceManager::GetResourceAmount(ResourceType type) const
    {
        auto it = mResources.find(type);
        if (it == mResources.end()) return 0;
        return it->second;
    }

    void ResourceManager::SetResource(ResourceType type, i32 amount)
    {
        if (type == ResourceType::None) return;
        mResources[type] = amount;
    }
} // namespace RF
