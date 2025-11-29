/**
 * @file building.h
 * @brief
 * @date 11/29/2025
 */

#pragma once

#include "core/base.h"
#include "game/hex_coordinate.h"
#include "game/resource_type.h"
#include "renderer/model.h"
#include <string>
#include <vector>

namespace RF
{
    enum class BuildingType
    {
        None = 0,
        Home,
        Lumbermill,
        Mine,
        Farm,
        Market,
        Barracks,
        Tower,
    };

    inline const char* BuildingTypeToString(BuildingType type)
    {
        switch (type)
        {
        case BuildingType::Home:       return "Home";
        case BuildingType::Lumbermill: return "Lumbermill";
        case BuildingType::Mine:       return "Mine";
        case BuildingType::Farm:       return "Farm";
        case BuildingType::Market:     return "Market";
        case BuildingType::Barracks:   return "Barracks";
        case BuildingType::Tower:      return "Tower";
        default: return "None";
        }
    }

    struct BuildingCost
    {
        std::vector<ResourceAmount> Resources;

        BuildingCost() = default;
        BuildingCost(std::initializer_list<ResourceAmount> resources)
            : Resources(resources) {}
    };

    struct BuildingDefinition
    {
        BuildingType Type;
        std::string  Name;
        std::string  Description;
        std::string  ModelPath;
        BuildingCost Cost;

        BuildingDefinition() = default;
        BuildingDefinition(BuildingType type, const std::string& name,
                           const std::string& desc, const std::string& model,
                           const BuildingCost& cost)
                               : Type(type), Name(name), Description(desc), ModelPath(model), Cost(cost)
        {
        }
    };

    class Building
    {
    public:
        Building(BuildingType type, const HexCoordinate& position, const BuildingDefinition& definition);

        BuildingType GetType() const { return mType; }
        const HexCoordinate& GetPosition() const { return mPosition; }
        const std::string& GetName() const { return mDefinition.Name; }

        void Draw(const Ref<Shader>& shader);

        bool IsConstructed() const { return mConstructed; }
        void SetConstructed(bool constructed) { mConstructed = constructed; }

        f32 GetRotation() const { return mRotation; }
        void SetRotation(f32 rotation) { mRotation = rotation; }

    private:
        BuildingType mType;
        HexCoordinate mPosition;
        BuildingDefinition mDefinition;
        Ref<Model> mModel;
        bool mConstructed;
        f32 mRotation; // degrees
    };

    class BuildingManager
    {
    public:
        BuildingManager();

        void Initialize();

        const BuildingDefinition* GetBuildingDefinition(BuildingType type) const;
        const std::vector<BuildingDefinition>& GetAllDefinitions() const { return mDefinitions; }

        bool CanPlaceBuilding(BuildingType type, const HexCoordinate& position) const;
        Building* PlaceBuilding(BuildingType type, const HexCoordinate& position, ResourceManager& resources);
        void RemoveBuilding(const HexCoordinate& position);

        Building* GetBuildingAt(const HexCoordinate& position);
        const Building* GetBuildingAt(const HexCoordinate& position) const;

        const std::vector<Scope<Building>>& GetAllBuildings() const { return mBuildings; }

        void DrawBuildings(const Ref<Shader>& shader);

    private:
        std::vector<BuildingDefinition> mDefinitions;
        std::vector<Scope<Building>> mBuildings;
        std::unordered_map<HexCoordinate, Building*> mBuildingMap;
    };
} // namespace RF
