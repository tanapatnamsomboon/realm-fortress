/**
 * @file map.h
 * @brief
 * @date 12/9/2025
 */

#pragma once

#include "core/base.h"
#include "game/system/tile.h"
#include "game/system/coordinate.h"
#include "game/map/chunk.h"
#include "renderer/shader.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace RealmFortress
{
    class Map
    {
    public:
        Map();
        ~Map() = default;

        void OnUpdate(const glm::vec3& camera_position);
        void Regenerate(u32 seed);

        Tile* GetTile(const Coordinate& coord);
        const Tile* GetTile(const Coordinate& coord) const;
        bool HasTile(const Coordinate& coord) const;
        std::vector<const Tile*> GetNeighbors(const Coordinate& coord) const;
        std::vector<const Tile*> GetTilesInRadius(const Coordinate& center, i32 radius) const;
        std::vector<const Tile*> GetTilesInLine(const Coordinate& start, const Coordinate& end) const;
        usize GetTileCount() const;

        void Draw(const Ref<Shader>& shader);

    private:
        ChunkCoordinate WorldToChunkCoord(const Coordinate& coord) const;
        Coordinate WorldToLocalCoord(const Coordinate& coord) const;

        glm::vec3 GetChunkCenter(ChunkCoordinate chunk_coord) const;

    private:
        std::unordered_map<ChunkCoordinate, Chunk, ChunkCoordHash> mChunks;
        u32 mSeed{ 0 };
        i32 mRenderDistance{ 5 };

        glm::vec3 mLastCameraPos{ 0.0f };

        i32 mChunksLoadedPerFrame{ 1 };
    };
} // namespace RealmFortress
