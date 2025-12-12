/**
 * @file structure_manager.h
 * @brief
 * @date 12/12/2025
 */

#pragma once

#include "core/base.h"
#include "game/structure/structure.h"
#include "game/system/map.h"
#include "renderer/shader.h"
#include <unordered_map>
#include <optional>

namespace RealmFortress
{
    class StructureManager
    {
    public:
        StructureManager() = default;

        bool CanPlaceStructure(const Coordinate& coord, const Map& map) const;
        bool PlaceStructure(StructureType type, const Coordinate& coord, f32 rotation, Map& map);
        bool RemoveStructure(const Coordinate& coord);

        Structure* GetStructure(const Coordinate& coord);
        const Structure* GetStructure(const Coordinate& coord) const;
        bool HasStructure(const Coordinate& coord) const;

        void Draw(const Ref<Shader>& shader) const;

        void SetPreview(StructureType type, const Coordinate& coord, f32 rotation = 0.0f);
        void ClearPreview();
        bool HasPreview() const { return mPreviewStructure.has_value(); }
        void DrawPreview(const Ref<Shader>& shader, const Map& map, f32 time) const;

        usize GetStructureCount() const { return mStructures.size(); }

    private:
        Ref<Model> GetOrLoadModel(StructureType type) const;
        static u64 MakeKey(StructureType type, i32 variant = 0);

    private:
        std::unordered_map<Coordinate, Structure> mStructures;
        std::optional<Structure> mPreviewStructure;

        mutable std::unordered_map<u64, Ref<Model>> mStructureModelCache;
    };
} // namespace RealmFortress
