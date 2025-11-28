/**
 * @file hex_coordinate.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "hex_coordinate.h"

namespace RF
{
    constexpr float hsize = 1.155f;
    constexpr float sqrt3 = 1.73205f;

    const HexCoordinate HexCoordinate::Directions[6] = {
        HexCoordinate(+1,  0),  // right
        HexCoordinate(+1, -1),  // top-right
        HexCoordinate( 0, -1),  // top-left
        HexCoordinate(-1,  0),  // left
        HexCoordinate(-1, +1),  // bottom-left
        HexCoordinate( 0, +1)   // bottom-right
    };

    glm::vec3 HexCoordinate::ToWorldPosition() const
    {
        f32 x = (sqrt3 * q + sqrt3 / 2 * r) * hsize;
        f32 z = (                 1.5f * r) * hsize;
        f32 y = 0.0f;

        return glm::vec3(x, y, z);
    }

    i32 HexCoordinate::Distance(const HexCoordinate& a, const HexCoordinate& b)
    {
        return (std::abs(a.q - b.q) + std::abs(a.r - b.r) + std::abs(a.s() - b.s())) / 2;
    }

    HexCoordinate HexCoordinate::GetNeighbor(i32 direction) const
    {
        RF_CORE_ASSERT(direction >= 0 && direction < 6, "invalid direction!");
        return *this + Directions[direction];
    }

    std::vector<HexCoordinate> HexCoordinate::GetNeighbors() const
    {
        std::vector<HexCoordinate> neighbors;
        neighbors.reserve(6);
        for (i32 i = 0; i < 6; i++)
        {
            neighbors.push_back(GetNeighbor(i));
        }
        return neighbors;
    }
} // namespace RF
