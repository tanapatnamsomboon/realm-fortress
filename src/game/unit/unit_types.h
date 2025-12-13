/**
 * @file unit_types.h
 * @brief
 * @date 12/14/2025
 */

#pragma once

#include "game/building/building.h"
#include <string>

namespace RealmFortress
{
    enum class UnitType : u8
    {
        None = 0,

        Soldier,
        Spearman,
        Swordsman,
        Archer,

        Knight,
        HorseArcher,

        Catapult,
        Cannon,

        Peasant,
        MerchantCart,

        Ship,

        Count
    };

    enum class UnitState : u8
    {
        Idle,
        Moving,
        Attacking,
        Gathering,
        Dead
    };

    inline std::string UnitTypeToModelPath(UnitType type, Faction faction)
    {
        std::string factionStr;
        switch (faction)
        {
        case Faction::Blue:   factionStr = "blue"; break;
        case Faction::Red:    factionStr = "red"; break;
        case Faction::Green:  factionStr = "green"; break;
        case Faction::Yellow: factionStr = "yellow"; break;
        default:              factionStr = "neutral"; break;
        }

        std::string base = "assets/objects/units/" + factionStr + "/";

        switch (type)
        {
        case UnitType::Soldier:      return base + "unit_" + factionStr + "_ascent.gltf";
        case UnitType::Spearman:     return base + "spear_" + factionStr + "_ascent.gltf";
        case UnitType::Swordsman:    return base + "sword_" + factionStr + "_ascent.gltf";
        case UnitType::Archer:       return base + "bow_" + factionStr + "_ascent.gltf";
        case UnitType::Knight:       return base + "horse_" + factionStr + "_ascent.gltf";
        case UnitType::Catapult:     return base + "catapult_" + factionStr + "_ascent.gltf";
        case UnitType::Cannon:       return base + "cannon_" + factionStr + "_ascent.gltf";
        case UnitType::MerchantCart: return base + "cart_merchant_" + factionStr + "_ascent.gltf";
        case UnitType::Ship:         return base + "ship_" + factionStr + "_ascent.gltf";
        default:                     return base + "unit_" + factionStr + "_ascent.gltf";
        }
    }

    inline const char* UnitTypeToString(UnitType type)
    {
        switch (type)
        {
        case UnitType::Soldier:      return "Soldier";
        case UnitType::Spearman:     return "Spearman";
        case UnitType::Swordsman:    return "Swordsman";
        case UnitType::Archer:       return "Archer";
        case UnitType::Knight:       return "Knight";
        case UnitType::HorseArcher:  return "Horse Archer";
        case UnitType::Catapult:     return "Catapult";
        case UnitType::Cannon:       return "Cannon";
        case UnitType::Peasant:      return "Peasant";
        case UnitType::MerchantCart: return "Merchant";
        case UnitType::Ship:         return "Ship";
        default:                     return "Unknown";
        }
    }

    inline const char* UnitStateToString(UnitState state)
    {
        switch (state)
        {
        case UnitState::Idle:      return "Idle";
        case UnitState::Moving:    return "Moving";
        case UnitState::Attacking: return "Attacking";
        case UnitState::Gathering: return "Gathering";
        case UnitState::Dead:      return "Dead";
        default:                   return "Unknown";
        }
    }
} // namespace RealmFortress
