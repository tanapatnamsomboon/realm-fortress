/**
 * @file hex_pathfinding.h
 * @brief
 * @date 12/6/2025
 */

#pragma once

#include "hex_coordinate.h"
#include <vector>
#include <functional>

namespace RealmFortress
{
    class HexMap;

    /**
     * @class HexPathfinder
     * @brief A* pathfinding implementation for hexagonal grids
     */
    class HexPathfinder
    {
    public:
        using CostFunction = std::function<f32(const HexCoordinate& from, const HexCoordinate& to)>;
        using WalkableFunction = std::function<bool(const HexCoordinate&)>;

        static std::vector<HexCoordinate> FindPath(
            const HexCoordinate& start,
            const HexCoordinate& goal,
            const WalkableFunction& isWalkable,
            const CostFunction& getCost = nullptr);

        static std::vector<HexCoordinate> FindPath(
            const HexCoordinate& start,
            const HexCoordinate& goal,
            const HexMap& map);

    private:
        struct Node
        {
            HexCoordinate Coord;
            f32 GCost = 0.0f;
            f32 HCost = 0.0f;
            f32 FCost() const { return GCost + HCost; }
            HexCoordinate Parent = HexCoordinate(-999, -999);

            Node() = default;
            Node(const HexCoordinate& coord) : Coord(coord) {}
        };

        static f32 Heuristic(const HexCoordinate& a, const HexCoordinate& b);
        static std::vector<HexCoordinate> ReconstructPath(
            const std::unordered_map<HexCoordinate, Node>& nodes,
            const HexCoordinate& start,
            const HexCoordinate& goal);
    };
} // namespace RealmFortress
