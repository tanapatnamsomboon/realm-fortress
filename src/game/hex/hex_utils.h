/**
 * @file hex_utils.h
 * @brief Utility functions for hex coordinate conversions and queries
 * @date 12/2/2025
 */

#pragma once

#include "hex_coordinate.h"
#include <glm/glm.hpp>
#include <optional>
#include <array>

namespace RealmFortress::HexUtils
{
    /**
     * @brief Convert world position to hex coordinate (pixel to hex)
     * @param worldPos World space position
     * @return Nearest hex coordinate
     */
    HexCoordinate WorldToHex(const glm::vec3& worldPos);

    /**
     * @brief Convert screen/mouse position to hex coordinate via ray casting
     * @param screenPos Screen position (from mouse)
     * @param camera Camera for raycasting
     * @param groundHeight Y-level of the ground plane to intersect with
     * @return Hex coordinate if a ray intersects ground plane, nullopt otherwise
     */
    std::optional<HexCoordinate> ScreenToHex(
        const glm::vec2&    screenPos,
        const class Camera& camera,
        f32                 groundHeight = 0.0f
    );

    /**
     * @brief Get hex corners in world space (for rendering outlines)
     * @param coord Hex coordinate
     * @param elevation Y-level to place corners at
     * @return Array of 6 corner positions
     *
     * For pointy-top hexagons:
     * - Corner 0: top (30°)
     * - Corner 1: top-right (90°)
     * - Corner 2: bottom-right (150°)
     * - Corner 3: bottom (210°)
     * - Corner 4: bottom-left (270°)
     * - Corner 5: top-left (330°)
     */
    std::array<glm::vec3, 6> GetHexCorners(
        const HexCoordinate& coord,
        f32                  elevation = 0.0f
    );

    /**
     * @brief Snap a world position to the nearest hex center
     * @param worldPos Any world position
     * @param elevation Y-level for the returned center
     * @return Center position of the nearest hex at specified elevation
     */
    glm::vec3 GetHexCenter(const glm::vec3& worldPos, f32 elevation = 0.0f);

    /**
     * @brief Check if a world point is inside a specific hex's bounds
     * @param worldPos World position to test
     * @param hex Hex coordinate to test against
     * @return True if worldPos is within the hex's area
     */
    bool IsPointInHex(const glm::vec3& worldPos, const HexCoordinate& hex);

    /**
     * @brief Check if two hexes are directly adjacent (distance = 1)
     * @param a First hex
     * @param b Second hex
     * @return True if hexes share an edge
     */
    bool AreAdjacent(const HexCoordinate& a, const HexCoordinate& b);

    /**
     * @brief Get direction index from one hex to an adjacent hex
     * @param from Starting hex
     * @param to Target hex (must be adjacent)
     * @return Direction index (0-5) or -1 if hexes are not adjacent
     *
     * Direction mapping:
     * - 0: East
     * - 1: Southeast
     * - 2: Southwest
     * - 3: West
     * - 4: Northwest
     * - 5: Northeast
     */
    i32 GetDirectionTo(const HexCoordinate& from, const HexCoordinate& to);

    /**
     * @brief Rotate a coordinate around a center point
     * @param coord Coordinate to rotate
     * @param center Center of rotation
     * @param steps Number of 60° rotations (positive = clockwise, negative = counter-clockwise)
     * @return Rotated coordinate
     */
    HexCoordinate RotateAround(
        const HexCoordinate& coord,
        const HexCoordinate& center,
        i32                  steps
    );

    /**
     * @brief Get all coordinates forming a line between two hexes
     * @param start Starting hex
     * @param end Ending hex
     * @return Vector of hex coordinates from start to end (inclusive)
     */
    std::vector<HexCoordinate> LineTrace(
        const HexCoordinate& start,
        const HexCoordinate& end
    );
}
