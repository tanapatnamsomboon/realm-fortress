/**
 * @file tile.h
 * @brief
 * @date 12/9/2025
 */

#pragma once

#include "core/base.h"
#include "game/system/coordinate.h"
#include "renderer/model.h"
#include <string>

#include "glm/gtx/associated_min_max.hpp"
#include "glm/gtx/associated_min_max.hpp"

namespace RealmFortress
{
    enum class TileType : u8
    {
        None = 0,
        Grass,
        Water,
        Coast,
        Road,
        River,
        Hill,
        Mountain,

        Count
    };

    class Tile
    {
    public:
        Tile() = default;
        Tile(const Coordinate& coord, TileType type, i32 elevation = 0);

        const Coordinate& GetCoordinate() const { return mCoordinate; }
        TileType GetType() const { return mType; }
        i32 GetElevation() const { return mElevation; }

        void SetType(TileType type);
        void SetElevation(i32 elevation);

        glm::vec3 GetWorldPosition() const;
        glm::mat4 GetTransform() const;

        void SetModel(const Ref<Model>& model) { mModel = model; }
        Ref<Model> GetModel() const { return mModel; }

        bool IsWalkable() const;
        bool IsWater() const;

    private:
        Coordinate mCoordinate{};
        TileType mType{ TileType::None };
        i32 mElevation{ 0 };
        Ref<Model> mModel{ nullptr };
    };

    const char* TileTypeToString(TileType type);
    std::string TileTypeToModelPath(TileType type);
} // namespace RealmFortress
