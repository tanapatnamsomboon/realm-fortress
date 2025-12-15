/**
 * @file building_manager.h
 * @brief
 * @date 12/16/2025
 */

#pragma once

#include "game/building/building.h"

namespace RealmFortress
{
    class BuildingManager
    {
    public:
        static BuildingManager& Get()
        {
            static BuildingManager instance;
            return instance;
        }

        BuildingManager(const BuildingManager&) = delete;
        BuildingManager& operator=(const BuildingManager&) = delete;

        void OnUpdate(Timestep ts);

        bool CanPlaceBuilding(BuildingType type, const Coordinate& coord, const Map& map) const;
        bool PlaceBuilding(BuildingType type, const Coordinate& coord, const Map& map);
        bool RemoveBuilding(const Coordinate& coord);

        Building* GetBuildingAt(const Coordinate& coord) const;
        const std::vector<Scope<Building>>& GetAllBuildings() const { return mBuildings; }

        std::vector<Building*> GetBuildingsByType(BuildingType type) const;
        i32 GetBuildingCount(BuildingType type) const;

        std::vector<Building*> GetBuildingsInRadius(const Coordinate& center, i32 radius) const;

        void Clear();

    private:
        BuildingManager() = default;

        Scope<Building> CreateBuilding(BuildingType type, const Coordinate& coord);

    private:
        std::vector<Scope<Building>> mBuildings;

        std::unordered_map<Coordinate, usize> mCoordinateMap;
    };
} // namespace RealmFortress
