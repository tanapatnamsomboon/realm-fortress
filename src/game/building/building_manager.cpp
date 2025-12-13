/**
 * @file building_manager.cpp
 * @brief
 * @date 12/12/2025
 */

#include "core/pch.h"
#include "building_manager.h"
#include "core/logger.h"
#include "../../renderer/model_cache.h"

namespace RealmFortress
{
    bool BuildingManager::CanPlaceBuilding(const Coordinate& coord, const Map& map) const
    {
        if (!map.HasTile(coord))
            return false;

        auto* tile = map.GetTile(coord);
        if (!tile || !tile->IsWalkable())
            return false;

        if (HasBuilding(coord))
            return false;

        return true;
    }

    bool BuildingManager::PlaceBuilding(BuildingType type, Faction faction, const Coordinate& coord, Map& map)
    {
        if (!CanPlaceBuilding(coord, map))
            return false;

        Building building(type, faction, coord);
        building.SetModel(GetOrLoadModel(type, faction));
        mBuildings[coord] = std::move(building);

        RF_CORE_INFO("Placed {} ({}) at ({}, {})",
                   BuildingTypeToString(type),
                   FactionToString(faction),
                   coord.Q, coord.R);

        return true;
    }

    bool BuildingManager::RemoveBuilding(const Coordinate& coord)
    {
        auto it = mBuildings.find(coord);
        if (it != mBuildings.end())
        {
            mBuildings.erase(it);
            return true;
        }
        return false;
    }

    Building* BuildingManager::GetBuilding(const Coordinate& coord)
    {
        auto it = mBuildings.find(coord);
        return it != mBuildings.end() ? &it->second : nullptr;
    }

    const Building* BuildingManager::GetBuilding(const Coordinate& coord) const
    {
        auto it = mBuildings.find(coord);
        return it != mBuildings.end() ? &it->second : nullptr;
    }

    bool BuildingManager::HasBuilding(const Coordinate& coord) const
    {
        return mBuildings.contains(coord);
    }

    void BuildingManager::Draw(const Ref<Shader>& shader) const
    {
        if (!shader)
            return;

        shader->Bind();

        for (const auto& building : mBuildings | std::views::values)
        {
            auto model = building.GetModel();
            if (model)
            {
                model->Draw(shader, building.GetTransform());
            }
        }
    }

    void BuildingManager::SetPreview(BuildingType type, Faction faction, const Coordinate& coord)
    {
        Building preview(type, faction, coord);
        preview.SetModel(GetOrLoadModel(type, faction));
        mPreviewBuilding = std::move(preview);
    }

    void BuildingManager::ClearPreview()
    {
        mPreviewBuilding = std::nullopt;
    }

    void BuildingManager::DrawPreview(const Ref<Shader>& shader, const Map& map, f32 time) const
    {
        if (!mPreviewBuilding.has_value() || !shader)
            return;

        const auto& preview = mPreviewBuilding.value();
        auto model = preview.GetModel();

        if (!model)
            return;

        shader->Bind();

        bool can_place = CanPlaceBuilding(preview.GetCoordinate(), map);

        // green if valid, red if not
        glm::vec3 preview_color = can_place ? glm::vec3(0.0f, 1.0f, 0.5f) : glm::vec3(1.0f, 0.0f, 0.0f);

        shader->SetFloat3("uHighlightColor", preview_color);
        shader->SetFloat("uHighlightIntensity", 0.4f);
        shader->SetFloat("uPulseTime", time);

        model->Draw(shader, preview.GetTransform());
    }

    Ref<Model> BuildingManager::GetOrLoadModel(BuildingType type, Faction faction) const
    {
        u64 key = MakeKey(type, faction);
        if (auto it = mBuildingModelCache.find(key); it != mBuildingModelCache.end())
            return it->second;

        std::string model_path = BuildingTypeToModelPath(type, faction);
        auto model = ModelCache::Load(model_path);

        if (!model)
            RF_CORE_WARN("Failed to load building model: {}", model_path);

        mBuildingModelCache.emplace(key, model);
        return model;
    }

    u64 BuildingManager::MakeKey(BuildingType type, Faction faction)
    {
        return static_cast<u64>(type) | (static_cast<u64>(faction) << 32);
    }
} // namespace RealmFortress
