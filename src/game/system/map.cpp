/**
 * @file map.cpp
 * @brief
 * @date 12/9/2025
 */

#include "core/pch.h"
#include "map.h"
#include "game/perlin_noise.h"
#include "renderer/model_cache.h"

namespace RealmFortress
{
    void Map::GenerateFlat(i32 radius, TileType default_type)
    {
        Clear();

        for (i32 q = -radius; q <= radius; ++q)
        {
            i32 r1 = std::max(-radius, -q - radius);
            i32 r2 = std::min(radius, -q + radius);

            for (i32 r = r1; r <= r2; ++r)
            {
                AddTile(Coordinate(q, r), default_type, 0);
            }
        }

        RF_CORE_INFO("Generated flat map with radius {} ({} tiles)", radius, mTiles.size());
    }

    void Map::GenerateRectangle(i32 width, i32 height, TileType default_type)
    {
        Clear();

        for (i32 q = 0; q < width; ++q)
        {
            for (i32 r = 0; r < height; ++r)
            {
                AddTile(Coordinate(q, r), default_type, 0);
            }
        }

        RF_CORE_INFO("Generated rectangular map {}x{} ({} tiles)", width, height, mTiles.size());
    }

    void Map::GenerateHexagon(i32 radius, TileType default_type)
    {
        Clear();

        for (i32 q = -radius; q <= radius; ++q)
        {
            i32 r1 = std::max(-radius, -q - radius);
            i32 r2 = std::min(radius, -q + radius);

            for (i32 r = r1; r <= r2; ++r)
            {
                AddTile(Coordinate(q, r), default_type, 0);
            }
        }

        RF_CORE_INFO("Generated hexagonal map with radius {} ({} tiles)", radius, mTiles.size());
    }

    struct NoiseConfig
    {
        f32 scale = 0.1f;              // Zoom level (smaller = more zoomed out)
        i32 octaves = 4;               // Detail layers
        f32 persistence = 0.5f;        // Roughness

        f32 water_threshold = 0.35f;    // % water
        f32 forest_threshold = 0.55f;   // % forest
        f32 mountain_threshold = 0.75f; // % mountain
    };

    void Map::GenerateWithNoise(i32 radius, u32 seed)
    {
        Clear();

        PerlinNoise noise(seed);
        std::mt19937 rng(seed);
        std::uniform_real_distribution dist(0.0f, 1.0f);

        f32 scale = 0.1f;
        i32 octaves = 4;
        f32 persistence = 0.5f;

        f32 water_threshold = 0.30f;
        f32 forest_threshold = 0.60f;
        f32 mountain_threshold = 0.70f;

        for (i32 q = -radius; q <= radius; ++q)
        {
            i32 r1 = std::max(-radius, -q - radius);
            i32 r2 = std::min(radius, -q + radius);

            for (i32 r = r1; r <= r2; ++r)
            {
                Coordinate coord(q, r);

                f32 noise_value = noise.OctaveNoise(q * scale, r * scale, octaves, persistence);

                TileType tile_type;
                DecorationType decoration = DecorationType::None;
                i32 elevation = 0;

                if (noise_value < water_threshold)
                {
                    tile_type = TileType::Water;
                    elevation = 0;
                }
                else if (noise_value >= mountain_threshold)
                {
                    tile_type = TileType::Grass;
                    elevation = 0;

                    f32 rand = dist(rng);
                    if (rand < 0.33f)
                        decoration = DecorationType::Mountain;
                    else if (rand < 0.66f)
                        decoration = DecorationType::MountainGrass;
                    else
                        decoration = DecorationType::MountainGrassTree;
                }
                else if (noise_value >= forest_threshold)
                {
                    tile_type = TileType::Grass;
                    elevation = 0;

                    f32 rand = dist(rng);
                    if (rand < 0.33f)
                        decoration = DecorationType::TreeSmall;
                    else if (rand < 0.66f)
                        decoration = DecorationType::TreeMedium;
                    else
                        decoration = DecorationType::TreeLarge;
                }
                else
                {
                    tile_type = TileType::Grass;
                    elevation = 0;
                }

                AddTile(coord, tile_type, elevation);

                if (decoration != DecorationType::None)
                {
                    Tile* tile = GetTile(coord);
                    if (tile)
                    {
                        tile->SetDecoration(decoration);
                    }
                }
            }
        }

        RF_CORE_INFO("Generated noise-based map with radius {} ({} tiles)", radius, mTiles.size());
    }

