/**
 * @file decoration_manager.cpp
 * @brief
 * @date 12/13/2025
 */

#include "core/pch.h"
#include "decoration_manager.h"
#include "renderer/model_cache.h"

namespace RealmFortress
{
    bool DecorationManager::CanPlaceDecoration(const Coordinate& coord, const Map& map) const
    {
        return map.HasTile(coord);
    }

    bool DecorationManager::PlaceDecoration(DecorationType type, const Coordinate& coord, f32 rotation, f32 scale)
    {
        Decoration decoration(type, coord, rotation, scale);

        auto model = GetOrLoadModel(type);
        if (model)
        {
            decoration.SetModel(model);
            mDecorations.push_back(std::move(decoration));

            RF_CORE_INFO("Placed {} at ({}, {})", DecorationTypeToString(type), coord.Q, coord.R);
            return true;
        }

        return false;
    }

    bool DecorationManager::RemoveDecoration(usize index)
    {
        if (index >= mDecorations.size())
        {
            mDecorations.erase(mDecorations.begin() + index);
            return true;
        }
        return false;
    }

    void DecorationManager::Clear()
    {
        mDecorations.clear();
    }

    std::vector<Decoration*> DecorationManager::GetDecorationsAt(const Coordinate& coord)
    {
        std::vector<Decoration*> result;
        for (auto& decoration : mDecorations)
        {
            if (decoration.GetCoordinate() == coord)
            {
                result.push_back(&decoration);
            }
        }
        return result;
    }

    void DecorationManager::Draw(const Ref<Shader>& shader) const
    {
        if (!shader) return;

        shader->Bind();

        for (const auto& decoration : mDecorations)
        {
            auto model = decoration.GetModel();
            if (model)
            {
                model->Draw(shader, decoration.GetTransform());
            }
        }
    }

    void DecorationManager::SetPreview(DecorationType type, const Coordinate& coord, f32 rotation)
    {
        Decoration preview(type, coord, rotation);

        auto model = GetOrLoadModel(type);
        if (model)
        {
            preview.SetModel(model);
            mPreviewDecoration = std::move(preview);
        }
    }

    void DecorationManager::ClearPreview()
    {
        mPreviewDecoration = std::nullopt;
    }

    void DecorationManager::DrawPreview(const Ref<Shader>& shader, const Map& map, f32 time) const
    {
        if (!mPreviewDecoration.has_value() || !shader)
            return;

        const auto& preview = mPreviewDecoration.value();
        auto model = preview.GetModel();

        if (!model) return;

        shader->Bind();

        bool can_place = CanPlaceDecoration(preview.GetCoordinate(), map);
        glm::vec3 color = can_place ? glm::vec3(0.0f, 1.0f, 0.5f) : glm::vec3(1.0f, 0.0f, 0.0f);

        shader->SetFloat3("uHighlightColor", color);
        shader->SetFloat("uHighlightIntensity", 0.3f);
        shader->SetFloat("uPulseTime", time);

        model->Draw(shader, preview.GetTransform());
    }

    Ref<Model> DecorationManager::GetOrLoadModel(DecorationType type) const
    {
        u64 key = static_cast<u64>(type);

        auto it = mDecorationModelCache.find(key);
        if (it != mDecorationModelCache.end())
        {
            return it->second;
        }

        std::string path = DecorationTypeToModelPath(type);
        Ref<Model> model = ModelCache::Load(path);

        if (model)
        {
            mDecorationModelCache[key] = model;
        }

        return model;
    }
} // namespace RealmFortress
