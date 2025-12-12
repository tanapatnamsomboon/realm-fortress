/**
 * @file structure_types.h
 * @brief
 * @date 12/12/2025
 */

#pragma once

#include <string>

namespace RealmFortress
{
    enum class StructureType
    {
        None = 0,

        WallStraight,
        WallStraightGate,
        WallCornerAInside,
        WallCornerAOutside,
        WallCornerAGate,
        WallCornerBInside,
        WallCornerBOutside,

        FenceWoodStraight,
        FenceWoodStraightGate,
        FenceStoneStraight,
        FenceStoneStraightGate,

        BridgeA,
        BridgeB,
        StageA,
        StageB,
        StageC,
        Scaffolding,
        Destroyed,
        Dirt,
        Grain,

        Count
    };

    inline std::string StructureTypeToModelPath(StructureType type)
    {
        const std::string base = "assets/objects/buildings/neutral/";

        switch (type)
        {
        case StructureType::WallStraight:           return base + "wall_straight.gltf";
        case StructureType::WallStraightGate:       return base + "wall_straight_gate.gltf";
        case StructureType::WallCornerAInside:      return base + "wall_corner_A_inside.gltf";
        case StructureType::WallCornerAOutside:     return base + "wall_corner_A_outside.gltf";
        case StructureType::WallCornerAGate:        return base + "wall_corner_A_gate.gltf";
        case StructureType::WallCornerBInside:      return base + "wall_corner_B_inside.gltf";
        case StructureType::WallCornerBOutside:     return base + "wall_corner_B_outside.gltf";

        case StructureType::FenceWoodStraight:      return base + "fence_wood_straight.gltf";
        case StructureType::FenceWoodStraightGate:  return base + "fence_wood_straight_gate.gltf";
        case StructureType::FenceStoneStraight:     return base + "fence_stone_straight.gltf";
        case StructureType::FenceStoneStraightGate: return base + "fence_stone_straight_gate.gltf";

        case StructureType::BridgeA:                return base + "building_bridge_A.gltf";
        case StructureType::BridgeB:                return base + "building_bridge_B.gltf";
        case StructureType::StageA:                 return base + "building_stage_A.gltf";
        case StructureType::StageB:                 return base + "building_stage_B.gltf";
        case StructureType::StageC:                 return base + "building_stage_C.gltf";
        case StructureType::Scaffolding:            return base + "building_scaffolding.gltf";
        case StructureType::Destroyed:              return base + "building_destroyed.gltf";
        case StructureType::Dirt:                   return base + "building_dirt.gltf";
        case StructureType::Grain:                  return base + "building_grain.gltf";
        default:                                    return base + "building_dirt.gltf";
        }
    }
} // namespace RealmFortress
