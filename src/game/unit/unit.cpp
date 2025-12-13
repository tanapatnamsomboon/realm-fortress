/**
 * @file unit.cpp
 * @brief
 * @date 12/14/2025
 */

#include "core/pch.h"
#include "unit.h"

namespace RealmFortress
{
    Unit::Unit(UnitType type, Faction faction, const Coordinate& coord)
        : mType(type)
        , mFaction(faction)
        , mCoordinate(coord)
        , mPosition(coord.ToWorldPosition())
    {
    }

    glm::mat4 Unit::GetTransform() const
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, mPosition);

        if (mRotation != 0.0f)
        {
            transform = glm::rotate(transform, glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        return transform;
    }

    void Unit::MoveTo(const Coordinate& target)
    {
        if (target == mCoordinate)
            return;

        mPath = mCoordinate.LineTo(target);
        mPathIndex = 0;
        mState = UnitState::Moving;
    }

    void Unit::Update(f32 deltaTime)
    {
        if (mState == UnitState::Moving)
        {
            UpdateMovement(deltaTime);
        }
    }

    void Unit::UpdateMovement(f32 deltaTime)
    {
        if (mPath.empty() || mPathIndex >= mPath.size())
        {
            mState = UnitState::Idle;
            return;
        }

        Coordinate targetCoord = mPath[mPathIndex];
        glm::vec3 targetPos = targetCoord.ToWorldPosition();

        glm::vec3 direction = targetPos - mPosition;
        f32 distance = glm::length(direction);

        if (distance < 0.1f)
        {
            mPosition = targetPos;
            mCoordinate = targetCoord;
            mPathIndex++;

            if (mPathIndex >= mPath.size())
            {
                mState = UnitState::Idle;
                mPath.clear();
            }
        }
        else
        {
            direction = glm::normalize(direction);
            mPosition += direction * mMoveSpeed * deltaTime;

            mRotation = glm::degrees(atan2(direction.x, direction.z));
        }
    }
} // namespace RealmFortress
