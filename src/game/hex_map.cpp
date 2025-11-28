/**
 * @file hex_map.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "hex_map.h"
#include "core/logger.h"
#include "renderer/renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>

namespace RF
{
    HexMap::HexMap()
    {
        LoadModels();
    }

    HexMap::~HexMap()
    {
        Clear();
    }

    void HexMap::Generate(i32 width, i32 height)
    {
        Clear();

        RF_CORE_INFO("generating hex map ({0}x{1})...", width, height);

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

        RF_CORE_INFO("generated {0} hex tiles", GetTileCount());
    }

    void HexMap::Clear()
    {
        mTiles.clear();
    }

    void HexMap::SetTile(const HexCoordinate& coord, HexTileType type, i32 elevation)
    {
        mTiles[coord] = HexTile(coord, type, elevation);
    }

    HexTile* HexMap::GetTile(const HexCoordinate& coord)
    {
        auto it = mTiles.find(coord);
        if (it != mTiles.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    const HexTile* HexMap::GetTile(const HexCoordinate& coord) const
    {
        auto it = mTiles.find(coord);
        if (it != mTiles.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    bool HexMap::HasTile(const HexCoordinate& coord) const
    {
        return mTiles.contains(coord);
    }

    void HexMap::Draw(const Ref<Shader>& shader)
    {
        if (!mModelsLoaded)
        {
            RF_CORE_WARN("Models not loaded, cannot draw hex map");
            return;
        }

        for (const auto& [coord, tile] : mTiles)
        {
            glm::vec3 worldPos = coord.ToWorldPosition();
            worldPos.y = tile.Elevation * 1.0f;

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);

            auto model = GetModelForTile(tile);
            if (model)
                model->Draw(shader, transform);
        }
    }

    void HexMap::LoadModels()
    {
        if (mModelsLoaded) return;

        RF_CORE_INFO("loading hex tile models...");

        try
        {
            mModels["hex_grass"] = CreateRef<Model>("assets/objects/tiles/base/hex_grass.gltf");
            mModels["hex_water"] = CreateRef<Model>("assets/objects/tiles/base/hex_water.gltf");

            RF_CORE_INFO("loaded {0} hex tile models", mModels.size());
            mModelsLoaded = true;
        }
        catch (const std::exception& e)
        {
            RF_CORE_ERROR("failed to load hex models: {0}", e.what());
        }
    }

    Ref<Model> HexMap::GetModelForTile(const HexTile& tile)
    {
        switch (tile.Type)
        {
        case HexTileType::Grass:
            return mModels["hex_grass"];
        case HexTileType::Water:
            return mModels["hex_water"];
        default:
            return mModels["hex_grass"]; // fallback
        }
    }
} // namespace RF