    void Map::AddTile(const Coordinate& coord, TileType type, i32 elevation)
    {
        Tile tile(coord, type, "", elevation);

        std::string model_path = TileTypeToModelPath(type);
        auto model = ModelCache::Load(model_path);

        tile.SetModel(model);
        mTiles[coord] = std::move(tile);
    }

    void Map::RemoveTile(const Coordinate& coord)
    {
        mTiles.erase(coord);
    }

    void Map::Clear()
    {
        mTiles.clear();
    }

    Tile* Map::GetTile(const Coordinate& coord)
    {
        auto it = mTiles.find(coord);
        return it != mTiles.end() ? &it->second : nullptr;
    }

    const Tile* Map::GetTile(const Coordinate& coord) const
    {
        auto it = mTiles.find(coord);
        return it != mTiles.end() ? &it->second : nullptr;
    }

    bool Map::HasTile(const Coordinate& coord) const
    {
        return mTiles.contains(coord);
    }

    std::vector<Tile*> Map::GetNeighbors(const Coordinate& coord)
    {
        std::vector<Tile*> neighbors;
        auto neighborCoords = coord.GetNeighbors();

        for (const auto& nc : neighborCoords)
        {
            if (auto* tile = GetTile(nc))
            {
                neighbors.push_back(tile);
            }
        }

        return neighbors;
    }

    std::vector<Tile*> Map::GetTilesInRadius(const Coordinate& center, i32 radius)
    {
        std::vector<Tile*> tiles;

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

    std::vector<Tile*> Map::GetTilesInLine(const Coordinate& start, const Coordinate& end)
    {
        std::vector<Tile*> tiles;
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

        for (auto& tile : mTiles | std::views::values)
        {
            auto model = tile.GetModel();
            if (model)
            {
                model->Draw(shader, tile.GetTransform());
            }

            if (tile.GetDecoration() != DecorationType::None)
            {
                auto decoration_model = tile.GetDecorationModel();
                if (decoration_model)
                {
                    glm::mat4 transform = tile.GetTransform();
                    decoration_model->Draw(shader, transform);
                }
            }
        }
    }

    void Map::DrawWithHighlight(
        const Ref<Shader>& base_shader,
        const Ref<Shader>& highlight_shader,
        const std::unordered_set<Coordinate>& highlighted_hexes,
        const glm::vec3& highlight_color, f32 time
    )
    {
        if (!base_shader || !highlight_shader)
        {
            RF_CORE_ERROR("Cannot draw map: shader is null");
            return;
        }

        base_shader->Bind();

        for (auto& [coord, tile] : mTiles)
        {
            if (highlighted_hexes.contains(coord))
                continue;

            auto model = tile.GetModel();
            if (model)
            {
                model->Draw(base_shader, tile.GetTransform());
            }

            if (tile.GetDecoration() != DecorationType::None)
            {
                auto decoration_model = tile.GetDecorationModel();
                if (decoration_model)
                {
                    decoration_model->Draw(base_shader, tile.GetTransform());
                }
            }
        }

        if (!highlighted_hexes.empty())
        {
            highlight_shader->Bind();
            highlight_shader->SetFloat3("uHighlightColor", highlight_color);
            highlight_shader->SetFloat("uPulseTime", time);
            highlight_shader->SetFloat("uHighlightIntensity", 0.5f);

            for (const auto& coord : highlighted_hexes)
            {
                auto* tile = GetTile(coord);
                if (tile && tile->GetModel())
                {
                    tile->GetModel()->Draw(highlight_shader, tile->GetTransform());
                }
            }
        }
    }
} // namespace RealmFortress
