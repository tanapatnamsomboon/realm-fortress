/**
 * @file hex_utils.cpp
 * @brief
 * @date 12/2/2025
 */

#include "hex_utils.h"
#include "scene/camera.h"

namespace RealmFortress::HexUtils
{
    HexCoordinate WorldToHex(const glm::vec3& worldPos)
    {
        using namespace Hex;

        f32 x = worldPos.x / MODEL_SIZE;
        f32 y = worldPos.y / MODEL_SIZE;
        f32 q = (SQRT3 / 3.0f * x - 1.0f / 3.0f * y);
        f32 r = (                   2.0f / 3.0f * y);
        f32 s = -q - r;

        i32 q_round = static_cast<i32>(std::round(q));
        i32 r_round = static_cast<i32>(std::round(r));
        i32 s_round = static_cast<i32>(std::round(s));

        f32 q_diff = std::abs(q_round - q);
        f32 r_diff = std::abs(r_round - r);
        f32 s_diff = std::abs(s_round - s);

        if (q_diff > r_diff && q_diff > s_diff)
            q_round = -r_round - s_round;
        else if (r_diff > s_diff)
            r_round = -q_round - s_round;

        return HexCoordinate(q_round, r_round);
    }

    std::optional<HexCoordinate> ScreenToHex(const glm::vec2& screenPos, const class Camera& camera, f32 groundHeight)
    {
        return std::nullopt;
    }

    std::array<glm::vec3, 6> GetHexCorners(const HexCoordinate& coord, f32 elevation)
    {
        using namespace Hex;

        glm::vec3 center = coord.ToWorldPosition(elevation);
        std::array<glm::vec3, 6> corners;

        for (i32 i = 0; i < 6; ++i)
        {
            f32 angle_deg = CORNER_ANGLE_OFFSET + CORNER_ANGLE_STEP * static_cast<f32>(i);
            f32 angle_rad = glm::radians(angle_deg);

            f32 x = MODEL_SIZE * std::cos(angle_rad);
            f32 z = MODEL_SIZE * std::sin(angle_rad);

            corners[i] = center + glm::vec3(x, 0.0f, z);
        }

        return corners;
    }

    glm::vec3 GetHexCenter(const glm::vec3& worldPos, f32 elevation)
    {
        HexCoordinate hex = WorldToHex(worldPos);
        glm::vec3 center = hex.ToWorldPosition(elevation);

        return center;
    }

    bool IsPointInHex(const glm::vec3& worldPos, const HexCoordinate& hex)
    {
        HexCoordinate nearestHex = WorldToHex(worldPos);
        return nearestHex == hex;
    }

    bool AreAdjacent(const HexCoordinate& a, const HexCoordinate& b)
    {
        return HexCoordinate::Distance(a, b) == 1;
    }

    i32 GetDirectionTo(const HexCoordinate& from, const HexCoordinate& to)
    {
        HexCoordinate diff = to - from;

        for (i32 i = 0; i < 6; ++i)
        {
            if (diff == HexCoordinate::Directions[i])
                return i;
        }

        return -1;
    }

    HexCoordinate RotateAround(const HexCoordinate& coord, const HexCoordinate& center, i32 steps)
    {
        HexCoordinate offset = coord - center;
        i32 q = offset.q;
        i32 r = offset.r;
        i32 s = offset.s();

        steps = ((steps % 6) + 6) % 6;

        for (i32 i = 0; i < steps; ++i)
        {
            i32 temp_q = q;
            i32 temp_r = r;
            i32 temp_s = s;

            q = -temp_s;
            r = -temp_q;
            s = -temp_r;
        }

        return HexCoordinate(q, r) + center;
    }

    std::vector<HexCoordinate> LineTrace(const HexCoordinate& start, const HexCoordinate& end)
    {
        return start.LineTo(end);
    }
}
