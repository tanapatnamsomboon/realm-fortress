/**
 * @file hex_coordinate.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "hex_coordinate.h"
#include "core/logger.h"

namespace RealmFortress
{
    const HexCoordinate HexCoordinate::Directions[6] = {
        HexCoordinate(+1,  0),  // 0: east
        HexCoordinate(+1, -1),  // 1: southeast
        HexCoordinate( 0, -1),  // 2: southwest
        HexCoordinate(-1,  0),  // 3: west
        HexCoordinate(-1, +1),  // 4: northwest
        HexCoordinate( 0, +1)   // 5: northeast
    };

    glm::vec3 HexCoordinate::ToWorldPosition() const
    {
        using namespace Hex;

        f32 x = (SQRT3 * q + SQRT3 / 2.0f * r) * MODEL_SIZE;
        f32 z = (             3.0f / 2.0f * r) * MODEL_SIZE;
        f32 y = BASE_HEIGHT;

        return glm::vec3(x, y, z);
    }

    glm::vec3 HexCoordinate::ToWorldPosition(i32 elevation) const
    {
        glm::vec3 pos = ToWorldPosition();
        pos.y = static_cast<f32>(elevation) * Hex::ELEVATION_STEP;

        return pos;
    }

    glm::vec3 HexCoordinate::ToWorldPosition(f32 height) const
    {
        glm::vec3 pos = ToWorldPosition();
        pos.y = height;

        return pos;
    }

    i32 HexCoordinate::Distance(const HexCoordinate& a, const HexCoordinate& b)
    {
        return (std::abs(a.q - b.q) + std::abs(a.r - b.r) + std::abs(a.s() - b.s())) / 2;
    }

    i32 HexCoordinate::DistanceTo(const HexCoordinate& other) const
    {
        return Distance(*this, other);
    }

    HexCoordinate HexCoordinate::GetNeighbor(i32 direction) const
    {
        RF_CORE_ASSERT(direction >= 0 && direction < 6, "Invalid direction!");

        return *this + Directions[direction];
    }

    std::vector<HexCoordinate> HexCoordinate::GetNeighbors() const
    {
        std::vector<HexCoordinate> neighbors;
        neighbors.reserve(6);
        for (i32 i = 0; i < 6; i++)
            neighbors.push_back(GetNeighbor(i));

        return neighbors;
    }

    std::vector<HexCoordinate> HexCoordinate::LineTo(const HexCoordinate& target) const
    {
        i32 dist = DistanceTo(target);
        std::vector<HexCoordinate> results;
        results.reserve(dist + 1);

        if (dist == 0)
        {
            results.push_back(*this);
            return results;
        }

        for (i32 i = 0; i <= dist; ++i)
        {
            f32 t = static_cast<f32>(i) / static_cast<f32>(dist);

            f32 q_lerp = static_cast<f32>(q) + (static_cast<f32>(target.q) - static_cast<f32>(q)) * t;
            f32 r_lerp = static_cast<f32>(r) + (static_cast<f32>(target.r) - static_cast<f32>(r)) * t;
            f32 s_lerp = static_cast<f32>(s()) + (static_cast<f32>(target.s()) - static_cast<f32>(s())) * t;

            i32 q_round = static_cast<i32>(std::round(q_lerp));
            i32 r_round = static_cast<i32>(std::round(r_lerp));
            i32 s_round = static_cast<i32>(std::round(s_lerp));

            f32 q_diff = std::abs(q_round - q_lerp);
            f32 r_diff = std::abs(r_round - r_lerp);
            f32 s_diff = std::abs(s_round - s_lerp);

            if (q_diff > r_diff && q_diff > s_diff)
                q_round = -r_round - s_round;
            else if (r_diff > s_diff)
                r_round = -q_round - s_round;

            results.emplace_back(q_round, r_round);
        }

        return results;
    }

    std::vector<HexCoordinate> HexCoordinate::GetRing(const HexCoordinate& center, i32 radius)
    {
        std::vector<HexCoordinate> results;
        if (radius == 0)
        {
            results.push_back(center);
            return results;
        }

        results.reserve(radius * 6);

        HexCoordinate hex = center + (Directions[4] * radius);

        for (i32 dir = 0; dir < 6; ++dir)
        {
            for (i32 step = 0; step < radius; ++step)
            {
                results.push_back(hex);
                hex = hex.GetNeighbor(dir);
            }
        }

        return results;
    }

    std::vector<HexCoordinate> HexCoordinate::GetArea(const HexCoordinate& center, i32 radius)
    {
        std::vector<HexCoordinate> results;
        results.reserve((radius + 1) * (radius + 1) * 3);

        for (i32 q = -radius; q <= radius; ++q)
        {
            i32 r1 = std::max(-radius, q - radius);
            i32 r2 = std::min(radius, q + radius);
            for (i32 r = r1; r <= r2; ++r)
            {
                results.emplace_back(center.q + q, center.r + r);
            }
        }

        return results;
    }
} // namespace RealmFortress
