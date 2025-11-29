/**
 * @file resource_type.h
 * @brief
 * @date 11/29/2025
 */

#pragma once

#include "core/base.h"
#include <string>
#include <unordered_map>

namespace RF
{
    enum class ResourceType
    {
        None = 0,
        Wood,
        Stone,
        Food,
        Gold,
        Iron
    };

    inline const char* ResourceTypeToString(ResourceType type)
    {
        switch (type)
        {
        case ResourceType::Wood:  return "Wood";
        case ResourceType::Stone: return "Stone";
        case ResourceType::Food:  return "Food";
        case ResourceType::Gold:  return "Gold";
        case ResourceType::Iron:  return "Iron";
        default: return "None";
        }
    }

    struct ResourceAmount
    {
        ResourceType Type;
        i32 Amount;

        ResourceAmount(ResourceType type = ResourceType::None, i32 amount = 0)
            : Type(type), Amount(amount)
        {
        }
    };

    class ResourceManager
    {
    public:
        ResourceManager();

        void AddResource(ResourceType type, i32 amount);
        bool RemoveResource(ResourceType type, i32 amount);
        bool HasResource(ResourceType type, i32 amount) const;

        i32 GetResourceAmount(ResourceType type) const;
        void SetResource(ResourceType type, i32 amount);

        const std::unordered_map<ResourceType, i32>& GetAllResources() const { return mResources; }

    private:
        std::unordered_map<ResourceType, i32> mResources;
    };

} // namespace RF
