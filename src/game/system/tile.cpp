/**
 * @file tile.cpp
 * @brief
 * @date 12/9/2025
 */

#include "core/pch.h"
#include "tile.h"
#include <glm/gtc/matrix_transform.hpp>

namespace RealmFortress
{
    Tile::Tile(const Coordinate& coord, TileType type, i32 elevation)
        : mCoordinate(coord), mType(type), mElevation(elevation)
    {
    }

    void Tile::SetType(TileType type)
    {
        mType = type;
    }

    void Tile::SetElevation(i32 elevation)
    {
        mElevation = elevation;
    }

    glm::vec3 Tile::GetWorldPosition() const
    {
        return mCoordinate.ToWorldPosition(mElevation);
    }

    glm::mat4 Tile::GetTransform() const
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, GetWorldPosition());
        return transform;
    }

    bool Tile::IsWalkable() const
    {
        switch (mType)
        {
        case TileType::Grass:    [[fallthrough]];
        case TileType::Road:     [[fallthrough]];
        case TileType::Coast:    return true;
        case TileType::Water:    [[fallthrough]];
        case TileType::River:    [[fallthrough]];
        case TileType::Mountain: return false;
        default:
            return false;
        }
    }

    bool Tile::IsWater() const
    {
        return mType == TileType::Water || mType == TileType::River;
    }

    const char* TileTypeToString(TileType type)
    {
        switch (type)
        {
        case TileType::None:     return "None";
        case TileType::Grass:    return "Grass";
        case TileType::Water:    return "Water";
        case TileType::Coast:    return "Coast";
        case TileType::Road:     return "Road";
        case TileType::River:    return "River";
        case TileType::Hill:     return "Hill";
        case TileType::Mountain: return "Mountain";
        default:                 return "Unknown";
        }
    }

    std::string TileTypeToModelPath(TileType type)
    {
        switch (type)
        {
        case TileType::Grass:    return "assets/objects/tiles/base/hex_grass.gltf";
        case TileType::Water:    return "assets/objects/tiles/base/hex_water.gltf";
        case TileType::Coast:    return "assets/objects/tiles/coast/hex_coast_A.gltf";
        case TileType::Road:     return "assets/objects/tiles/roads/hex_road_A.gltf";
        case TileType::River:    return "assets/objects/tiles/rivers/hex_river_straight.gltf";
        case TileType::Hill:     return "assets/objects/decoration/nature/hill_A.gltf";
        case TileType::Mountain: return "assets/objects/decoration/nature/mountain_A.gltf";
        default:                 return "assets/objects/tiles/base/hex_grass.gltf";
        }
    }
} // namespace RealmFortress
