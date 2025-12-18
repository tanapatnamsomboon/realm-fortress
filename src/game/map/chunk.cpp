/**
 * @file chunk.cpp
 * @brief
 * @date 12/18/2025
 */

#include "core/pch.h"
#include "chunk.h"
#include "game/map/perlin_noise.h"
#include "renderer/model_cache.h"

namespace RealmFortress
{
    static void CalculateBiome(f32 elevation, f32 moisture, TileType& out_type, DecorationType& out_decoration, i32& out_elevation)
    {
        if (elevation < 0.35f)
        {
            out_type = TileType::Water;
            out_decoration = DecorationType::None;
            out_elevation = 0;
            return;
        }

        out_type = TileType::Grass;
        out_elevation = 0;

        if (elevation > 0.65f)
        {
            if (elevation > 0.7f)
                out_decoration = DecorationType::Mountain;
            else if (elevation > 0.75f)
                out_decoration = DecorationType::MountainGrass;
            else
                out_decoration = DecorationType::MountainGrassTree;
            return;
        }

        if (moisture < 0.4f)
        {
            out_decoration = DecorationType::None;
        }
        else if (moisture < 0.55f)
        {
            out_decoration = DecorationType::None;
        }
        else
        {
            if (moisture > 0.8f)
                out_decoration = DecorationType::TreeLarge;
            else if (moisture > 0.7f)
                out_decoration = DecorationType::TreeMedium;
            else
                out_decoration = DecorationType::TreeSmall;
        }
    }

    Chunk::Chunk(ChunkCoordinate coord)
        : mCoord(coord)
    {
    }

    static f32 HashToAngle(u32 seed, i32 q, i32 r)
    {
        u32 h = seed;
        h ^= q * 374761393u;
        h ^= r * 668265263u;
        h = (h ^ (h >> 13)) * 1274126177u;
        return (h & 0xFFFF) / 65535.0f * glm::two_pi<f32>();
    }

    void Chunk::Generate(u32 seed)
    {
        mTiles.clear();

        PerlinNoise elevation_noise(seed);
        PerlinNoise moisture_noise(seed + 12345);

        f32 scale = 0.08f; // Noise size

        for (i32 q = 0; q < CHUNK_SIZE; ++q)
        {
            for (i32 r = 0; r < CHUNK_SIZE; ++r)
            {
                i32 global_q = mCoord.Q * CHUNK_SIZE + q;
                i32 global_r = mCoord.R * CHUNK_SIZE + r;

                f32 e_val = elevation_noise.OctaveNoise(global_q * scale, global_r * scale, 4);
                e_val = std::pow(e_val, 1.2f);
                f32 m_val = moisture_noise.OctaveNoise(global_q * scale, global_r * scale, 4);

                TileType type;
                DecorationType deco;
                i32 height;

                CalculateBiome(e_val, m_val, type, deco, height);

                Coordinate local_coord(q, r);
                Tile tile(Coordinate(global_q, global_r), type, "", height);

                std::string model_path = TileTypeToModelPath(type);
                tile.SetModel(ModelCache::Load(model_path));
                tile.SetDecoration(deco);

                if (deco != DecorationType::None)
                {
                    f32 rotation = HashToAngle(seed, global_q, global_r);
                    tile.SetRotation(rotation);
                }

                mTiles[local_coord] = std::move(tile);
            }
        }
    }

    Tile* Chunk::GetTile(const Coordinate& local_coord)
    {
        auto it = mTiles.find(local_coord);
        return it != mTiles.end() ? &it->second : nullptr;
    }
} // namespace RealmFortress
