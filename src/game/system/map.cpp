/**
 * @file map.cpp
 * @brief
 * @date 12/9/2025
 */

#include "core/pch.h"
#include "map.h"

namespace RealmFortress
{
    void Map::GenerateFlat(i32 radius, TileType defaultType)
    {
        Clear();

        for (i32 q = -radius; q <= radius; ++q)
        {
            i32 r1 = std::max(-radius, -q - radius);
            i32 r2 = std::min(radius, -q + radius);

            for (i32 r = r1; r <= r2; ++r)
            {
                AddTile(Coordinate(q, r), defaultType, 0);
            }
        }

        RF_CORE_INFO("Generated flat map with radius {} ({} tiles)", radius, mTiles.size());
    }

    void Map::GenerateRectangle(i32 width, i32 height, TileType defaultType)
    {
        Clear();

        for (i32 q = 0; q < width; ++q)
        {
            for (i32 r = 0; r < height; ++r)
            {
                AddTile(Coordinate(q, r), defaultType, 0);
            }
        }

        RF_CORE_INFO("Generated rectangular map {}x{} ({} tiles)", width, height, mTiles.size());
    }

    void Map::GenerateHexagon(i32 radius, TileType defaultType)
    {
        Clear();

        for (i32 q = -radius; q <= radius; ++q)
        {
            i32 r1 = std::max(-radius, -q - radius);
            i32 r2 = std::min(radius, -q + radius);

            for (i32 r = r1; r <= r2; ++r)
            {
                AddTile(Coordinate(q, r), defaultType, 0);
            }
        }

        RF_CORE_INFO("Generated hexagonal map with radius {} ({} tiles)", radius, mTiles.size());
    }

    void Map::AddTile(const Coordinate& coord, TileType type, i32 elevation)
    {
        Tile tile(coord, type, elevation);

        std::string modelPath = TileTypeToModelPath(type);
        try
        {
            tile.SetModel(CreateRef<Model>(modelPath));
        }
        catch (const std::exception& e)
        {
            RF_CORE_WARN("Failed to load model for tile at ({}, {}): {}", coord.Q, coord.R, e.what());

            tile.SetModel(nullptr);
        }

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
        }
    }

    void Map::LoadModelsForAllTiles()
    {
        for (auto& [coord, tile] : mTiles)
        {
            std::string modelPath = TileTypeToModelPath(tile.GetType());
            try
            {
                tile.SetModel(CreateRef<Model>(modelPath));
            }
            catch (const std::exception& e)
            {
                RF_CORE_WARN("Failed to load model for tile at ({}, {}): {}", coord.Q, coord.R, e.what());
                tile.SetModel(nullptr);
            }
        }

        RF_CORE_INFO("Loaded models for {} tiles", mTiles.size());
    }
} // namespace RealmFortress
