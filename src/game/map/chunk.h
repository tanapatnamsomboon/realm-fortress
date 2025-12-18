/**
 * @file chunk.h
 * @brief
 * @date 12/18/2025
 */

#pragma once

#include "core/base.h"
#include "game/system/tile.h"
#include <unordered_map>

namespace RealmFortress
{
    constexpr i32 CHUNK_SIZE = 16;

    struct ChunkCoordinate
    {
        i32 Q, R;
        bool operator==(const ChunkCoordinate& other) const { return Q == other.Q && R == other.R; }
    };

    struct ChunkCoordHash
    {
        usize operator()(const ChunkCoordinate& k) const
        {
            return std::hash<i32>()(k.Q) ^ (std::hash<i32>()(k.R) << 1);
        }
    };

    class Chunk
    {
    public:
        Chunk(ChunkCoordinate coord);

        void Generate(u32 seed);

        Tile* GetTile(const Coordinate& local_coord);
        const std::unordered_map<Coordinate, Tile>& GetTiles() const { return mTiles; }

        ChunkCoordinate GetCoordinate() const { return mCoord; }

    private:
        ChunkCoordinate mCoord;
        std::unordered_map<Coordinate, Tile> mTiles;
    };
} // namespace RealmFortress
