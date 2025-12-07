/**
 * @file selection.cpp
 * @brief
 * @date 11/29/2025
 */

#include "core/pch.h"
#include "selection.h"
#include "hex/hex_map.h"
#include "core/logger.h"
#include "hex/hex_utils.h"

namespace RealmFortress
{
    SelectionManager::SelectionManager() = default;

    std::optional<HexCoordinate> SelectionManager::GetHexAtScreenPosition(
        f32 screenX, f32 screenY,
        u32 screenWidth, u32 screenHeight,
        const PerspectiveCamera& camera,
        const HexMap& map)
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

        constexpr f32 hsize = 1.155f;
        const f32 sqrt3 = std::sqrt(3.0f);

        f32 q = (x * sqrt3 / 3.0f - z / 3.0f) / hsize;
        f32 r = z * 2.0f / 3.0f / hsize;

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

        if (map.HasTile(coord))
        {
            return coord;
        }

        return std::nullopt;
    }

    void SelectionManager::SelectHex(const HexCoordinate& coord)
    {
        mSelectedHex = coord;
        mSelectionType = SelectionType::Hex;
        ClearUnitSelection();
        RF_TRACE("Selected hex: ({}, {})", coord.q, coord.r);
    }

    void SelectionManager::ClearSelection()
    {
        mSelectedHex.reset();
        mSelectionType = SelectionType::None;
        ClearUnitSelection();
    }

    void SelectionManager::SetPlacementMode(BuildingType type)
    {
        mPlacementMode = true;
        mPlacementBuildingType = type;
        ClearSelection();
        RF_INFO("Entered placement mode for {}", BuildingTypeToString(type));
    }

    void SelectionManager::ClearPlacementMode()
    {
        mPlacementMode = false;
        mPlacementBuildingType = BuildingType::None;
    }

    void SelectionManager::SelectUnit(Unit* unit)
    {
        if (!unit) return;

        ClearUnitSelection();
        mSelectedUnits.push_back(unit);
        unit->SetSelected(true);
        mSelectionType = SelectionType::Unit;

        RF_TRACE("Selected unit: {} at ({}, {})", unit->GetName(), unit->GetPosition().q, unit->GetPosition().r);
    }

    void SelectionManager::SelectUnits(const std::vector<Unit*>& units)
    {
        ClearUnitSelection();

        for (Unit* unit : units)
        {
            if (unit)
            {
                mSelectedUnits.push_back(unit);
                unit->SetSelected(true);
            }
        }

        if (!mSelectedUnits.empty())
        {
            mSelectionType = mSelectedUnits.size() == 1 ? SelectionType::Unit : SelectionType::MultipleUnits;
            RF_TRACE("Selected {} units", mSelectedUnits.size());
        }
    }

    void SelectionManager::AddUnitToSelection(Unit* unit)
    {
        if (!unit) return;

        auto it = std::ranges::find(mSelectedUnits, unit);
        if (it != mSelectedUnits.end())
            return;

        mSelectedUnits.push_back(unit);
        unit->SetSelected(true);
        mSelectionType = mSelectedUnits.size() == 1 ? SelectionType::Unit : SelectionType::MultipleUnits;
    }

    void SelectionManager::RemoveUnitFromSelection(Unit* unit)
    {
        if (!unit) return;

        auto it = std::ranges::find(mSelectedUnits, unit);
        if (it != mSelectedUnits.end())
        {
            unit->SetSelected(false);
            mSelectedUnits.erase(it);
        }

        if (mSelectedUnits.empty())
        {
            mSelectionType = SelectionType::None;
        }
        else if (mSelectedUnits.size() == 1)
        {
            mSelectionType = SelectionType::Unit;
        }
    }

    void SelectionManager::ClearUnitSelection()
    {
        for (Unit* unit : mSelectedUnits)
        {
            if (unit)
                unit->SetSelected(false);
        }
        mSelectedUnits.clear();
    }

    void SelectionManager::IssueUnitMoveOrder(const HexCoordinate& target, const HexMap& map)
    {
        for (Unit* unit : mSelectedUnits)
        {
            if (unit)
            {
                unit->MoveTo(target, map);
            }
        }

        RF_TRACE("Move order issued to {} units", mSelectedUnits.size());
    }

    void SelectionManager::IssueUnitAttackOrder(Unit* target)
    {
        if (!target) return;

        for (Unit* unit : mSelectedUnits)
        {
            if (unit)
            {
                unit->Attack(target);
            }
        }

        RF_TRACE("Attack order issued to {} units", mSelectedUnits.size());
    }
} // namespace RealmFortress
