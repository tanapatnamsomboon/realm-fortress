/**
 * @file structure.h
 * @brief
 * @date 12/12/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/model.h"
#include "game/system/coordinate.h"
#include "game/structure/structure_types.h"
#include <glm/glm.hpp>

namespace RealmFortress
{
    class Structure
    {
    public:
        Structure() = default;
        Structure(StructureType type, const Coordinate& coord, i32 variant = 0, f32 rotation = 0);

        StructureType GetType() const { return mType; }
        const Coordinate& GetCoordinate() const { return mCoordinate; }
        i32 GetVariant() const { return mVariant; }
        f32 GetRotation() const { return mRotation; }

        void SetModel(const Ref<Model>& model) { mModel = model; }
        Ref<Model> GetModel() const { return mModel; }

        glm::mat4 GetTransform() const;

    private:
        StructureType mType = StructureType::None;
        Coordinate    mCoordinate{};
        i32           mVariant{ 0 };
        f32           mRotation{ 0.0f };

        Ref<Model>    mModel;
    };
} // namespace RealmFortress
