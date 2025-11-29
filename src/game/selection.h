/**
 * @file selection.h
 * @brief
 * @date 11/29/2025
 */

#pragma once

#include "core/base.h"
#include "game/hex_coordinate.h"
#include "game/building.h"
#include "scene/camera.h"
#include <optional>

namespace RF
{
    class HexMap;

    class SelectionManager
    {
    public:
        SelectionManager();

        std::optional<HexCoordinate> GetHexAtScreenPosition(
            f32 screenX, f32 screenY,
            u32 screenWidth, u32 screenHeight,
            const PerspectiveCamera& camera,
            const HexMap& hexMap
        );

        void SelectHex(const HexCoordinate& coord);
        void ClearSelection();

        bool HasSelection() const { return mSelectedHex.has_value(); }
        const HexCoordinate& GetSelectedHex() const { return mSelectedHex.value(); }

        void SetPlacementMode(BuildingType type);
        void ClearPlacementMode();
        bool IsInPlacementMode() const { return mPlacementMode; }
        BuildingType GetPlacementType() const { return mPlacementBuildingType; }

    private:
        std::optional<HexCoordinate> mSelectedHex;
        bool mPlacementMode;
        BuildingType mPlacementBuildingType;
    };
} // namespace RF
