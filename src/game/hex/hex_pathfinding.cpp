/**
 * @file hex_pathfinding.cpp
 * @brief
 * @date 12/6/2025
 */

#include "core/pch.h"
#include "hex_pathfinding.h"
#include "hex_map.h"

namespace RealmFortress
{
    std::vector<HexCoordinate> HexPathfinder::FindPath(
        const HexCoordinate& start,
        const HexCoordinate& goal,
        const WalkableFunction& isWalkable,
        const CostFunction& getCost)
    {
        if (start == goal)
            return { start };

        if (!isWalkable(start) || !isWalkable(goal))
            return {};

        auto costFunc = getCost ? getCost : [](const HexCoordinate& from, const HexCoordinate& to)
        {
            return 1.0f;
        };

        auto compare = [](const Node& a, const Node& b)
        {
            return a.FCost() > b.FCost();
        };
        std::priority_queue<Node, std::vector<Node>, decltype(compare)>openSet(compare);

        std::unordered_map<HexCoordinate, Node> allNodes;
        std::unordered_set<HexCoordinate> closedSet;

        Node startNode(start);
        startNode.GCost = 0.0f;
        startNode.HCost = Heuristic(start, goal);
        allNodes[start] = startNode;
        openSet.push(startNode);

        while (!openSet.empty())
        {
            Node current = openSet.top();
            openSet.pop();

            if (closedSet.contains(current.Coord))
                continue;

            if (current.Coord == goal)
            {
                return ReconstructPath(allNodes, start, goal);
            }

            closedSet.insert(current.Coord);

            for (const auto& neighbor : current.Coord.GetNeighbors())
            {
                if (closedSet.contains(neighbor) || !isWalkable(neighbor))
                    continue;

                f32 newGCost = current.GCost + costFunc(current.Coord, neighbor);

                auto it = allNodes.find(neighbor);
                if (it == allNodes.end() || newGCost < it->second.GCost)
                {
                    Node neighborNode(neighbor);
                    neighborNode.GCost = newGCost;
                    neighborNode.HCost = Heuristic(neighbor, goal);
                    neighborNode.Parent = current.Coord;

                    allNodes[neighbor] = neighborNode;
                    openSet.push(neighborNode);
                }
            }
        }

        return {};
    }

    std::vector<HexCoordinate> HexPathfinder::FindPath(
        const HexCoordinate& start,
        const HexCoordinate& goal,
        const HexMap& map)
    {
        auto isWalkable = [&map](const HexCoordinate& coord) {
            return map.IsWalkable(coord);
        };

        return FindPath(start, goal, isWalkable);
    }

    f32 HexPathfinder::Heuristic(const HexCoordinate& a, const HexCoordinate& b)
    {
        return static_cast<f32>(HexCoordinate::Distance(a, b));
    }

    std::vector<HexCoordinate> HexPathfinder::ReconstructPath(
        const std::unordered_map<HexCoordinate, Node>& nodes,
        const HexCoordinate& start,
        const HexCoordinate& goal)
    {
        std::vector<HexCoordinate> path;
        HexCoordinate current = goal;
        HexCoordinate invalid(-999, -999);

        while (current != start)
        {
            path.push_back(current);

            auto it = nodes.find(current);
            if (it == nodes.end() || it->second.Parent == invalid)
                break;

            current = it->second.Parent;
        }

        path.push_back(start);
        std::ranges::reverse(path);
        return path;
    }
}
