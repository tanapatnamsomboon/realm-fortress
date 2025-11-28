/**
 * @file hex_coordinate.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/base.h"
#include <glm/glm.hpp>
#include <vector>

namespace RF
{
    struct HexCoordinate
    {
        i32 q;
        i32 r;

        HexCoordinate()
            : q(0), r(0)
        {
        }

        HexCoordinate(i32 q, i32 r)
            : q(q), r(r)
        {
        }

        i32 s() const { return -q - r; }

        glm::vec3 ToWorldPosition() const;

        bool operator==(const HexCoordinate& other) const
        {
            return q == other.q && r == other.r;
        }

        bool operator!=(const HexCoordinate& other) const
        {
            return !(*this == other);
        }

        HexCoordinate operator+(const HexCoordinate& other) const
        {
            return HexCoordinate(q + other.q, r + other.r);
        }

        HexCoordinate operator-(const HexCoordinate& other) const
        {
            return HexCoordinate(q - other.q, r - other.r);
        }

        static i32 Distance(const HexCoordinate& a, const HexCoordinate& b);

        HexCoordinate GetNeighbor(i32 direction) const;

        std::vector<HexCoordinate> GetNeighbors() const;

        static const HexCoordinate Directions[6];
    };
} // namespace RF

namespace std
{
    template<>
    struct hash<RF::HexCoordinate>
    {
        size_t operator()(const RF::HexCoordinate& coord) const noexcept
        {
            return hash<RF::i32>()(coord.q) ^ (hash<RF::i32>()(coord.r) << 1);
        }
    };
} // namespace std
