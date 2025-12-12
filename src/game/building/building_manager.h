/**
 * @file building_manager.h
 * @brief
 * @date 12/12/2025
 */

#pragma once

#include "core/base.h"
#include "game/building/building.h"
#include "game/system/coordinate.h"
#include "game/system/map.h"
#include "renderer/shader.h"
#include <unordered_map>
#include <optional>

namespace RealmFortress
{
    class BuildingManager
    {
    public:
        BuildingManager() = default;

        bool CanPlaceBuilding(const Coordinate& coord, const Map& map) const;
        bool PlaceBuilding(BuildingType type, Faction faction, const Coordinate& coord, Map& map);
        bool RemoveBuilding(const Coordinate& coord);

        Building* GetBuilding(const Coordinate& coord);
        const Building* GetBuilding(const Coordinate& coord) const;
        bool HasBuilding(const Coordinate& coord) const;

        void Draw(const Ref<Shader>& shader) const;

        void SetPreview(BuildingType type, Faction faction, const Coordinate& coord);
        void ClearPreview();
        bool HasPreview() const { return mPreviewBuilding.has_value(); }
        void DrawPreview(const Ref<Shader>& shader, const Map& map, f32 time) const;

        usize GetBuildingCount() const { return mBuildings.size(); }

    private:
        Ref<Model> GetOrLoadModel(BuildingType type, Faction faction) const;
        static u64 MakeKey(BuildingType type, Faction faction);

    private:
        std::unordered_map<Coordinate, Building> mBuildings;
        std::optional<Building> mPreviewBuilding;

        mutable std::unordered_map<u64, Ref<Model>> mBuildingModelCache;
    };
} // namespace RealmFortress
