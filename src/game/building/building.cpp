/**
 * @file building.cpp
 * @brief
 * @date 12/12/2025
 */

#include "core/pch.h"
#include "building.h"

namespace RealmFortress
{
    inline const char* BuildingTypeToToken(BuildingType type)
    {
        switch (type)
        {
        case BuildingType::TownHall:      return "townhall";
        case BuildingType::Castle:        return "castle";
        case BuildingType::HomeA:         return "home_A";
        case BuildingType::HomeB:         return "home_B";
        case BuildingType::Barracks:      return "barracks";
        case BuildingType::ArcheryRange:  return "archeryrange";
        case BuildingType::Blacksmith:    return "blacksmith";
        case BuildingType::Church:        return "church";
        case BuildingType::Docks:         return "docks";
        case BuildingType::LumberMill:    return "lumbermill";
        case BuildingType::Market:        return "market";
        case BuildingType::Mine:          return "mine";
        case BuildingType::Shipyard:      return "shipyard";
        case BuildingType::Shrine:        return "shrine";
        case BuildingType::Stables:       return "stables";
        case BuildingType::Tavern:        return "tavern";
        case BuildingType::Tent:          return "tent";
        case BuildingType::Watchtower:    return "watchtower";
        case BuildingType::Watermill:     return "watermill";
        case BuildingType::Well:          return "well";
        case BuildingType::Windmill:      return "windmill";
        case BuildingType::Workshop:      return "workshop";

        case BuildingType::TowerA:        return "tower_A";
        case BuildingType::TowerB:        return "tower_B";
        case BuildingType::TowerBase:     return "tower_base";
        case BuildingType::TowerCannon:   return "tower_cannon";
        case BuildingType::TowerCatapult: return "tower_catapult";
        default:                          return nullptr;
        }
    }

    inline const char* FactionToFolder(Faction faction)
    {
        switch (faction)
        {
        case Faction::Blue:   return "blue";
        case Faction::Red:    return "red";
        case Faction::Green:  return "green";
        case Faction::Yellow: return "yellow";
        default:              return "neutral";
        }
    }

    Building::Building(BuildingType type, Faction faction, const Coordinate& coord)
        : mType(type), mFaction(faction), mCoordinate(coord)
    {
    }

    glm::mat4 Building::GetTransform() const
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, mCoordinate.ToWorldPosition());
        return transform;
    }

    const char* BuildingTypeToString(BuildingType type)
    {
        switch (type)
        {
        case BuildingType::None:         return "None";
        case BuildingType::TownHall:     return "Town Hall";
        case BuildingType::Castle:       return "Castle";
        case BuildingType::HomeA:        return "Home A";
        case BuildingType::HomeB:        return "Home B";
        case BuildingType::Barracks:     return "Barracks";
        case BuildingType::ArcheryRange: return "Archery Range";
        case BuildingType::Blacksmith:   return "Blacksmith";
        case BuildingType::Church:       return "Church";
        case BuildingType::Docks:        return "Docks";
        case BuildingType::LumberMill:   return "Lumber Mill";
        case BuildingType::Market:       return "Market";
        case BuildingType::Mine:         return "Mine";
        case BuildingType::Shipyard:     return "Shipyard";
        case BuildingType::Shrine:       return "Shrine";
        case BuildingType::Stables:      return "Stables";
        case BuildingType::Tavern:       return "Tavern";
        case BuildingType::Tent:         return "Tent";
        case BuildingType::Watchtower:   return "Watchtower";
        case BuildingType::Watermill:    return "Watermill";
        case BuildingType::Well:         return "Well";
        case BuildingType::Windmill:     return "Windmill";
        case BuildingType::Workshop:     return "Workshop";

        case BuildingType::TowerA:       return "Tower A";
        case BuildingType::TowerB:       return "Tower B";
        case BuildingType::TowerBase:    return "Tower Base";
        case BuildingType::TowerCannon:  return "Tower Cannon";
        case BuildingType::TowerCatapult:return "Tower Catapult";
        default:                         return "Unknown";
        }
    }

    const char* FactionToString(Faction faction)
    {
        switch (faction)
        {
        case Faction::Neutral: return "Neutral";
        case Faction::Blue:    return "Blue";
        case Faction::Red:     return "Red";
        case Faction::Green:   return "Green";
        case Faction::Yellow:  return "Yellow";
        default:               return "Unknown";
        }
    }

    std::string BuildingTypeToModelPath(BuildingType type, Faction faction)
    {
        const char* faction_str = FactionToFolder(faction);
        const char* token = BuildingTypeToToken(type);

        if (!token)
            token = "townhall";

        std::string base = "assets/objects/buildings/";
        base += faction_str;
        base += "/building_";
        base += token;
        base += "_";
        base += faction_str;
        base += ".gltf";
        return base;
    }
} // namespace RealmFortress
