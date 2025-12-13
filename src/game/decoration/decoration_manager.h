/**
 * @file decoration_manager.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "core/base.h"
#include "game/decoration/decoration.h"
#include "game/system/map.h"
#include "renderer/shader.h"
#include <vector>
#include <optional>

namespace RealmFortress
{
    class DecorationManager
    {
    public:
        DecorationManager() = default;

        bool CanPlaceDecoration(const Coordinate& coord, const Map& map) const;
        bool PlaceDecoration(DecorationType type, const Coordinate& coord, f32 rotation = 0.0f, f32 scale = 1.0f);
        bool RemoveDecoration(usize index);
        void Clear();

        std::vector<Decoration*> GetDecorationsAt(const Coordinate& coord);

        void Draw(const Ref<Shader>& shader) const;

        void SetPreview(DecorationType type, const Coordinate& coord, f32 rotation = 0.0f);
        void ClearPreview();
        bool HasPreview() const { return mPreviewDecoration.has_value(); }
        void DrawPreview(const Ref<Shader>& shader, const Map& map, f32 time) const;

        usize GetDecorationCount() const { return mDecorations.size(); }

    private:
        Ref<Model> GetOrLoadModel(DecorationType type) const;

    private:
        std::vector<Decoration> mDecorations;
        std::optional<Decoration> mPreviewDecoration;
        mutable std::unordered_map<u64, Ref<Model>> mDecorationModelCache;
    };
} // namespace RealmFortress
