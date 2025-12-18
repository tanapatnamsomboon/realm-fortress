/**
 * @file coordinate.cpp
 * @brief
 * @date 12/7/2025
 */

#include "core/pch.h"
#include "coordinate.h"
#include "game/system/constants.h"

namespace RealmFortress
{
    constexpr Coordinate DirectionVector(Direction dir) noexcept
    {
        switch (dir)
        {
        case Direction::East:      return Coordinate( 1,  0);
        case Direction::NorthEast: return Coordinate( 1, -1);
        case Direction::NorthWest: return Coordinate( 0, -1);
        case Direction::West:      return Coordinate(-1,  0);
        case Direction::SouthWest: return Coordinate(-1,  1);
        case Direction::SouthEast: return Coordinate( 0,  1);
        default: return Coordinate(0, 0);
        }
    }

    glm::vec3 Coordinate::ToWorldPosition() const noexcept
    {
        f32 x = (SQRT3 * Q + SQRT3 / 2.0f * R) * MODEL_SIZE;
        f32 z = (             3.0f / 2.0f * R) * MODEL_SIZE;
        f32 y = BASE_HEIGHT;

        return glm::vec3(x, y, z);
    }

    glm::vec3 Coordinate::ToWorldPosition(i32 elevation) const noexcept
    {
        glm::vec3 pos = ToWorldPosition();
        pos.y = static_cast<f32>(elevation) * ELEVATION_STEP;

        return pos;
    }

    glm::vec3 Coordinate::ToWorldPosition(f32 height) const noexcept
    {
        glm::vec3 pos = ToWorldPosition();
        pos.y = height;

        return pos;
    }

    Coordinate Coordinate::FromWorldPosition(const glm::vec3& world_position) noexcept
    {
        f32 x = world_position.x / MODEL_SIZE;
        f32 z = world_position.z / MODEL_SIZE;

        f32 q = (SQRT3 / 3.0f * x - 1.0f / 3.0f * z);
        f32 r = (                   2.0f / 3.0f * z);

        f32 s = -q - r;

        i32 round_q = static_cast<i32>(std::round(q));
        i32 round_r = static_cast<i32>(std::round(r));
        i32 round_s = static_cast<i32>(std::round(s));

        f32 diff_q = std::abs(round_q - q);
        f32 diff_r = std::abs(round_r - r);
        f32 diff_s = std::abs(round_s - s);

        if (diff_q > diff_r && diff_q > diff_s)
        {
            round_q = -round_r - round_s;
        }
        else if (diff_r > diff_s)
        {
            round_r = -round_q - round_s;
        }

        return Coordinate(round_q, round_r);
    }

    i32 Coordinate::DistanceTo(const Coordinate& other) const noexcept
    {
        i32 dq = std::abs(Q - other.Q);
        i32 dr = std::abs(R - other.R);
        i32 ds = std::abs(S() - other.S());

        return (dq + dr + ds)  / 2.0f;
    }

    Coordinate Coordinate::GetNeighbor(Direction dir) const noexcept
    {
        return *this + DirectionVector(dir);
    }

    std::vector<Coordinate> Coordinate::GetNeighbors() const noexcept
    {
        std::vector<Coordinate> neighbors;
        neighbors.reserve(6);

        for (u8 i = 0; i < static_cast<u8>(Direction::Count); ++i)
        {
            neighbors.push_back(GetNeighbor(static_cast<Direction>(i)));
        }

        return neighbors;
    }

    std::vector<Coordinate> Coordinate::LineTo(const Coordinate& target) const
    {
        // TODO: implement

        return {};
    }
} // RealmFortress
