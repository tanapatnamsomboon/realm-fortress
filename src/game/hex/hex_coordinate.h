/**
 * @file hex_coordinate.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "core/base.h"
#include "hex_constants.h"
#include <glm/glm.hpp>
#include <vector>

namespace RealmFortress
{
    /**
     * @struct HexCoordinate
     * @brief Axial coordinate system for pointy-top hexagons
     *
     * Uses the axial (q, r) system where:
     * - q: column (diagonal axis)
     * - r: row (horizontal axis)
     * - s: implicit third axis (s = -q - r)
     */
    struct HexCoordinate
    {
        i32 q; // column
        i32 r; // row

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
        glm::vec3 ToWorldPosition(i32 elevation) const;
        glm::vec3 ToWorldPosition(f32 height) const;

        static i32 Distance(const HexCoordinate& a, const HexCoordinate& b);
        i32 DistanceTo(const HexCoordinate& other) const;

        HexCoordinate GetNeighbor(i32 direction) const;
        std::vector<HexCoordinate> GetNeighbors() const;

        std::vector<HexCoordinate> LineTo(const HexCoordinate& target) const;
        static std::vector<HexCoordinate> GetRing(const HexCoordinate& center, i32 radius);
        static std::vector<HexCoordinate> GetArea(const HexCoordinate& center, i32 radius);

        bool operator==(const HexCoordinate& other) const { return q == other.q && r == other.r; }
        bool operator!=(const HexCoordinate& other) const { return !(*this == other); }
        HexCoordinate operator+(const HexCoordinate& other) const { return HexCoordinate(q + other.q, r + other.r); }
        HexCoordinate operator-(const HexCoordinate& other) const { return HexCoordinate(q - other.q, r - other.r); }
        HexCoordinate operator*(i32 scalar) const { return HexCoordinate(q * scalar, r * scalar); }

        static const HexCoordinate Directions[6];
    };
} // namespace RealmFortress

namespace std
{
    template<>
    struct hash<RealmFortress::HexCoordinate>
    {
        size_t operator()(const RealmFortress::HexCoordinate& coord) const noexcept
        {
            return hash<RealmFortress::i32>()(coord.q) ^ (hash<RealmFortress::i32>()(coord.r) << 1);
        }
    };
} // namespace std
