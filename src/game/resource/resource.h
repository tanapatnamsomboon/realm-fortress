/**
 * @file resource.h
 * @brief
 * @date 12/16/2025
 */

#pragma once

#include "core/base.h"
#include <unordered_map>

namespace RealmFortress
{
    enum class ResourceType : u8
    {
        Lumber,
        Stone,
        Wheat,

        Count
    };

    using ResourceCost = std::unordered_map<ResourceType, i32>;

    inline const char* ResourceTypeToString(ResourceType type)
    {
        switch (type)
        {
        case ResourceType::Lumber: return "Lumber";
        case ResourceType::Stone:  return "Stone";
        case ResourceType::Wheat:  return "Wheat";
        default:                   return "Unknown";
        }
    }

    inline i32 ResourceTypeToSpace(ResourceType type)
    {
        switch (type)
        {
        case ResourceType::Lumber: return 2;
        case ResourceType::Stone:  return 3;
        case ResourceType::Wheat:  return 1;
        default:                   return 0;
        }
    }

    inline const char* ResourceTypeToThumbnailModelPath(ResourceType type)
    {
        switch (type)
        {
        case ResourceType::Lumber: return "assets/objects/decoration/props/resource_lumber.gltf";
        case ResourceType::Stone:  return "assets/objects/decoration/props/resource_stone.gltf";
        case ResourceType::Wheat:  return "assets/objects/decoration/props/resource_stone.gltf";
        default:                   return "";
        }
    }

    class ResourceStorage
    {
    public:
        ResourceStorage();

        void Add(ResourceType type, i32 amount);
        void Add(const ResourceCost& cost);

        bool Consume(ResourceType type, i32 amount);
        bool Consume(const ResourceCost& cost);

        bool Has(ResourceType type, i32 amount) const;
        bool Has(const ResourceCost& cost) const;

        i32 GetAmount(ResourceType type) const;
        i32 GetUsedSpace() const;

        const auto& GetAll() const { return mStorages; }

    private:
        std::unordered_map<ResourceType, i32> mStorages;
    };
} // namespace RealmFortress
