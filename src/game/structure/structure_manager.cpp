/**
 * @file structure_manager.cpp
 * @brief
 * @date 12/12/2025
 */

#include "core/pch.h"
#include "structure_manager.h"
#include "../../renderer/model_manager.h"

namespace RealmFortress
{
    bool StructureManager::CanPlaceStructure(const Coordinate& coord, const Map& map) const
    {
        if (!map.HasTile(coord))
            return false;
        auto* tile = map.GetTile(coord);
        if (!tile || !tile->IsWalkable())
            return false;
        if (HasStructure(coord))
            return false;
        return true;
    }

    bool StructureManager::PlaceStructure(StructureType type, const Coordinate& coord, f32 rotation, Map& map)
    {
        if (!CanPlaceStructure(coord, map))
            return false;

        Structure s(type, coord, 0, rotation);
        s.SetModel(GetOrLoadModel(type));
        mStructures.emplace(coord, std::move(s));

        RF_CORE_INFO("Placed structure {} at ({}, {})", (int)type, coord.Q, coord.R);
        return true;
    }

    bool StructureManager::RemoveStructure(const Coordinate& coord)
    {
        auto it = mStructures.find(coord);
        if (it != mStructures.end())
        {
            mStructures.erase(it);
            return true;
        }
        return false;
    }

    Structure* StructureManager::GetStructure(const Coordinate& coord)
    {
        auto it = mStructures.find(coord);
        return it != mStructures.end() ? &it->second : nullptr;
    }

    const Structure* StructureManager::GetStructure(const Coordinate& coord) const
    {
        auto it = mStructures.find(coord);
        return it != mStructures.end() ? &it->second : nullptr;
    }

    bool StructureManager::HasStructure(const Coordinate& coord) const
    {
        return mStructures.contains(coord);
    }

    void StructureManager::Draw(const Ref<Shader>& shader) const
    {
        if (!shader)
            return;

        shader->Bind();
        for (const auto& s : mStructures | std::views::values)
        {
            auto model = s.GetModel();
            if (model)
                model->Draw(shader, s.GetTransform());
        }
    }

    void StructureManager::SetPreview(StructureType type, const Coordinate& coord, f32 rotation)
    {
        Structure preview(type, coord, 0, rotation);
        preview.SetModel(GetOrLoadModel(type));
        mPreviewStructure = std::move(preview);
    }

    void StructureManager::ClearPreview()
    {
        mPreviewStructure = std::nullopt;
    }

    void StructureManager::DrawPreview(const Ref<Shader>& shader, const Map& map, f32 time) const
    {
        if (!mPreviewStructure.has_value() || !shader)
            return;

        const auto& preview = mPreviewStructure.value();
        auto model = preview.GetModel();
        if (!model)
            return;

        shader->Bind();
        bool can_place = CanPlaceStructure(preview.GetCoordinate(), map);
        glm::vec3 preview_color = can_place ? glm::vec3(0.0f, 1.0f, 0.5f) : glm::vec3(1.0f, 0.0f, 0.0f);

        shader->SetFloat3("uHighlightColor", preview_color);
        shader->SetFloat("uHighlightIntensity", 0.4f);
        shader->SetFloat("uPulseTime", time);

        model->Draw(shader, preview.GetTransform());
    }

    Ref<Model> StructureManager::GetOrLoadModel(StructureType type) const
    {
        u64 key = MakeKey(type, 0);
        if (auto it = mStructureModelCache.find(key); it != mStructureModelCache.end())
            return it->second;

        std::string path = StructureTypeToModelPath(type);
        auto model = ModelManager::Load(path);
        if (!model)
            RF_CORE_WARN("Structure model load failed: {}", path);

        mStructureModelCache.emplace(key, model);
        return model;
    }

    u64 StructureManager::MakeKey(StructureType type, i32 variant)
    {
        return static_cast<u64>(type) | (static_cast<u64>(static_cast<u32>(variant)) << 32);
    }
} // namespace RealmFortress
