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
        Tile(const Coordinate& coord, TileType type, const std::string& model_path = "", i32 elevation = 0);

        const Coordinate& GetCoordinate() const { return mCoordinate; }
        TileType GetType() const { return mType; }
        i32 GetElevation() const { return mElevation; }
        const std::string& GetModelPath() const { return mModelPath; }
        f32 GetRotation() const { return mRotation; }

        void SetType(TileType type);
        void SetElevation(i32 elevation);
        void SetModelPath(const std::string& model_path) { mModelPath = model_path; }
        void SetRotation(f32 degrees) { mRotation = degrees; }

        glm::vec3 GetWorldPosition() const;
        glm::mat4 GetTransform() const;

        void SetModel(const Ref<Model>& model) { mModel = model; }
        Ref<Model> GetModel() const { return mModel; }

        bool IsWalkable() const;
        bool IsWater() const;

    private:
        Coordinate mCoordinate{};
        TileType mType{ TileType::None };
        std::string mModelPath;
        f32 mRotation{ 0.0f };
        i32 mElevation{ 0 };
        Ref<Model> mModel{ nullptr };
    };

    const char* TileTypeToString(TileType type);
    std::string TileTypeToModelPath(TileType type);
} // namespace RealmFortress
