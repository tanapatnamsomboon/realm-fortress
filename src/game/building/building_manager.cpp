/**
 * @file building_manager.cpp
 * @brief
 * @date 12/16/2025
 */

#include "core/pch.h"
#include "building_manager.h"
#include "renderer/model_cache.h"
#include "game/building/townhall.h"
#include "game/building/mine.h"
#include "game/building/lumbermill.h"
#include "game/building/farm.h"
#include "game/resource/warehouse.h"

namespace RealmFortress
{
    void BuildingManager::OnUpdate(Timestep ts)
    {
        for (auto& building : mBuildings)
        {
            if (building && building->IsActive())
            {
                building->OnUpdate(ts);
            }
        }
    }

    bool BuildingManager::CanPlaceBuilding(BuildingType type, const Coordinate& coord, const Map& map) const
    {
        if (!map.HasTile(coord)) return false;
        if (GetBuildingAt(coord) != nullptr) return false;

        i32 townhall_count = GetBuildingCount(BuildingType::Townhall);

        if (type == BuildingType::Townhall)
        {
            if (townhall_count > 0)
            {
                RF_CORE_WARN("Cannot build more than one Town Hall!");
                return false;
            }
        }
        else
        {
            if (townhall_count == 0)
            {
                RF_CORE_WARN("Must build Town Hall first!");
                return false;
            }
        }

        const auto& definition = GetBuildingDefinition(type);
        if (!Warehouse::Get().Has(definition.ConstructionCost))
        {
            return false;
        }

        auto temp_building = const_cast<BuildingManager*>(this)->CreateBuilding(type, coord);
        if (!temp_building)
        {
            return false;
        }

        return temp_building->CanPlace(coord, map);
    }

    bool BuildingManager::PlaceBuilding(BuildingType type, const Coordinate& coord, const Map& map)
    {
        if (!CanPlaceBuilding(type, coord, map))
        {
            return false;
        }

        const auto& definition = GetBuildingDefinition(type);
        if (!Warehouse::Get().Consume(definition.ConstructionCost))
        {
            return false;
        }

        auto building = CreateBuilding(type, coord);
        if (!building)
        {
            // refund
            Warehouse::Get().Add(definition.ConstructionCost);
            return false;
        }

        auto model = ModelCache::Load(definition.ModelPath);
        if (model)
        {
            building->SetModel(model);
        }
        else
        {
            RF_CORE_WARN("Failed to load model for {}: {}", definition.Name, definition.ModelPath);
        }

        building->OnPlaced(map);

        mCoordinateMap[coord] = mBuildings.size();
        mBuildings.push_back(std::move(building));

        return true;
    }

    bool BuildingManager::RemoveBuilding(const Coordinate& coord, const Map& map)
    {
        auto it = mCoordinateMap.find(coord);

        if (it == mCoordinateMap.end())
        {
            return false;
        }

        usize index = it->second;
        if (index >= mBuildings.size())
        {
            return false;
        }

        if (mBuildings[index])
        {
            mBuildings[index]->OnDestroyed(map);
        }

        if (index != mBuildings.size() - 1)
        {
            mBuildings[index] = std::move(mBuildings.back());

            const Coordinate& swapped_coord = mBuildings[index]->GetCoordinate();
            mCoordinateMap[swapped_coord] = index;
        }

        mBuildings.pop_back();
        mCoordinateMap.erase(it);

        return true;
    }

    Building* BuildingManager::GetBuildingAt(const Coordinate& coord) const
    {
        auto it = mCoordinateMap.find(coord);

        if (it == mCoordinateMap.end())
        {
            return nullptr;
        }

        usize index = it->second;
        if (index >= mBuildings.size())
        {
            return nullptr;
        }

        return mBuildings[index].get();
    }

    std::vector<Building*> BuildingManager::GetBuildingsByType(BuildingType type) const
    {
        std::vector<Building*> result;
        result.reserve(mBuildings.size() / 4);

        for (const auto& building : mBuildings)
        {
            if (building && building->GetType() == type)
            {
                result.push_back(building.get());
            }
        }

        return result;
    }

    i32 BuildingManager::GetBuildingCount(BuildingType type) const
    {
        i32 count = 0;
        for (const auto& building : mBuildings)
        {
            if (building && building->GetType() == type)
            {
                count++;
            }
        }
        return count;
    }

    std::vector<Building*> BuildingManager::GetBuildingsInRadius(const Coordinate& center, i32 radius) const
    {
        std::vector<Building*> result;

        for (const auto& building : mBuildings)
        {
            if (!building) continue;

            const Coordinate& building_coord = building->GetCoordinate();
            i32 dist = center.DistanceTo(building_coord);

            if (dist <= radius)
            {
                result.push_back(building.get());
            }
        }

        return result;
    }

    void BuildingManager::Clear()
    {
        mBuildings.clear();
        mCoordinateMap.clear();
    }

    Scope<Building> BuildingManager::CreateBuilding(BuildingType type, const Coordinate& coord)
    {
        switch (type)
        {
        case BuildingType::Townhall:
            return CreateScope<Townhall>(coord);
        case BuildingType::Mine:
            return CreateScope<Mine>(coord);
        case BuildingType::LumberMill:
            return CreateScope<Lumbermill>(coord);
        case BuildingType::Farm:
            return CreateScope<Farm>(coord);

        default:
            return nullptr;
        }
    }
} // namespace RealmFortress
