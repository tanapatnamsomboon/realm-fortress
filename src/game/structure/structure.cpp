/**
 * @file structure.cpp
 * @brief
 * @date 12/12/2025
 */

#include "core/pch.h"
#include "structure.h"
#include <glm/gtc/matrix_transform.hpp>

namespace RealmFortress
{
    Structure::Structure(StructureType type, const Coordinate& coord, i32 variant, f32 rotation)
        : mType(type), mCoordinate(coord), mVariant(variant), mRotation(rotation)
    {
    }

    glm::mat4 Structure::GetTransform() const
    {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, mCoordinate.ToWorldPosition());
        transform = glm::rotate(transform, mRotation, glm::vec3(0.0f, 1.0f, 0.0f));

        return transform;
    }
} // namespace RealmFortress
