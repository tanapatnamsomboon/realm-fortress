/**
 * @file building.cpp
 * @brief
 * @date 12/16/2025
 */

#include "core/pch.h"
#include "building.h"

namespace RealmFortress
{
    Building::Building(BuildingType type, const Coordinate& coord)
        : mType(type), mCoord(coord)
    {
    }

    glm::mat4 Building::GetTransform() const
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, mCoord.ToWorldPosition());
        return transform;
    }
} // namespace RealmFortress
