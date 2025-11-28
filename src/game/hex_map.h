/**
 * @file hex_map.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/base.h"
#include "game/hex_coordinate.h"
#include "renderer/model.h"
#include "renderer/shader.h"
#include <unordered_map>
#include <string>

namespace RF
{
    enum class HexTileType
    {
        Grass,
        Water,
        Mountain,
        Forest,
        None
    };

    struct HexTile
    {
        HexCoordinate Coordinate;
        HexTileType Type;
        i32 Elevation;

        HexTile()
            : Type(HexTileType::None), Elevation(0)
        {
        }

        HexTile(const HexCoordinate& coord, HexTileType type, i32 elevation = 0)
            : Coordinate(coord), Type(type), Elevation(elevation)
        {
        }
    };

    class HexMap
    {
    public:
        HexMap();
        ~HexMap();

        void Generate(i32 width, i32 height);
        void Clear();

        void SetTile(const HexCoordinate& coord, HexTileType type, i32 elevation = 0);
        HexTile* GetTile(const HexCoordinate& coord);
        const HexTile* GetTile(const HexCoordinate& coord) const;
        bool HasTile(const HexCoordinate& coord) const;

        void Draw(const Ref<Shader>& shader);

        i32 GetTileCount() const { return static_cast<i32>(mTiles.size()); }

    private:
        void LoadModels();
        Ref<Model> GetModelForTile(const HexTile& tile);

    private:
        std::unordered_map<HexCoordinate, HexTile> mTiles;

        std::unordered_map<std::string, Ref<Model>> mModels;
        bool mModelsLoaded = false;
    };
} // namespace RF
