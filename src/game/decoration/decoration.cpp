/**
 * @file decoration.cpp
 * @brief
 * @date 12/13/2025
 */

#include "core/pch.h"
#include "decoration.h"

namespace RealmFortress
{
    Decoration::Decoration(DecorationType type, const Coordinate& coord, f32 rotation, f32 scale)
        : mType(type), mCoordinate(coord), mRotation(rotation), mScale(scale)
    {
    }

    glm::mat4 Decoration::GetTransform() const
    {
        glm::mat4 transform = glm::mat4(1.0f);

        glm::vec3 position = mCoordinate.ToWorldPosition();
        transform = glm::translate(transform, position);

        if (mRotation != 0.0f)
        {
            transform = glm::rotate(transform, glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        if (mScale != 1.0f)
        {
            transform = glm::scale(transform, glm::vec3(mScale));
        }

        return transform;
    }
} // namespace RealmFortress
