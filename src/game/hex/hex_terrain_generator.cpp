/**
 * @file hex_terrain_generator.cpp
 * @brief Implementation of procedural terrain generation
 * @date 12/5/2025
 */

#include "core/pch.h"
#include "hex_terrain_generator.h"
#include "core/logger.h"

namespace RealmFortress
{
    // Simple Perlin noise implementation
    // Based on Ken Perlin's improved noise: https://mrl.nyu.edu/~perlin/noise/

    static const i32 PERMUTATION[] = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
        134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
        55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
        18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
        250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
        189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
        228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
        107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    // Double the permutation to avoid overflow
    static i32 P[512];
    static bool P_INITIALIZED = false;

    static void InitializePermutation()
    {
        if (P_INITIALIZED) return;

        for (i32 i = 0; i < 256; i++)
        {
            P[i] = PERMUTATION[i];
            P[256 + i] = PERMUTATION[i];
        }

        P_INITIALIZED = true;
    }

    static f32 Fade(f32 t)
    {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    static f32 Lerp(f32 t, f32 a, f32 b)
    {
        return a + t * (b - a);
    }

    static f32 Grad(i32 hash, f32 x, f32 y)
    {
        i32 h = hash & 15;
        f32 u = h < 8 ? x : y;
        f32 v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    HexTerrainGenerator::HexTerrainGenerator()
    {
        InitializePermutation();
    }

    f32 HexTerrainGenerator::PerlinNoise(f32 x, f32 y)
    {
        // Find unit square that contains point
        i32 X = static_cast<i32>(std::floor(x)) & 255;
        i32 Y = static_cast<i32>(std::floor(y)) & 255;

        // Find relative x, y of point in square
        x -= std::floor(x);
        y -= std::floor(y);

        // Compute fade curves
        f32 u = Fade(x);
        f32 v = Fade(y);

        // Hash coordinates of the 4 square corners
        i32 a = P[X] + Y;
        i32 aa = P[a];
        i32 ab = P[a + 1];
        i32 b = P[X + 1] + Y;
        i32 ba = P[b];
        i32 bb = P[b + 1];

        // Blend results from 4 corners
        f32 res = Lerp(v,
            Lerp(u, Grad(P[aa], x, y), Grad(P[ba], x - 1, y)),
            Lerp(u, Grad(P[ab], x, y - 1), Grad(P[bb], x - 1, y - 1))
        );

        // Return value in range [-1, 1]
        return res;
    }

    f32 HexTerrainGenerator::FractalNoise(f32 x, f32 y, i32 octaves, f32 persistence, f32 lacunarity)
    {
        f32 total = 0.0f;
        f32 frequency = 1.0f;
        f32 amplitude = 1.0f;
        f32 maxValue = 0.0f;

        for (i32 i = 0; i < octaves; i++)
        {
            total += PerlinNoise(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        // Normalize to [0, 1]
        return (total / maxValue + 1.0f) * 0.5f;
    }

    void HexTerrainGenerator::Generate(HexMap& map, const TerrainGenerationSettings& settings)
    {
        RF_CORE_INFO("Generating terrain with seed: {}", settings.Seed);

        mRng.seed(settings.Seed);
        mNoiseCache.clear();

        map.Clear();

        // Phase 1: Generate base terrain from noise
        GenerateBaseTerrain(map, settings);

        // Phase 2: Add coastlines where water meets land
        GenerateCoastlines(map);

        // Phase 3: Generate rivers
        if (settings.GenerateRivers)
        {
            GenerateRivers(map, settings);
        }

        // Phase 4: Generate roads
        if (settings.GenerateRoads)
        {
            GenerateRoads(map, settings);
        }

        // Phase 5: Smooth terrain
        SmoothTerrain(map);

        RF_CORE_INFO("Terrain generation complete. Tiles: {}", map.GetTileCount());
    }

    void HexTerrainGenerator::GenerateBaseTerrain(HexMap& map, const TerrainGenerationSettings& settings)
    {
        RF_CORE_INFO("Generating base terrain ({} x {})...", settings.Width, settings.Height);

        for (i32 q = 0; q < settings.Width; q++)
        {
            for (i32 r = 0; r < settings.Height; r++)
            {
                HexCoordinate coord(q, r);
                f32 noise = GetNoiseAt(coord, settings);

                HexTileType type = NoiseToTileType(noise, settings);

                // Calculate elevation based on noise
                i32 elevation = 0;
                if (noise > settings.GrassThreshold)
                {
                    elevation = static_cast<i32>((noise - settings.GrassThreshold) * 10.0f);
                }

                map.SetTile(coord, type, elevation);
            }
        }

        RF_CORE_INFO("Base terrain generated");
    }

    void HexTerrainGenerator::GenerateCoastlines(HexMap& map)
    {
        RF_CORE_INFO("Generating coastlines...");

        std::vector<HexCoordinate> coastTiles;

        // Find all water tiles adjacent to land
        HexCoordinate min, max;
        map.GetBounds(min, max);

        for (i32 q = min.q; q <= max.q; q++)
        {
            for (i32 r = min.r; r <= max.r; r++)
            {
                HexCoordinate coord(q, r);
                HexTile* tile = map.GetTile(coord);

                if (!tile || tile->Type != HexTileType::Water)
                    continue;

                // Check if any neighbor is land
                bool hasLandNeighbor = false;
                auto neighbors = coord.GetNeighbors();

                for (const auto& neighbor : neighbors)
                {
                    HexTile* neighborTile = map.GetTile(neighbor);
                    if (neighborTile && neighborTile->Type != HexTileType::Water)
                    {
                        hasLandNeighbor = true;
                        break;
                    }
                }

                if (hasLandNeighbor)
                {
                    coastTiles.push_back(coord);
                }
            }
        }

        // Convert water tiles to coast tiles
        // For now, just use CoastA randomly
        std::uniform_int_distribution<i32> coastDist(0, 4);

        for (const auto& coord : coastTiles)
        {
            HexTileType coastType = static_cast<HexTileType>(
                static_cast<i32>(HexTileType::CoastA) + coastDist(mRng)
            );

            HexTile* tile = map.GetTile(coord);
            if (tile)
            {
                tile->Type = coastType;
            }
        }

        RF_CORE_INFO("Coastlines generated: {} tiles", coastTiles.size());
    }

    void HexTerrainGenerator::GenerateRivers(HexMap& map, const TerrainGenerationSettings& settings)
    {
        RF_CORE_INFO("Generating {} rivers...", settings.NumRivers);

        HexCoordinate min, max;
        map.GetBounds(min, max);

        std::uniform_int_distribution<i32> qDist(min.q, max.q);
        std::uniform_int_distribution<i32> rDist(min.r, max.r);

        for (i32 i = 0; i < settings.NumRivers; i++)
        {
            // Find a random starting point on higher ground
            HexCoordinate start;
            i32 attempts = 0;

            do
            {
                start = HexCoordinate(qDist(mRng), rDist(mRng));
                attempts++;

                if (attempts > 100)
                {
                    RF_CORE_WARN("Could not find suitable river start point");
                    break;
                }

            } while (!map.HasTile(start) ||
                     map.GetTile(start)->Type == HexTileType::Water ||
                     map.GetTile(start)->Elevation < 1);

            if (attempts > 100) continue;

            // Generate river path downhill to water
            auto riverPath = GenerateRiverPath(map, start, settings.MaxRiverLength);

            if (riverPath.size() < static_cast<size_t>(settings.MinRiverLength))
                continue;

            // Place river tiles
            for (size_t j = 0; j < riverPath.size(); j++)
            {
                const auto& coord = riverPath[j];
                HexTile* tile = map.GetTile(coord);

                if (!tile) continue;

                // Determine which river tile to use based on neighbors
                HexCoordinate from = (j > 0) ? riverPath[j - 1] : coord;
                HexCoordinate to = (j < riverPath.size() - 1) ? riverPath[j + 1] : coord;

                HexTileType riverType = DetermineRiverTile(map, coord, from, to);
                tile->Type = riverType;
            }

            RF_CORE_INFO("Generated river {} with {} tiles", i + 1, riverPath.size());
        }
    }

    std::vector<HexCoordinate> HexTerrainGenerator::GenerateRiverPath(
        HexMap& map,
        const HexCoordinate& start,
        i32 maxLength)
    {
        std::vector<HexCoordinate> path;
        path.push_back(start);

        HexCoordinate current = start;

        for (i32 i = 0; i < maxLength; i++)
        {
            // Find the lowest neighbor
            auto neighbors = current.GetNeighbors();
            HexCoordinate lowest = current;
            i32 lowestElevation = 99999;

            for (const auto& neighbor : neighbors)
            {
                HexTile* tile = map.GetTile(neighbor);
                if (!tile) continue;

                // Prefer going downhill, but allow slight uphill for variation
                i32 elevation = tile->Elevation;
                if (tile->Type == HexTileType::Water)
                    elevation = -1; // Water is "lowest"

                if (elevation < lowestElevation)
                {
                    lowestElevation = elevation;
                    lowest = neighbor;
                }
            }

            // Stop if we reached water or can't go lower
            if (lowest == current)
                break;

            HexTile* lowestTile = map.GetTile(lowest);
            if (lowestTile && lowestTile->Type == HexTileType::Water)
            {
                path.push_back(lowest);
                break;
            }

            current = lowest;
            path.push_back(current);
        }

        return path;
    }

    HexTileType HexTerrainGenerator::DetermineRiverTile(
        HexMap& map,
        const HexCoordinate& coord,
        const HexCoordinate& from,
        const HexCoordinate& to)
    {
        // For now, just use straight river
        // TODO: Implement proper river tile selection based on flow direction
        return HexTileType::RiverStraight;
    }

    void HexTerrainGenerator::GenerateRoads(HexMap& map, const TerrainGenerationSettings& settings)
    {
        // TODO: Implement road generation between settlements
        RF_CORE_INFO("Road generation not yet implemented");
    }

    void HexTerrainGenerator::SmoothTerrain(HexMap& map)
    {
        // TODO: Implement terrain smoothing to reduce harsh transitions
        RF_CORE_INFO("Terrain smoothing not yet implemented");
    }

    f32 HexTerrainGenerator::GetNoiseAt(const HexCoordinate& coord, const TerrainGenerationSettings& settings)
    {
        // Check cache first
        auto it = mNoiseCache.find(coord);
        if (it != mNoiseCache.end())
            return it->second;

        // Convert hex coordinates to continuous space
        glm::vec3 worldPos = coord.ToWorldPosition();
        f32 x = worldPos.x * settings.NoiseScale;
        f32 y = worldPos.z * settings.NoiseScale;

        // Generate fractal noise
        f32 noise = FractalNoise(
            x,
            y,
            settings.Octaves,
            settings.Persistence,
            settings.Lacunarity
        );

        // Cache the result
        mNoiseCache[coord] = noise;

        return noise;
    }

    HexTileType HexTerrainGenerator::NoiseToTileType(f32 noise, const TerrainGenerationSettings& settings)
    {
        if (noise < settings.WaterThreshold)
            return HexTileType::Water;

        if (noise < settings.CoastThreshold)
            return HexTileType::Grass; // Coast will be handled separately

        if (noise < settings.GrassThreshold)
            return HexTileType::Grass;

        if (noise < settings.ForestThreshold)
            return HexTileType::Grass; // Can add forest later

        if (noise < settings.HillThreshold)
            return HexTileType::Hill;

        if (noise < settings.MountainThreshold)
            return HexTileType::Hill;

        return HexTileType::Mountain;
    }
} // namespace RealmFortress
