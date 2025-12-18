/**
 * @file selection.h
 * @brief
 * @date 12/11/2025
 */

#pragma once

#include "core/base.h"
#include "game/system/coordinate.h"
#include <optional>

namespace RealmFortress
{
    class Selection
    {
    public:
        Selection() = default;

        void Select(const Coordinate& coord);
        void ClearSelection();
        bool HasSelection() const { return mSelected.has_value(); }
        std::optional<Coordinate> GetSelected() const { return mSelected; }
        bool IsSelected(const Coordinate& coord) const;

        void SetHovered(const Coordinate& coord);
        void ClearHover();
        bool HasHover() const { return mHovered.has_value(); }
        std::optional<Coordinate> GetHovered() const { return mHovered; }
        bool IsHovered(const Coordinate& coord) const;

        bool IsHighlighted(const Coordinate& coord) const;

    private:
        std::optional<Coordinate> mSelected;
        std::optional<Coordinate> mHovered;
    };
} // namespace RealmFortress
