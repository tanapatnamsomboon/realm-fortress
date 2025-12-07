/**
 * @file resource_type.h
 * @brief
 * @date 11/29/2025
 */

#pragma once

#include "core/base.h"
#include "core/timestep.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace RealmFortress
{
    enum class ResourceType : u8
    {
        None = 0,
        Wood,
        Stone,
        Food,
        Gold,
        Iron,
        Population,
        PopulationCap,

        Count
    };

    inline const char* ResourceTypeToString(ResourceType type)
    {
        switch (type)
        {
        case ResourceType::Wood:          return "Wood";
        case ResourceType::Stone:         return "Stone";
        case ResourceType::Food:          return "Food";
        case ResourceType::Gold:          return "Gold";
        case ResourceType::Iron:          return "Iron";
        case ResourceType::Population:    return "Population";
        case ResourceType::PopulationCap: return "Pop Cap";
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

        bool operator==(const ResourceAmount& other) const
        {
            return Type == other.Type && Amount == other.Amount;
        }
    };

    /**
     * @struct ResourceProduction
     * @brief Defines resource platform/consumption rates per interval
     */
    struct ResourceProduction
    {
        std::vector<ResourceAmount> Production;
        std::vector<ResourceAmount> Consumption;
        f32 Interval = 1.0f; // seconds

        ResourceProduction() = default;
        ResourceProduction(
            std::initializer_list<ResourceAmount> production,
            std::initializer_list<ResourceAmount> consumption = {},
            f32 interval = 1.0f)
                : Production(production), Consumption(consumption), Interval(interval)
        {
        }
    };

    /**
     * @class ResourceManager
     * @brief Manages all resources for a faction/player
     */
    class ResourceManager
    {
    public:
        ResourceManager();

        void AddResource(ResourceType type, i32 amount);
        bool RemoveResource(ResourceType type, i32 amount);
        bool HasResource(ResourceType type, i32 amount) const;

        i32 GetResourceAmount(ResourceType type) const;
        void SetResource(ResourceType type, i32 amount);

        bool CanAfford(const std::vector<ResourceAmount>& cost) const;
        bool SpendResources(const std::vector<ResourceAmount>& cost);

        bool CanAddPopulation(i32 amount) const;
        void AddPopulation(i32 amount);
        void RemovePopulation(i32 amount);
        void AddPopulationCap(i32 amount);
        void RemovePopulationCap(i32 amount);

        i32 GetPopulation() const { return GetResourceAmount(ResourceType::PopulationCap); }
        i32 GetPopulationCap() const { return GetResourceAmount(ResourceType::PopulationCap); }
        i32 GetAvailablePopulation() const { return GetPopulationCap() - GetResourceAmount(ResourceType::PopulationCap); }

        void Update(Timestep ts);
        void RegisterProducer(const std::string& id, const ResourceProduction& production);
        void UnregisterProducer(const std::string& id);

        const std::unordered_map<ResourceType, i32>& GetAllResources() const { return mResources; }

        i32 GetProductionRate(ResourceType type) const;
        i32 GetConsumptionRate(ResourceType type) const;

    private:
        void ProcessProduction();

    private:
        std::unordered_map<ResourceType, i32> mResources;
        std::unordered_map<std::string, ResourceProduction> mProducers;

        f32 mProductionTimer = 0.0f;
        f32 mProductionInterval = 1.0f; // seconds
    };

} // namespace RealmFortress
