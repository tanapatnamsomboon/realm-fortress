/**
 * @file unit.h
 * @brief
 * @date 12/14/2025
 */

#pragma once

#include "core/base.h"
#include "game/unit/unit_types.h"
#include "game/system/coordinate.h"
#include "renderer/model.h"
#include <glm/glm.hpp>
#include <vector>

namespace RealmFortress
{
    class Unit
    {
    public:
        Unit() = default;
        Unit(UnitType type, Faction faction, const Coordinate& coord);

        UnitType GetType() const { return mType; }
        Faction GetFaction() const { return mFaction; }
        UnitState GetState() const { return mState; }

        const Coordinate& GetCoordinate() const { return mCoordinate; }
        const glm::vec3& GetPosition() const { return mPosition; }
        f32 GetRotation() const { return mRotation; }

        void SetModel(const Ref<Model>& model) { mModel = model; }
        Ref<Model> GetModel() const { return mModel; }

        glm::mat4 GetTransform() const;

        void MoveTo(const Coordinate& target);
        void Update(f32 deltaTime);
        bool IsMoving() const { return mState == UnitState::Moving; }

        i32 GetHealth() const { return mHealth; }
        i32 GetMaxHealth() const { return mMaxHealth; }
        bool IsAlive() const { return mHealth > 0; }

    private:
        void UpdateMovement(f32 deltaTime);

    private:
        UnitType mType{ UnitType::None };
        Faction mFaction{ Faction::Neutral };
        UnitState mState{ UnitState::Idle };

        Coordinate mCoordinate{};
        glm::vec3 mPosition{ 0.0f };
        f32 mRotation{ 0.0f };

        std::vector<Coordinate> mPath;
        usize mPathIndex{ 0 };
        f32 mMoveSpeed{ 3.0f };

        // Stats
        i32 mHealth{ 100 };
        i32 mMaxHealth{ 100 };

        Ref<Model> mModel{ nullptr };
    };
} // namespace RealmFortress
