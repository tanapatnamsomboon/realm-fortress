/**
 * @file decoration_type.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "core/base.h"

namespace RealmFortress
{
    enum class DecorationType : u8
    {
        None = 0,

        TreeSmall,
        TreeMedium,
        TreeLarge,
        RockA, RockB, RockC, RockD, RockE,
        HillA, HillB, HillC,
        MountainA, MountainB, MountainC,
        Cloud,
        Waterlily,
        Waterplant,

        Barrel, BarrelOpen,
        Crate, CrateSmall, CrateLarge,
        Haybale,
        Sack,
        Pallet,
        Anchor,
        Boat, BoatRack,
        Ladder,
        Target,
        Tent,
        Trough,
        Weaponrack,
        Wheelbarrow,
        Flag,

        Count
    };

    inline std::string DecorationTypeToModelPath(DecorationType type)
    {
        switch (type)
        {
        // Nature
        case DecorationType::TreeSmall:     return "assets/objects/decoration/nature/tree_small.gltf";
        case DecorationType::TreeMedium:    return "assets/objects/decoration/nature/tree_medium.gltf";
        case DecorationType::TreeLarge:     return "assets/objects/decoration/nature/tree_large.gltf";
        case DecorationType::RockA:         return "assets/objects/decoration/nature/rock_A.gltf";
        case DecorationType::RockB:         return "assets/objects/decoration/nature/rock_B.gltf";
        case DecorationType::RockC:         return "assets/objects/decoration/nature/rock_C.gltf";
        case DecorationType::RockD:         return "assets/objects/decoration/nature/rock_D.gltf";
        case DecorationType::RockE:         return "assets/objects/decoration/nature/rock_E.gltf";
        case DecorationType::HillA:         return "assets/objects/decoration/nature/hill_A.gltf";
        case DecorationType::HillB:         return "assets/objects/decoration/nature/hill_B.gltf";
        case DecorationType::HillC:         return "assets/objects/decoration/nature/hill_C.gltf";
        case DecorationType::MountainA:     return "assets/objects/decoration/nature/mountain_A.gltf";
        case DecorationType::MountainB:     return "assets/objects/decoration/nature/mountain_B.gltf";
        case DecorationType::MountainC:     return "assets/objects/decoration/nature/mountain_C.gltf";
        case DecorationType::Cloud:         return "assets/objects/decoration/nature/cloud.gltf";
        case DecorationType::Waterlily:     return "assets/objects/decoration/nature/waterlily.gltf";
        case DecorationType::Waterplant:    return "assets/objects/decoration/nature/waterplant.gltf";

        // Props
        case DecorationType::Barrel:        return "assets/objects/decoration/props/barrel.gltf";
        case DecorationType::BarrelOpen:    return "assets/objects/decoration/props/barrel_open.gltf";
        case DecorationType::Crate:         return "assets/objects/decoration/props/crate.gltf";
        case DecorationType::CrateSmall:    return "assets/objects/decoration/props/crate_small.gltf";
        case DecorationType::CrateLarge:    return "assets/objects/decoration/props/crate_large.gltf";
        case DecorationType::Haybale:       return "assets/objects/decoration/props/haybale.gltf";
        case DecorationType::Sack:          return "assets/objects/decoration/props/sack.gltf";
        case DecorationType::Pallet:        return "assets/objects/decoration/props/pallet.gltf";
        case DecorationType::Anchor:        return "assets/objects/decoration/props/anchor.gltf";
        case DecorationType::Boat:          return "assets/objects/decoration/props/boat.gltf";
        case DecorationType::BoatRack:      return "assets/objects/decoration/props/boatrack.gltf";
        case DecorationType::Ladder:        return "assets/objects/decoration/props/ladder.gltf";
        case DecorationType::Target:        return "assets/objects/decoration/props/target.gltf";
        case DecorationType::Tent:          return "assets/objects/decoration/props/tent.gltf";
        case DecorationType::Trough:        return "assets/objects/decoration/props/trough.gltf";
        case DecorationType::Weaponrack:    return "assets/objects/decoration/props/weaponrack.gltf";
        case DecorationType::Wheelbarrow:   return "assets/objects/decoration/props/wheelbarrow.gltf";
        case DecorationType::Flag:          return "assets/objects/decoration/props/flag.gltf";

        default: return "assets/objects/decoration/props/barrel.gltf";
        }
    }

    inline const char* DecorationTypeToString(DecorationType type)
    {
        switch (type)
        {
        case DecorationType::TreeSmall:     return "Tree (Small)";
        case DecorationType::TreeMedium:    return "Tree (Medium)";
        case DecorationType::TreeLarge:     return "Tree (Large)";
        case DecorationType::RockA:         return "Rock A";
        case DecorationType::RockB:         return "Rock B";
        case DecorationType::RockC:         return "Rock C";
        case DecorationType::HillA:         return "Hill A";
        case DecorationType::MountainA:     return "Mountain A";
        case DecorationType::Barrel:        return "Barrel";
        case DecorationType::Crate:         return "Crate";
        case DecorationType::Haybale:       return "Haybale";
        default:                            return "Unknown";
        }
    }

    inline bool IsNatureDecoration(DecorationType type)
    {
        return type >= DecorationType::TreeSmall && type <= DecorationType::Waterplant;
    }

    inline bool IsPropDecoration(DecorationType type)
    {
        return type >= DecorationType::Barrel && type < DecorationType::Count;
    }

} // namespace RealmFortress
