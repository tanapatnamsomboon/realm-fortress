/**
 * @file hex_map.h
 * @brief Hex map management and rendering system
 * @date 11/27/2025
 */

#pragma once

#include "core/base.h"
#include "game/hex/hex_coordinate.h"
#include "renderer/model.h"
#include "renderer/shader.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace RealmFortress
{
    /**
     * @enum HexTileType
     * @brief Types of terrain tiles available in the game
     */
    enum class HexTileType : u8
    {
        None = 0,

        // Base terrain
        Grass,
        GrassBottom,      // Grass with cliff face
        GrassSlopedLow,   // Grass sloped down
        GrassSlopedHigh,  // Grass sloped up
        Water,
        Transition,       // Grass to water transition

        // Coast variants (where land meets water)
        CoastA,
        CoastB,
        CoastC,
        CoastD,
        CoastE,

        // Rivers (with water)
        RiverStraight,        // hex_river_A
        RiverStraightCurvy,   // hex_river_A_curvy
        RiverCurveB,          // hex_river_B
        RiverCurveC,          // hex_river_C
        RiverForkD,           // hex_river_D
        RiverForkE,           // hex_river_E
        RiverForkF,           // hex_river_F
        RiverForkG,           // hex_river_G
        RiverJunctionH,       // hex_river_H
        RiverJunctionI,       // hex_river_I
        RiverJunctionJ,       // hex_river_J
        RiverJunctionK,       // hex_river_K
        RiverEndL,            // hex_river_L
        RiverCrossingA,       // hex_river_crossing_A
        RiverCrossingB,       // hex_river_crossing_B

        // Roads
        RoadA,                // Straight
        RoadASlopedLow,
        RoadASlopedHigh,
        RoadB,                // Curve
        RoadC,                // Curve
        RoadD,                // T-junction
        RoadE,                // T-junction
        RoadF,                // T-junction
        RoadG,                // Y-junction
        RoadH,                // Y-junction
        RoadI,                // Y-junction
        RoadJ,                // Cross
        RoadK,                // End
        RoadL,                // End
        RoadM,                // End

        // Decorative (not walkable)
        Mountain,
        Forest,
        Hill,

        Count
    };

    /**
     * @struct HexTile
     * @brief Represents a single hex tile in the map
     */
    struct HexTile
    {
        HexCoordinate Coordinate;
        HexTileType Type = HexTileType::None;
        i32 Elevation = 0;
        u8 Variation = 0;     // For tiles with multiple visual variants

        bool Walkable = true;
        bool Visible = true;
        bool Explored = false;

        HexTile() = default;

        HexTile(const HexCoordinate& coord, HexTileType type, i32 elevation = 0)
            : Coordinate(coord), Type(type), Elevation(elevation)
        {
            // Set walkability based on tile type
            UpdateWalkability();
        }

        void UpdateWalkability()
        {
            switch (Type)
            {
            case HexTileType::Water:
            case HexTileType::Mountain:
            case HexTileType::None:
                Walkable = false;
                break;
            default:
                Walkable = true;
                break;
            }
        }
    };

    /**
     * @class HexMap
     * @brief Manages the hex tile grid and rendering
     */
    class HexMap
    {
    public:
        HexMap();
        ~HexMap();

        void Generate(i32 width, i32 height);
        void GenerateFromNoise(i32 width, i32 height, u32 seed = 0);
        void Clear();

        void SetTile(const HexCoordinate& coord, HexTileType type, i32 elevation = 0);
        void SetTileElevation(const HexCoordinate& coord, i32 elevation);
        void SetTileVariation(const HexCoordinate& coord, u8 variation);

        HexTile* GetTile(const HexCoordinate& coord);
        const HexTile* GetTile(const HexCoordinate& coord) const;
        bool HasTile(const HexCoordinate& coord) const;

        void RemoveTile(const HexCoordinate& coord);

        void Draw(const Ref<Shader>& shader);
        void DrawTile(const HexCoordinate& coord, const Ref<Shader>& shader);

        void BeginBatchRender();
        void EndBatchRender(const Ref<Shader>& shader);

        i32 GetTileCount() const { return static_cast<i32>(mTiles.size()); }
        std::vector<HexCoordinate> GetTilesInRange(const HexCoordinate& center, i32 range) const;
        std::vector<HexTile*> GetTilesOfType(HexTileType type);

        void GetBounds(HexCoordinate& min, HexCoordinate& max) const;
        glm::vec2 GetWorldBounds() const;

        std::vector<HexCoordinate> FindPath(const HexCoordinate& start, const HexCoordinate& end);
        bool IsWalkable(const HexCoordinate& coord) const;
        bool IsVisible(const HexCoordinate& coord) const;

        void SetDebugMode(bool enabled) { mDebugMode = enabled; }
        bool IsDebugMode() const { return mDebugMode; }

        // Utility methods
        static const char* TileTypeToString(HexTileType type);
        static bool IsTileTypeWalkable(HexTileType type);

    private:
        void LoadTileModels();
        Ref<Model> GetModelForTile(const HexTile& tile);
        std::string GetModelPath(HexTileType type, u8 variation = 0);

    private:
        std::unordered_map<HexCoordinate, HexTile> mTiles;

        std::unordered_map<std::string, Ref<Model>> mModelCache;
        bool mModelsLoaded = false;

        struct BatchData
        {
            HexTileType Type;
            std::vector<glm::mat4> Transforms;
        };
        std::unordered_map<HexTileType, BatchData> mBatchData;
        bool mIsBatching = false;

        bool mDebugMode = false;
    };
} // namespace RealmFortress
