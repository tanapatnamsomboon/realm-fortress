/**
 * @file building.h
 * @brief
 * @date 11/29/2025
 */

#pragma once

#include "core/base.h"
#include "game/hex/hex_coordinate.h"
#include "game/resource/resource_type.h"
#include "game/faction.h"
#include "renderer/model.h"
#include <string>
#include <vector>

namespace RealmFortress
{
    enum class BuildingType
    {
        None = 0,

        // Civilian buildings
        Townhall,
        Home,
        Lumbermill,
        Mine,
        Farm,
        Windmill,
        Watermill,
        Market,
        Tavern,
        Well,
        Workshop,

        // Military buildings
        Barracks,
        ArcheryRange,
        Stables,
        Blacksmith,
        Tower,
        Watchtower,
        Castle,

        // Naval
        Shipyard,
        Docks,

        // Religious
        Church,
        Shrine,

        Count
    };

    inline const char* BuildingTypeToString(BuildingType type)
    {
        switch (type)
        {
        case BuildingType::Townhall:     return "Town Hall";
        case BuildingType::Home:         return "Home";
        case BuildingType::Lumbermill:   return "Lumbermill";
        case BuildingType::Mine:         return "Mine";
        case BuildingType::Farm:         return "Farm";
        case BuildingType::Windmill:     return "Windmill";
        case BuildingType::Watermill:    return "Watermill";
        case BuildingType::Market:       return "Market";
        case BuildingType::Tavern:       return "Tavern";
        case BuildingType::Well:         return "Well";
        case BuildingType::Workshop:     return "Workshop";
        case BuildingType::Barracks:     return "Barracks";
        case BuildingType::ArcheryRange: return "Archery Range";
        case BuildingType::Stables:      return "Stables";
        case BuildingType::Blacksmith:   return "Blacksmith";
        case BuildingType::Tower:        return "Tower";
        case BuildingType::Watchtower:   return "Watchtower";
        case BuildingType::Castle:       return "Castle";
        case BuildingType::Shipyard:     return "Shipyard";
        case BuildingType::Docks:        return "Docks";
        case BuildingType::Church:       return "Church";
        case BuildingType::Shrine:       return "Shrine";
        default: return "None";
        }
    }

    struct BuildingCost
    {
        std::vector<ResourceAmount> Resources;
        f32 BuildTime = 0.0f; // seconds

        BuildingCost() = default;
        BuildingCost(std::initializer_list<ResourceAmount> resources, f32 buildingTime)
            : Resources(resources), BuildTime(buildingTime)
        {
        }
    };

    struct BuildingDefinition
    {
        BuildingType Type = BuildingType::None;
        std::string  Name;
        std::string  Description;
        std::string  ModelPath;
        BuildingCost Cost;
        ResourceProduction Production;
        i32 PopulationProvided = 0;

        BuildingDefinition() = default;
        BuildingDefinition(BuildingType type, const std::string& name,
                           const std::string& desc, const std::string& model,
                           const BuildingCost& cost,
                           const ResourceProduction& production = {},
                           i32 population = 0)
                               : Type(type), Name(name), Description(desc), ModelPath(model)
                               , Cost(cost), Production(production), PopulationProvided(population)
        {
        }

        std::string GetModelPath(Faction faction) const;
    };

    class Building
    {
    public:
        Building(
            BuildingType              type,
            const HexCoordinate&      position,
            Faction                   faction,
            const BuildingDefinition& definition);
        ~Building();

        BuildingType GetType() const { return mType; }
        Faction GetFaction() const { return mFaction; }
        const HexCoordinate& GetPosition() const { return mPosition; }
        const std::string& GetName() const { return mDefinition.Name; }
        BuildingDefinition GetDefinition() const { return mDefinition; }

        void Update(Timestep ts);
        void Draw(const Ref<Shader>& shader);

        bool IsConstructed() const { return mConstructed; }
        f32 GetConstructionProgress() const { return mConstructionProgress; }

        void SetConstructed(bool constructed) { mConstructed = constructed; }

        f32 GetRotation() const { return mRotation; }
        void SetRotation(f32 rotation) { mRotation = rotation; }

        std::string GetProductionId() const;

    private:
        BuildingType mType;
        Faction mFaction;
        HexCoordinate mPosition;
        BuildingDefinition mDefinition;
        Ref<Model> mModel;
        bool mConstructed;
        f32 mConstructionProgress;
        f32 mRotation; // degrees
    };

    class BuildingManager
    {
    public:
        BuildingManager();

        void Initialize();
        void Update(Timestep ts);

        const BuildingDefinition* GetBuildingDefinition(BuildingType type) const;
        const std::vector<BuildingDefinition>& GetAllDefinitions() const { return mDefinitions; }

        bool CanPlaceBuilding(BuildingType type, const HexCoordinate& position) const;
        Building* PlaceBuilding(
            BuildingType         type,
            const HexCoordinate& position,
            Faction              faction,
            ResourceManager&     resources);
        void RemoveBuilding(const HexCoordinate& position);

        Building* GetBuildingAt(const HexCoordinate& position);
        const Building* GetBuildingAt(const HexCoordinate& position) const;

        const std::vector<Scope<Building>>& GetAllBuildings() const { return mBuildings; }
        std::vector<Building*> GetBuildingOfFaction(Faction faction);

        void DrawBuildings(const Ref<Shader>& shader);

        void SetResourceManager(ResourceManager* resourceManager) { mResourceManager = resourceManager; }

    private:
        std::vector<BuildingDefinition> mDefinitions;
        std::vector<Scope<Building>> mBuildings;
        std::unordered_map<HexCoordinate, Building*> mBuildingMap;
        ResourceManager* mResourceManager = nullptr;
    };
} // namespace RealmFortress
