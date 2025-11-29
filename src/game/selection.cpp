/**
 * @file selection.cpp
 * @brief
 * @date 11/29/2025
 */

#include "core/pch.h"
#include "selection.h"
#include "game/hex_map.h"
#include "core/logger.h"

namespace RF
{
    SelectionManager::SelectionManager()
        : mPlacementMode(false), mPlacementBuildingType(BuildingType::None)
    {
    }

    std::optional<HexCoordinate> SelectionManager::GetHexAtScreenPosition(
        f32 screenX, f32 screenY,
        u32 screenWidth, u32 screenHeight,
        const PerspectiveCamera& camera,
        const HexMap& hexMap)
    {
        f32 ndcX = (2.0f * screenX) / screenWidth - 1.0f;
        f32 ndcY = 1.0f - (2.0f * screenY) / screenHeight;

        glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);

        glm::vec4 rayView = glm::inverse(camera.GetProjection()) * rayClip;
        rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);

        glm::vec3 rayWorld = glm::vec3(glm::inverse(camera.GetViewMatrix()) * rayView);
        rayWorld = glm::normalize(rayWorld);

        glm::vec3 rayOrigin = camera.GetPosition();

        if (std::abs(rayWorld.y) < 0.001f)
        {
            return std::nullopt;
        }

        f32 t = -rayOrigin.y / rayWorld.y;
        if (t < 0)
        {
            return std::nullopt;
        }

        glm::vec3 intersectionPoint = rayOrigin + rayWorld * t;

        f32 x = intersectionPoint.x;
        f32 z = intersectionPoint.z;

        f32 q = (x * std::sqrt(3.0f) / 3.0f - z / 3.0f) / 1.0f;
        f32 r = z * 2.0f / 3.0f / 1.0f;

        f32 s = -q - r;

        i32 rq = static_cast<i32>(std::round(q));
        i32 rr = static_cast<i32>(std::round(r));
        i32 rs = static_cast<i32>(std::round(s));

        f32 q_diff = std::abs(rq - q);
        f32 r_diff = std::abs(rr - r);
        f32 s_diff = std::abs(rs - s);

        if (q_diff > r_diff && q_diff > s_diff)
        {
            rq = -rr - rs;
        }
        else if (r_diff > s_diff)
        {
            rr = -rq - rs;
        }

        HexCoordinate coord(rq, rr);

        if (hexMap.HasTile(coord))
        {
            return coord;
        }

        return std::nullopt;
    }

    void SelectionManager::SelectHex(const HexCoordinate& coord)
    {
        mSelectedHex = coord;
        RF_TRACE("selected hex: ({0}, {1})", coord.q, coord.r);
    }

    void SelectionManager::ClearSelection()
    {
        mSelectedHex.reset();
    }

    void SelectionManager::SetPlacementMode(BuildingType type)
    {
        mPlacementMode = true;
        mPlacementBuildingType = type;
        RF_INFO("entered placement mode for {0}", BuildingTypeToString(type));
    }

    void SelectionManager::ClearPlacementMode()
    {
        mPlacementMode = false;
        mPlacementBuildingType = BuildingType::None;
    }
} // namespace RF
