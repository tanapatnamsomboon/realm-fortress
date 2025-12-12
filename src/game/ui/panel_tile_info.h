/**
 * @file panel_tile_info.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "game/ui/panel_base.h"
#include "game/system/tile.h"
#include "game/building/building.h"
#include "game/structure/structure.h"

namespace RealmFortress
{
    class PanelTileInfo final : public PanelBase
    {
    public:
        PanelTileInfo();

        void OnRender() override;

        void SetSelectedTile(const Tile* tile) { mSelectedTile = tile; }
        void SetSelectedBuilding(const Building* building) { mSelectedBuilding = building; }
        void SetSelectedStructure(const Structure* structure) { mSelectedStructure = structure; }
        void Clear();

    private:
        const Tile* mSelectedTile{ nullptr };
        const Building* mSelectedBuilding{ nullptr };
        const Structure* mSelectedStructure{ nullptr };
    };
} // namespace RealmFortress
