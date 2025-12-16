/**
 * @file building.h
 * @brief
 * @date 12/15/2025
 */

#pragma once

#include "core/base.h"
#include "core/timestep.h"
#include "game/resource/resource.h"
#include "game/system/coordinate.h"
#include "game/system/map.h"
#include "renderer/model.h"

namespace RealmFortress
{
    enum class BuildingType
    {
        LumberMill,
        Mine,
        Farm,

        Count,
    };

    enum class BuildingCategory : u8
    {
        Production,
        Military,
        Utility,
        Residential,

        Count
    };

    struct BuildingDefinition
    {
        const char*      Name;
        const char*      Description;
        BuildingCategory Category;
        ResourceCost     ConstructionCost;
        f32              ConstructionTime;
        const char*      ModelPath;
    };

    inline const BuildingDefinition& GetBuildingDefinition(BuildingType type)
    {
        static const BuildingDefinition definitions[] = {
            {
                "Lumber Mill",
                "Produces lumber from nearby trees",
                BuildingCategory::Production,
                { { ResourceType::Lumber, 10 }, { ResourceType::Stone, 5 } },
                5.0f,
                "assets/objects/buildings/blue/building_lumbermill_blue.gltf"
            },
            {
                "Mine",
                "Extracts stone from the ground",
                BuildingCategory::Production,
                { { ResourceType::Lumber, 15 }, { ResourceType::Stone, 3 } },
                8.0f,
                "assets/objects/buildings/blue/building_mine_blue.gltf"
            },
            {
                "Farm",
                "Grows food for your settlement",
                BuildingCategory::Production,
                { { ResourceType::Lumber, 8 }, { ResourceType::Stone, 2 } },
                4.0f,
                "assets/objects/buildings/blue/building_farm_blue.gltf"
            },
        };
        return definitions[static_cast<size_t>(type)];
    }

    inline const char* BuildingCategoryToString(BuildingCategory category)
    {
        switch (category)
        {
        case BuildingCategory::Production:  return "Production";
        case BuildingCategory::Military:    return "Military";
        case BuildingCategory::Utility:     return "Utility";
        case BuildingCategory::Residential: return "Residential";
        default:                            return "Unknown";
        }
    }

    class Building
    {
    public:
        Building(BuildingType type, const Coordinate& coord);
        virtual ~Building() = default;

        virtual void OnUpdate(Timestep ts) = 0;
        virtual void OnPlaced() = 0;
        virtual void OnDestroyed() {};

        virtual bool CanPlace(const Coordinate& coord, const Map& map) const = 0;

        const Coordinate& GetCoordinate() const { return mCoord; }
        BuildingType GetType() const { return mType; }
        const BuildingDefinition& GetDefinition() const { return GetBuildingDefinition(mType); }
        BuildingCategory GetCategory() const { return GetDefinition().Category; }

        void SetModel(const Ref<Model>& model) { mModel = model; }
        Ref<Model> GetModel() const { return mModel; }
        glm::mat4 GetTransform() const;

        bool IsActive() const { return mActive; }
        void SetActive(bool active) { mActive = active; }

    protected:
        BuildingType mType;
        Coordinate mCoord;
        Ref<Model> mModel;
        bool mActive{ true };
    };
} // namespace RealmFortress
