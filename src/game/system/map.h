/**
 * @file map.h
 * @brief
 * @date 12/9/2025
 */

#pragma once

#include "core/base.h"
#include "game/system/tile.h"
#include "game/system/coordinate.h"
#include "renderer/shader.h"
#include <unordered_map>
#include <vector>

namespace RealmFortress
{
    class Map
    {
    public:
        Map() = default;
        ~Map() = default;

        void GenerateFlat(i32 radius, TileType default_type = TileType::Grass);
        void GenerateRectangle(i32 width, i32 height, TileType default_type = TileType::Grass);
        void GenerateHexagon(i32 radius, TileType default_type = TileType::Grass);
        void GenerateWithNoise(i32 radius, u32 seed = 0);

        void AddTile(const Coordinate& coord, TileType type, i32 elevation = 0);
        void RemoveTile(const Coordinate& coord);
        void Clear();

        Tile* GetTile(const Coordinate& coord);
        const Tile* GetTile(const Coordinate& coord) const;
        bool HasTile(const Coordinate& coord) const;

        std::vector<Tile*> GetNeighbors(const Coordinate& coord);
        std::vector<Tile*> GetTilesInRadius(const Coordinate& center, i32 radius);
        std::vector<Tile*> GetTilesInLine(const Coordinate& start, const Coordinate& end);

        void Draw(const Ref<Shader>& shader);

        void DrawWithHighlight(
            const Ref<Shader>& base_shader,
            const Ref<Shader>& highlight_shader,
            const std::unordered_set<Coordinate>& highlighted_hexes,
            const glm::vec3& highlight_color,
            f32 time
        );

        usize GetTileCount() const { return mTiles.size(); }
        const std::unordered_map<Coordinate, Tile>& GetTiles() const { return mTiles; }

    private:
        std::unordered_map<Coordinate, Tile> mTiles;
    };
} // namespace RealmFortress
