/**
 * @file coordinate.h
 * @brief
 * @date 12/7/2025
 */

#pragma once

#include "core/base.h"
#include <glm/glm.hpp>

namespace RealmFortress
{
    enum class Direction : u8
    {
        East = 0,
        NorthEast,
        NorthWest,
        West,
        SouthWest,
        SouthEast,

        Count
    };

    struct Coordinate
    {
        int Q{};
        int R{};

        constexpr Coordinate() = default;
        constexpr Coordinate(int q, int r)
            : Q(q), R(r)
        {
        }

        i32 S() const { return -Q - R; }

        glm::vec3 ToWorldPosition() const noexcept;
        glm::vec3 ToWorldPosition(i32 elevation) const noexcept;
        glm::vec3 ToWorldPosition(f32 height) const noexcept;
        static Coordinate FromWorldPosition(const glm::vec3& world_position) noexcept;

        i32 DistanceTo(const Coordinate& other) const noexcept;

        Coordinate GetNeighbor(Direction dir) const noexcept;
        std::vector<Coordinate> GetNeighbors() const noexcept;

        std::vector<Coordinate> LineTo(const Coordinate& target) const;

        bool operator==(const Coordinate& other) const { return Q == other.Q && R == other.R; }
        bool operator!=(const Coordinate& other) const { return !(*this == other); }
        bool operator<(const Coordinate& other) const { return Q < other.Q && R < other.R; }
        Coordinate operator+(Coordinate other) const { return Coordinate(Q + other.Q, R + other.R); }
        Coordinate operator-(Coordinate other) const { return Coordinate(Q - other.Q, R - other.R); }
        Coordinate operator*(i32 scalar) const { return Coordinate(Q * scalar, R * scalar); }
        Coordinate operator+=(Coordinate other) { Q += other.Q; R += other.R; return *this; }
        Coordinate operator-=(Coordinate other) { Q -= other.Q; R -= other.R; return *this; }
    };
} // namespace RealmFortress

namespace std
{
    template<>
    struct hash<RealmFortress::Coordinate>
    {
        size_t operator()(const RealmFortress::Coordinate& coord) const noexcept
        {
            return hash<RealmFortress::i32>()(coord.Q) ^ (hash<RealmFortress::i32>()(coord.R) << 1);
        }
    };
}
