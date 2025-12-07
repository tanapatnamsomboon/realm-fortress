/**
 * @file hex_map.cpp
 * @brief Hex map implementation with correct asset paths
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "hex_map.h"
#include "core/logger.h"
#include "renderer/renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>

namespace RealmFortress
{
    HexMap::HexMap()
    {
        LoadTileModels();
    }

    HexMap::~HexMap()
    {
        Clear();
    }

    void HexMap::Generate(i32 width, i32 height)
    {
        Clear();

        RF_CORE_INFO("Generating hex map ({0}x{1})...", width, height);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution dis(0, 10);

        for (i32 q = 0; q < width; q++)
        {
            for (i32 r = 0; r < height; r++)
            {
                HexCoordinate coord(q, r);

                HexTileType type = HexTileType::Grass;
                if (dis(gen) < 2)
                {
                    type = HexTileType::Water;
                }

                SetTile(coord, type, 0);
            }
        }

        RF_CORE_INFO("Generated {0} hex tiles", GetTileCount());
    }

    void HexMap::GenerateFromNoise(i32 width, i32 height, u32 seed)
    {
        RF_CORE_INFO("Generating hex map from noise: {}x{} (seed: {})", width, height, seed);
        Clear();

        // TODO: Implement proper noise-based generation
        // For now, use simple random generation
        Generate(width, height);
    }

    void HexMap::Clear()
    {
        mTiles.clear();
    }

    void HexMap::SetTile(const HexCoordinate& coord, HexTileType type, i32 elevation)
    {
        mTiles[coord] = HexTile(coord, type, elevation);
    }

    void HexMap::SetTileElevation(const HexCoordinate& coord, i32 elevation)
    {
        if (auto* tile = GetTile(coord))
            tile->Elevation = elevation;
    }

    void HexMap::SetTileVariation(const HexCoordinate& coord, u8 variation)
    {
        if (auto* tile = GetTile(coord))
            tile->Variation = variation;
    }

    HexTile* HexMap::GetTile(const HexCoordinate& coord)
    {
        auto it = mTiles.find(coord);
        return (it != mTiles.end()) ? &it->second : nullptr;
    }

    const HexTile* HexMap::GetTile(const HexCoordinate& coord) const
    {
        auto it = mTiles.find(coord);
        return (it != mTiles.end()) ? &it->second : nullptr;
    }

    bool HexMap::HasTile(const HexCoordinate& coord) const
    {
        return mTiles.contains(coord);
    }

    void HexMap::RemoveTile(const HexCoordinate& coord)
    {
        mTiles.erase(coord);
    }

    void HexMap::Draw(const Ref<Shader>& shader)
    {
        if (!mModelsLoaded)
        {
            RF_CORE_WARN("Models not loaded, cannot draw hex map");
            return;
        }

        // Group tiles by type for efficient rendering
        std::unordered_map<HexTileType, std::vector<glm::mat4>> transformsByType;

        for (const auto& [coord, tile] : mTiles)
        {
            if (tile.Type == HexTileType::None)
                continue;

            glm::vec3 position = coord.ToWorldPosition(tile.Elevation);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

            transformsByType[tile.Type].push_back(transform);
        }

        // Draw each tile type in batches
        for (const auto& [type, transforms] : transformsByType)
        {
            HexTile dummyTile;
            dummyTile.Type = type;

            Ref<Model> model = GetModelForTile(dummyTile);
            if (!model)
                continue;

            for (const auto& transform : transforms)
            {
                model->Draw(shader, transform);
            }
        }
    }

    void HexMap::DrawTile(const HexCoordinate& coord, const Ref<Shader>& shader)
    {
        const HexTile* tile = GetTile(coord);
        if (!tile || tile->Type == HexTileType::None)
            return;

        Ref<Model> model = GetModelForTile(*tile);
        if (!model)
            return;

        glm::vec3 position = coord.ToWorldPosition(tile->Elevation);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

        model->Draw(shader, transform);
    }

    void HexMap::BeginBatchRender()
    {
        mIsBatching = true;
        mBatchData.clear();

        for (const auto& [coord, tile] : mTiles)
        {
            if (tile.Type == HexTileType::None)
                continue;

            glm::vec3 position = coord.ToWorldPosition(tile.Elevation);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

            mBatchData[tile.Type].Type = tile.Type;
            mBatchData[tile.Type].Transforms.push_back(transform);
        }
    }

    void HexMap::EndBatchRender(const Ref<Shader>& shader)
    {
        if (!mIsBatching)
            return;

        for (auto& [type, batch] : mBatchData)
        {
            HexTile dummyTile;
            dummyTile.Type = type;

            Ref<Model> model = GetModelForTile(dummyTile);
            if (!model)
                continue;

            for (const auto& transform : batch.Transforms)
            {
                model->Draw(shader, transform);
            }
        }

        mIsBatching = false;
        mBatchData.clear();
    }

    std::vector<HexCoordinate> HexMap::GetTilesInRange(const HexCoordinate& center, i32 range) const
    {
        std::vector<HexCoordinate> results;
        auto area = HexCoordinate::GetArea(center, range);

        for (const auto& coord : area)
        {
            if (HasTile(coord))
                results.push_back(coord);
        }

        return results;
    }

    std::vector<HexTile*> HexMap::GetTilesOfType(HexTileType type)
    {
        std::vector<HexTile*> results;

        for (auto& tile : mTiles | std::views::values)
        {
            if (tile.Type == type)
                results.push_back(&tile);
        }

        return results;
    }

    void HexMap::GetBounds(HexCoordinate& min, HexCoordinate& max) const
    {
        if (mTiles.empty())
        {
            min = max = HexCoordinate(0, 0);
            return;
        }

        auto it = mTiles.begin();
        min = max = it->first;

        for (const auto& coord : mTiles | std::views::keys)
        {
            min.q = std::min(min.q, coord.q);
            min.r = std::min(min.r, coord.r);
            max.q = std::max(max.q, coord.q);
            max.r = std::max(max.r, coord.r);
        }
    }

    glm::vec2 HexMap::GetWorldBounds() const
    {
        HexCoordinate min, max;
        GetBounds(min, max);

        glm::vec3 minWorld = min.ToWorldPosition();
        glm::vec3 maxWorld = max.ToWorldPosition();

        return glm::vec2(
            std::abs(maxWorld.x - minWorld.x),
            std::abs(maxWorld.z - minWorld.z)
        );
    }

    std::vector<HexCoordinate> HexMap::FindPath(const HexCoordinate& start, const HexCoordinate& end)
    {
        // TODO: Implement proper A* pathfinding algorithm
        // For now, return a simple straight line
        return start.LineTo(end);
    }

    bool HexMap::IsWalkable(const HexCoordinate& coord) const
    {
        const HexTile* tile = GetTile(coord);
        return tile && tile->Walkable;
    }

    bool HexMap::IsVisible(const HexCoordinate& coord) const
    {
        const HexTile* tile = GetTile(coord);
        return tile && tile->Visible;
    }

    const char* HexMap::TileTypeToString(HexTileType type)
    {
        switch (type)
        {
        case HexTileType::None:             return "None";
        case HexTileType::Grass:            return "Grass";
        case HexTileType::GrassBottom:      return "Grass Bottom";
        case HexTileType::GrassSlopedLow:   return "Grass Sloped Low";
        case HexTileType::GrassSlopedHigh:  return "Grass Sloped High";
        case HexTileType::Water:            return "Water";
        case HexTileType::Transition:       return "Transition";
        case HexTileType::CoastA:           return "Coast A";
        case HexTileType::CoastB:           return "Coast B";
        case HexTileType::CoastC:           return "Coast C";
        case HexTileType::CoastD:           return "Coast D";
        case HexTileType::CoastE:           return "Coast E";
        case HexTileType::Mountain:         return "Mountain";
        case HexTileType::Forest:           return "Forest";
        case HexTileType::Hill:             return "Hill";
        case HexTileType::RoadA:            return "Road A";
        default:                            return "Unknown";
        }
    }

    bool HexMap::IsTileTypeWalkable(HexTileType type)
    {
        switch (type)
        {
        case HexTileType::Water:
        case HexTileType::Mountain:
        case HexTileType::None:
            return false;
        default:
            return true;
        }
    }

    void HexMap::LoadTileModels()
    {
        RF_CORE_INFO("Loading hex tile models...");

        // Load the most common tiles first
        const std::vector<std::pair<HexTileType, std::string>> commonTileModels = {
            { HexTileType::Grass,            "assets/objects/tiles/base/hex_grass.gltf" },
            { HexTileType::Water,            "assets/objects/tiles/base/hex_water.gltf" },
            { HexTileType::GrassBottom,      "assets/objects/tiles/base/hex_grass_bottom.gltf" },
            { HexTileType::GrassSlopedLow,   "assets/objects/tiles/base/hex_grass_sloped_low.gltf" },
            { HexTileType::GrassSlopedHigh,  "assets/objects/tiles/base/hex_grass_sloped_high.gltf" },
            { HexTileType::Transition,       "assets/objects/tiles/base/hex_transition.gltf" },
        };

        for (const auto& [type, path] : commonTileModels)
        {
            try
            {
                auto model = CreateRef<Model>(path);
                mModelCache[path] = model;
                RF_CORE_INFO("Loaded tile model: {}", path);
            }
            catch (const std::exception& e)
            {
                RF_CORE_ERROR("Failed to load tile model {}: {}", path, e.what());
            }
        }

        mModelsLoaded = true;
        RF_CORE_INFO("Tile models loaded: {}", mModelCache.size());
    }

    Ref<Model> HexMap::GetModelForTile(const HexTile& tile)
    {
        std::string path = GetModelPath(tile.Type, tile.Variation);

        auto it = mModelCache.find(path);
        if (it != mModelCache.end())
            return it->second;

        // Try to load the model on-demand
        try
        {
            auto model = CreateRef<Model>(path);
            mModelCache[path] = model;
            RF_CORE_INFO("Loaded tile model on-demand: {}", path);
            return model;
        }
        catch (const std::exception& e)
        {
            RF_CORE_ERROR("Failed to load model on-demand: {} - {}", path, e.what());

            // Fall back to grass tile if available
            auto fallback = mModelCache.find("assets/objects/tiles/base/hex_grass.gltf");
            if (fallback != mModelCache.end())
                return fallback->second;

            return nullptr;
        }
    }

    std::string HexMap::GetModelPath(HexTileType type, u8 variation)
    {
        switch (type)
        {
        // Base terrain tiles
        case HexTileType::Grass:
            return "assets/objects/tiles/base/hex_grass.gltf";
        case HexTileType::GrassBottom:
            return "assets/objects/tiles/base/hex_grass_bottom.gltf";
        case HexTileType::GrassSlopedLow:
            return "assets/objects/tiles/base/hex_grass_sloped_low.gltf";
        case HexTileType::GrassSlopedHigh:
            return "assets/objects/tiles/base/hex_grass_sloped_high.gltf";
        case HexTileType::Water:
            return "assets/objects/tiles/base/hex_water.gltf";
        case HexTileType::Transition:
            return "assets/objects/tiles/base/hex_transition.gltf";

        // Coast tiles
        case HexTileType::CoastA:
            return "assets/objects/tiles/coast/hex_coast_A.gltf";
        case HexTileType::CoastB:
            return "assets/objects/tiles/coast/hex_coast_B.gltf";
        case HexTileType::CoastC:
            return "assets/objects/tiles/coast/hex_coast_C.gltf";
        case HexTileType::CoastD:
            return "assets/objects/tiles/coast/hex_coast_D.gltf";
        case HexTileType::CoastE:
            return "assets/objects/tiles/coast/hex_coast_E.gltf";

        // River tiles
        case HexTileType::RiverStraight:
            return "assets/objects/tiles/rivers/hex_river_A.gltf";
        case HexTileType::RiverStraightCurvy:
            return "assets/objects/tiles/rivers/hex_river_A_curvy.gltf";
        case HexTileType::RiverCurveB:
            return "assets/objects/tiles/rivers/hex_river_B.gltf";
        case HexTileType::RiverCurveC:
            return "assets/objects/tiles/rivers/hex_river_C.gltf";
        case HexTileType::RiverForkD:
            return "assets/objects/tiles/rivers/hex_river_D.gltf";
        case HexTileType::RiverForkE:
            return "assets/objects/tiles/rivers/hex_river_E.gltf";
        case HexTileType::RiverForkF:
            return "assets/objects/tiles/rivers/hex_river_F.gltf";
        case HexTileType::RiverForkG:
            return "assets/objects/tiles/rivers/hex_river_G.gltf";
        case HexTileType::RiverJunctionH:
            return "assets/objects/tiles/rivers/hex_river_H.gltf";
        case HexTileType::RiverJunctionI:
            return "assets/objects/tiles/rivers/hex_river_I.gltf";
        case HexTileType::RiverJunctionJ:
            return "assets/objects/tiles/rivers/hex_river_J.gltf";
        case HexTileType::RiverJunctionK:
            return "assets/objects/tiles/rivers/hex_river_K.gltf";
        case HexTileType::RiverEndL:
            return "assets/objects/tiles/rivers/hex_river_L.gltf";
        case HexTileType::RiverCrossingA:
            return "assets/objects/tiles/rivers/hex_river_crossing_A.gltf";
        case HexTileType::RiverCrossingB:
            return "assets/objects/tiles/rivers/hex_river_crossing_B.gltf";

        // Road tiles
        case HexTileType::RoadA:
            return "assets/objects/tiles/roads/hex_road_A.gltf";
        case HexTileType::RoadASlopedLow:
            return "assets/objects/tiles/roads/hex_road_A_sloped_low.gltf";
        case HexTileType::RoadASlopedHigh:
            return "assets/objects/tiles/roads/hex_road_A_sloped_high.gltf";
        case HexTileType::RoadB:
            return "assets/objects/tiles/roads/hex_road_B.gltf";
        case HexTileType::RoadC:
            return "assets/objects/tiles/roads/hex_road_C.gltf";
        case HexTileType::RoadD:
            return "assets/objects/tiles/roads/hex_road_D.gltf";
        case HexTileType::RoadE:
            return "assets/objects/tiles/roads/hex_road_E.gltf";
        case HexTileType::RoadF:
            return "assets/objects/tiles/roads/hex_road_F.gltf";
        case HexTileType::RoadG:
            return "assets/objects/tiles/roads/hex_road_G.gltf";
        case HexTileType::RoadH:
            return "assets/objects/tiles/roads/hex_road_H.gltf";
        case HexTileType::RoadI:
            return "assets/objects/tiles/roads/hex_road_I.gltf";
        case HexTileType::RoadJ:
            return "assets/objects/tiles/roads/hex_road_J.gltf";
        case HexTileType::RoadK:
            return "assets/objects/tiles/roads/hex_road_K.gltf";
        case HexTileType::RoadL:
            return "assets/objects/tiles/roads/hex_road_L.gltf";
        case HexTileType::RoadM:
            return "assets/objects/tiles/roads/hex_road_M.gltf";

        // Decorative tiles (these should probably be separate decoration objects)
        case HexTileType::Mountain:
            return "assets/objects/decoration/nature/mountain_A.gltf";
        case HexTileType::Forest:
            return "assets/objects/decoration/nature/trees_A_large.gltf";
        case HexTileType::Hill:
            return "assets/objects/decoration/nature/hill_single_A.gltf";

        default:
            RF_CORE_WARN("Unknown tile type: {}", static_cast<int>(type));
            return "assets/objects/tiles/base/hex_grass.gltf";
        }
    }
} // namespace RealmFortress
