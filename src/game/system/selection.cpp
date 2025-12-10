/**
 * @file selection.cpp
 * @brief
 * @date 12/11/2025
 */

#include "selection.h"

namespace RealmFortress
{
    void Selection::Select(const Coordinate& coord)
    {
        mSelected = coord;
    }

    void Selection::ClearSelection()
    {
        mSelected = std::nullopt;
    }

    bool Selection::IsSelected(const Coordinate& coord) const
    {
        return mSelected.has_value() && mSelected.value() == coord;
    }

    void Selection::SetHovered(const Coordinate& coord)
    {
        mHovered = coord;
    }

    void Selection::ClearHover()
    {
        mHovered = std::nullopt;
    }

    bool Selection::IsHovered(const Coordinate& coord) const
    {
        return mHovered.has_value() && mHovered.value() == coord;
    }

    bool Selection::IsHighlighted(const Coordinate& coord) const
    {
        return IsSelected(coord) || IsHovered(coord);
    }
} // namespace RealmFortress
