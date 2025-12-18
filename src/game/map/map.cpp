/**
 * @file map.cpp
 * @brief
 * @date 12/9/2025
 */

#include "core/pch.h"
#include "map.h"
#include "game/map/perlin_noise.h"
#include "renderer/model_cache.h"

namespace RealmFortress
{
    Map::Map()
    {
        mSeed = static_cast<u32>(std::time(nullptr));
    }

    void Map::OnUpdate(const glm::vec3& camera_position)
    {
        mLastCameraPos = camera_position;

        Coordinate center = Coordinate::FromWorldPosition(camera_position);

        ChunkCoordinate center_chunk = WorldToChunkCoord(center);

        auto it = mChunks.begin();
        while (it != mChunks.end())
        {
            i32 dist_q = std::abs(it->first.Q - center_chunk.Q);
            i32 dist_r = std::abs(it->first.R - center_chunk.R);

            if (dist_q > mRenderDistance + 2 || dist_r > mRenderDistance + 2)
            {
                it = mChunks.erase(it);
            }
            else
            {
                ++it;
            }
        }

        struct MissingChunk
        {
            ChunkCoordinate Coord;
            i32 DistSq;
        };
        std::vector<MissingChunk> missing_chunks;

        for (i32 q = -mRenderDistance; q <= mRenderDistance; ++q)
        {
            for (i32 r = -mRenderDistance; r <= mRenderDistance; ++r)
            {
                ChunkCoordinate target_chunk = { center_chunk.Q + q, center_chunk.R + r };

                if (!mChunks.contains(target_chunk))
                {
                    i32 dq = target_chunk.Q - center_chunk.Q;
                    i32 dr = target_chunk.R - center_chunk.R;
                    missing_chunks.push_back({ target_chunk, dq * dq + dr * dr });
                }
            }
        }

        std::ranges::sort(missing_chunks, [](const MissingChunk& a, const MissingChunk& b)
        {
            return a.DistSq < b.DistSq;
        });

        i32 loaded_count = 0;
        for (const auto& missing : missing_chunks)
        {
            if (loaded_count >= mChunksLoadedPerFrame) break;

            Chunk new_chunk(missing.Coord);
            new_chunk.Generate(mSeed);
            mChunks.emplace(missing.Coord, std::move(new_chunk));

            loaded_count++;
        }
    }

    void Map::Regenerate(u32 seed)
    {
        mSeed = seed;
        mChunks.clear();
        RF_CORE_INFO("Map cleared and seed updated to {}", mSeed);
    }

    Tile* Map::GetTile(const Coordinate& coord)
    {
        ChunkCoordinate chunk_coord = WorldToChunkCoord(coord);
        auto it = mChunks.find(chunk_coord);

        if (it != mChunks.end())
        {
            return it->second.GetTile(WorldToLocalCoord(coord));
        }
        return nullptr;
    }

    const Tile* Map::GetTile(const Coordinate& coord) const
    {
        return const_cast<Map*>(this)->GetTile(coord);
    }

    bool Map::HasTile(const Coordinate& coord) const
    {
        return GetTile(coord) != nullptr;
    }

    std::vector<const Tile*> Map::GetNeighbors(const Coordinate& coord) const
    {
        std::vector<const Tile*> neighbors;
        auto neighbor_coords = coord.GetNeighbors();

        for (const auto& nc : neighbor_coords)
        {
            if (auto* tile = GetTile(nc))
            {
                neighbors.push_back(tile);
            }
        }
        return neighbors;
    }

    std::vector<const Tile*> Map::GetTilesInRadius(const Coordinate& center, i32 radius) const
    {
        std::vector<const Tile*> tiles;

        for (i32 q = -radius; q <= radius; ++q)
        {
            i32 r1 = std::max(-radius, -q - radius);
            i32 r2 = std::min(radius, -q + radius);

            for (i32 r = r1; r <= r2; ++r)
            {
                Coordinate coord = center + Coordinate(q, r);
                if (auto* tile = GetTile(coord))
                {
                    tiles.push_back(tile);
                }
            }
        }

        return tiles;
    }

    std::vector<const Tile*> Map::GetTilesInLine(const Coordinate& start, const Coordinate& end) const
    {
        std::vector<const Tile*> tiles;
        auto lineCoords = start.LineTo(end);

        for (const auto& coord : lineCoords)
        {
            if (auto* tile = GetTile(coord))
            {
                tiles.push_back(tile);
            }
        }

        return tiles;
    }

    void Map::Draw(const Ref<Shader>& shader)
    {
        if (!shader)
        {
            RF_CORE_ERROR("Cannot draw map: shader is null");
            return;
        }
        shader->Bind();
        shader->SetFloat3("uCameraPos", mLastCameraPos);
        shader->SetFloat("uFogStart", 60.0f);
        shader->SetFloat("uFogEnd", 130.0f);
        shader->SetFloat3("uFogColor", glm::vec3(0.53f, 0.81f, 0.92f));

        std::map<Model*, std::vector<glm::mat4>> batch_data;

        f32 max_draw_dist_sq = 140.0f * 140.0f;

        for (const auto& [chunk_coord, chunk] : mChunks)
        {
            glm::vec3 chunk_center = GetChunkCenter(chunk_coord);

            f32 dist_sq = glm::distance2(mLastCameraPos, chunk_center);
            if (dist_sq > max_draw_dist_sq) continue;

            for (const auto& tile : chunk.GetTiles() | std::views::values)
            {
                // Draw Base Model
                if (auto model = tile.GetModel())
                    batch_data[model.get()].push_back(tile.GetTransform());

                // Draw Decoration
                if (tile.GetDecoration() != DecorationType::None)
                {
                    if (auto deco_model = tile.GetDecorationModel())
                        batch_data[deco_model.get()].push_back(tile.GetDecorationTransform());
                }
            }
        }

        for (auto& [model_ptr, transforms] : batch_data)
        {
            if (model_ptr && !transforms.empty())
            {
                model_ptr->DrawInstanced(shader, transforms);
            }
        }
    }

    usize Map::GetTileCount() const
    {
        usize count = 0;
        for (const auto& val : mChunks | std::views::values)
            count += val.GetTiles().size();
        return count;
    }

    ChunkCoordinate Map::WorldToChunkCoord(const Coordinate& coord) const
    {
        i32 q = static_cast<i32>(std::floor(static_cast<f32>(coord.Q) / CHUNK_SIZE));
        i32 r = static_cast<i32>(std::floor(static_cast<f32>(coord.R) / CHUNK_SIZE));
        return { q, r };
    }

    Coordinate Map::WorldToLocalCoord(const Coordinate& coord) const
    {
        i32 q = coord.Q % CHUNK_SIZE;
        i32 r = coord.R % CHUNK_SIZE;

        if (q < 0) q += CHUNK_SIZE;
        if (r < 0) r += CHUNK_SIZE;

        return Coordinate(q, r);
    }

    glm::vec3 Map::GetChunkCenter(ChunkCoordinate chunk_coord) const
    {
        f32 center_q = (chunk_coord.Q * CHUNK_SIZE) + (CHUNK_SIZE / 2.0f);
        f32 center_r = (chunk_coord.R * CHUNK_SIZE) + (CHUNK_SIZE / 2.0f);

        Coordinate coord(center_q, center_r);

        return coord.ToWorldPosition();
    }
} // namespace RealmFortress
