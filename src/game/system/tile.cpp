/**
 * @file tile.cpp
 * @brief
 * @date 12/9/2025
 */

#include "core/pch.h"
#include "tile.h"
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/model_cache.h"

namespace RealmFortress
{
    Tile::Tile(const Coordinate& coord, TileType type, const std::string& model_path, i32 elevation)
        : mCoordinate(coord), mType(type), mModelPath(model_path), mElevation(elevation)
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
        // TODO: implement
        return true;
    }

    bool Tile::IsWater() const
    {
        return mType == TileType::Water;
    }

    void Tile::SetDecoration(DecorationType decoration)
    {
        mDecoration = decoration;
        if (decoration != DecorationType::None)
        {
            const char* path = DecorationTypeToModelPath(decoration);
            if (path)
            {
                mDecorationModel = ModelCache::Load(path);
            }
        }
    }

    glm::mat4 Tile::GetDecorationTransform() const
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, GetWorldPosition());
        transform = glm::rotate(transform, glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));

        return transform;
    }

    const char* TileTypeToString(TileType type)
    {
        switch (type)
        {
        case TileType::None:     return "None";
        case TileType::Grass:    return "Grass";
        case TileType::Water:    return "Water";
        default:                 return "Unknown";
        }
    }

    std::string TileTypeToModelPath(TileType type)
    {
        switch (type)
        {
        case TileType::Grass:    return "assets/objects/tiles/base/hex_grass.gltf";
        case TileType::Water:    return "assets/objects/tiles/base/hex_water.gltf";
        default:                 return "assets/objects/tiles/base/hex_grass.gltf";
        }
    }
} // namespace RealmFortress
