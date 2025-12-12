/**
 * @file building.cpp
 * @brief
 * @date 12/12/2025
 */

#include "core/pch.h"
#include "building.h"

namespace RealmFortress
{
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
        case BuildingType::House:        return "House";
        case BuildingType::Barracks:     return "Barracks";
        case BuildingType::ArcheryRange: return "Archery Range";
        case BuildingType::Stables:      return "Stables";
        case BuildingType::Farm:         return "Farm";
        case BuildingType::LumberMill:   return "Lumber Mill";
        case BuildingType::Mine:         return "Mine";
        case BuildingType::Market:       return "Market";
        case BuildingType::Wall:         return "Wall";
        case BuildingType::Tower:        return "Tower";
        case BuildingType::Gate:         return "Gate";
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
        std::string faction_str;

        switch (faction)
        {
        case Faction::Blue:   faction_str = "blue"; break;
        case Faction::Red:    faction_str = "red"; break;
        case Faction::Green:  faction_str = "green"; break;
        case Faction::Yellow: faction_str = "yellow"; break;
        default:              faction_str = "neutral"; break;
        }

        std::string base = "assets/objects/buildings/" + faction_str + "/building_";

        switch (type)
        {
        case BuildingType::TownHall:     return base + "townhall_" + faction_str + ".gltf";
        case BuildingType::Castle:       return base + "castle_" + faction_str + ".gltf";
        case BuildingType::House:        return base + "home_A_" + faction_str + ".gltf";
        case BuildingType::Barracks:     return base + "barracks_" + faction_str + ".gltf";
        case BuildingType::ArcheryRange: return base + "archery_" + faction_str + ".gltf";
        case BuildingType::Stables:      return base + "stables_" + faction_str + ".gltf";
        case BuildingType::LumberMill:   return base + "lumbermill_" + faction_str + ".gltf";
        case BuildingType::Mine:         return base + "mine_" + faction_str + ".gltf";
        case BuildingType::Market:       return base + "market_" + faction_str + ".gltf";
        case BuildingType::Tower:        return base + "tower_A_" + faction_str + ".gltf";
        default:                         return "assets/objects/buildings/" + faction_str + "/building_townhall_" + faction_str + ".gltf";
        }
    }
} // namespace RealmFortress
