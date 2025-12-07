/**
 * @file hex_terrain_generator.h
 * @brief Procedural terrain generation for hex maps
 * @date 12/5/2025
 */

#pragma once

#include "core/base.h"
#include "hex_map.h"
#include "hex_coordinate.h"
#include <random>

namespace RealmFortress
{
    /**
     * @struct TerrainGenerationSettings
     * @brief Settings for procedural terrain generation
     */
    struct TerrainGenerationSettings
    {
        u32 Seed = 0;

        // Map dimensions
        i32 Width = 20;
        i32 Height = 20;

        // Noise parameters
        f32 NoiseScale = 0.1f;         // How zoomed in/out the noise is
        i32 Octaves = 4;               // Number of noise layers
        f32 Persistence = 0.5f;        // How much each octave contributes
        f32 Lacunarity = 2.0f;         // How much detail is added per octave

        // Terrain thresholds
        f32 WaterThreshold = 0.3f;     // Below this = water
        f32 CoastThreshold = 0.35f;    // Between water and coast
        f32 GrassThreshold = 0.6f;     // Regular grass
        f32 ForestThreshold = 0.75f;   // Forest areas
        f32 HillThreshold = 0.85f;     // Hills
        f32 MountainThreshold = 0.92f; // Mountains

        // River generation
        bool GenerateRivers = true;
        i32 NumRivers = 3;
        i32 MinRiverLength = 5;
        i32 MaxRiverLength = 15;

        // Road generation
        bool GenerateRoads = false;

        TerrainGenerationSettings() = default;
    };

    /**
     * @class HexTerrainGenerator
     * @brief Generates procedural terrain for hex maps using noise
     */
    class HexTerrainGenerator
    {
    public:
        HexTerrainGenerator();
        ~HexTerrainGenerator() = default;

        void Generate(HexMap& map, const TerrainGenerationSettings& settings);

        // Noise functions
        static f32 PerlinNoise(f32 x, f32 y);
        static f32 FractalNoise(f32 x, f32 y, i32 octaves, f32 persistence, f32 lacunarity);

    private:
        void GenerateBaseTerrain(HexMap& map, const TerrainGenerationSettings& settings);
        void GenerateCoastlines(HexMap& map);
        void GenerateRivers(HexMap& map, const TerrainGenerationSettings& settings);
        void GenerateRoads(HexMap& map, const TerrainGenerationSettings& settings);
        void SmoothTerrain(HexMap& map);

        // River generation helpers
        std::vector<HexCoordinate> GenerateRiverPath(
            HexMap& map,
            const HexCoordinate& start,
            i32 maxLength
        );

        HexTileType DetermineRiverTile(
            HexMap& map,
            const HexCoordinate& coord,
            const HexCoordinate& from,
            const HexCoordinate& to
        );

        f32 GetNoiseAt(const HexCoordinate& coord, const TerrainGenerationSettings& settings);
        HexTileType NoiseToTileType(f32 noise, const TerrainGenerationSettings& settings);

    private:
        std::mt19937 mRng;
        std::unordered_map<HexCoordinate, f32> mNoiseCache;
    };
} // namespace RealmFortress
