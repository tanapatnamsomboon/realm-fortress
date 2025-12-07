/**
 * @file resource_type.cpp
 * @brief
 * @date 11/29/2025
 */

#include "core/pch.h"
#include "resource_type.h"
#include "core/logger.h"

namespace RealmFortress
{
    ResourceManager::ResourceManager()
    {
        // Initial resources
        mResources[ResourceType::Wood]          = 200;
        mResources[ResourceType::Stone]         = 100;
        mResources[ResourceType::Food]          = 300;
        mResources[ResourceType::Gold]          = 50;
        mResources[ResourceType::Iron]          = 30;
        mResources[ResourceType::Population]    = 0;
        mResources[ResourceType::PopulationCap] = 0;
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

    bool ResourceManager::CanAfford(const std::vector<ResourceAmount>& cost) const
    {
        for (const auto& res : cost)
        {
            if (!HasResource(res.Type, res.Amount))
                return false;
        }
        return true;
    }

    bool ResourceManager::SpendResources(const std::vector<ResourceAmount>& cost)
    {
        if (!CanAfford(cost))
            return false;

        for (const auto& res : cost)
        {
            RemoveResource(res.Type, res.Amount);
        }
        return true;
    }

    bool ResourceManager::CanAddPopulation(i32 amount) const
    {
        i32 current = GetResourceAmount(ResourceType::Population);
        i32 cap = GetResourceAmount(ResourceType::Population);
        return current + amount <= cap;
    }

    void ResourceManager::AddPopulation(i32 amount)
    {
        if (CanAddPopulation(amount))
        {
            AddResource(ResourceType::Population, amount);
        }
        else
        {
            RF_WARN("Cannot add population: Would exceed cap!");
        }
    }

    void ResourceManager::RemovePopulation(i32 amount)
    {
        i32 current = GetPopulation();
        i32 remove = std::min(amount, current);
        RemoveResource(ResourceType::Population, remove);
    }

    void ResourceManager::AddPopulationCap(i32 amount)
    {
        AddResource(ResourceType::PopulationCap, amount);
        RF_INFO("Population cap increased by {} (New cap: {})", amount, GetPopulationCap());
    }

    void ResourceManager::RemovePopulationCap(i32 amount)
    {
        i32 current = GetPopulationCap();
        i32 remove = std::min(amount, current);
        RemoveResource(ResourceType::PopulationCap, remove);

        if (GetPopulation() > GetPopulationCap())
        {
            i32 excess = GetPopulation() - GetPopulationCap();
            RemovePopulation(excess);
            RF_WARN("Population reduced by {} to fit new cap", excess);
        }
    }

    void ResourceManager::Update(Timestep ts)
    {
        mProductionTimer += static_cast<f32>(ts);

        if (mProductionTimer >= mProductionInterval)
        {
            ProcessProduction();
            mProductionTimer = 0.0f;
        }
    }

    void ResourceManager::RegisterProducer(const std::string& id, const ResourceProduction& production)
    {
        mProducers[id] = production;
        RF_TRACE("Registered producer {}", id);
    }

    void ResourceManager::UnregisterProducer(const std::string& id)
    {
        mProducers.erase(id);
        RF_TRACE("Unregistered producer {}", id);
    }

    i32 ResourceManager::GetProductionRate(ResourceType type) const
    {
        i32 total = 0;
        for (const auto& production : mProducers | std::views::values)
        {
            for (const auto& produce : production.Production)
            {
                if (produce.Type == type)
                    total += produce.Amount;
            }
        }

        return total;
    }

    i32 ResourceManager::GetConsumptionRate(ResourceType type) const
    {
        i32 total = 0;
        for (const auto& production : mProducers | std::views::values)
        {
            for (const auto& consume : production.Consumption)
            {
                if (consume.Type == type)
                    total += consume.Amount;
            }
        }

        return total;
    }

    void ResourceManager::ProcessProduction()
    {
        for (const auto& production : mProducers | std::views::values)
        {
            bool canProduce = true;
            for (const auto& consume : production.Consumption)
            {
                if (!HasResource(consume.Type, consume.Amount))
                {
                    canProduce = false;
                    break;
                }
            }

            if (!canProduce)
                continue;

            for (const auto& consume : production.Consumption)
            {
                RemoveResource(consume.Type, consume.Amount);
            }

            for (const auto& produce : production.Production)
            {
                AddResource(produce.Type, produce.Amount);
            }
        }
    }
} // namespace RealmFortress
