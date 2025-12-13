/**
 * @file decoration.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "core/base.h"
#include "game/decoration/decoration_type.h"
#include "game/system/coordinate.h"
#include "renderer/model.h"
#include <glm/glm.hpp>

namespace RealmFortress
{
    class Decoration
    {
    public:
        Decoration() = default;
        Decoration(DecorationType type, const Coordinate& coord, f32 rotation = 0.0f, f32 scale = 1.0f);

        DecorationType GetType() const { return mType; }
        const Coordinate& GetCoordinate() const { return mCoordinate; }
        f32 GetRotation() const { return mRotation; }
        f32 GetScale() const { return mScale; }

        void SetModel(const Ref<Model>& model) { mModel = model; }
        Ref<Model> GetModel() const { return mModel; }

        glm::mat4 GetTransform() const;

        bool IsNature() const { return IsNatureDecoration(mType); }
        bool IsProp() const { return IsPropDecoration(mType); }

    private:
        DecorationType mType{ DecorationType::None };
        Coordinate mCoordinate{};
        f32 mRotation{ 0.0f };
        f32 mScale{ 1.0f };
        Ref<Model> mModel{ nullptr };
    };
} // namespace RealmFortress
