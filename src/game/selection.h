/**
 * @file selection.h
 * @brief
 * @date 11/29/2025
 */

#pragma once

#include "core/base.h"
#include "game/hex/hex_coordinate.h"
#include "game/unit/unit.h"
#include "game/building/building.h"
#include "scene/camera.h"
#include <optional>

namespace RealmFortress
{
    class HexMap;

    enum class SelectionType
    {
        None,
        Hex,
        Building,
        Unit,
        MultipleUnits
    };

    class SelectionManager
    {
    public:
        SelectionManager();

        std::optional<HexCoordinate> GetHexAtScreenPosition(
            f32 screenX, f32 screenY,
            u32 screenWidth, u32 screenHeight,
            const PerspectiveCamera& camera,
            const HexMap& map
        );

        void SelectHex(const HexCoordinate& coord);
        void ClearSelection();

        bool HasSelection() const { return mSelectionType != SelectionType::None; }
        SelectionType GetSelectionType() const { return mSelectionType; }
        const HexCoordinate& GetSelectedHex() const { return mSelectedHex.value(); }

        void SetPlacementMode(BuildingType type);
        void ClearPlacementMode();
        bool IsInPlacementMode() const { return mPlacementMode; }
        BuildingType GetPlacementType() const { return mPlacementBuildingType; }

        void SelectUnit(Unit* unit);
        void SelectUnits(const std::vector<Unit*>& units);
        void AddUnitToSelection(Unit* unit);
        void RemoveUnitFromSelection(Unit* unit);
        void ClearUnitSelection();

        bool HasSelectedUnits() const { return !mSelectedUnits.empty(); }
        const std::vector<Unit*>& GetSelectedUnits() const { return mSelectedUnits; }
        Unit* GetPrimarySelectedUnit() const { return mSelectedUnits.empty() ? nullptr : mSelectedUnits[0]; }

        void IssueUnitMoveOrder(const HexCoordinate& target, const HexMap& map);
        void IssueUnitAttackOrder(Unit* target);

    private:
        std::optional<HexCoordinate> mSelectedHex;
        SelectionType mSelectionType = SelectionType::None;

        bool mPlacementMode = false;
        BuildingType mPlacementBuildingType = BuildingType::None;

        std::vector<Unit*> mSelectedUnits;
    };
} // namespace RealmFortress
