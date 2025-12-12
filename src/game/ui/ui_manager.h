/**
 * @file ui_manager.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "core/base.h"
#include "game/ui/panel_resources.h"
#include "game/ui/panel_build.h"
#include "game/ui/panel_tile_info.h"
#include "game/ui/panel_debug.h"

namespace RealmFortress
{
    class UIManager
    {
    public:
        UIManager();

        void OnRender();

        PanelResources& GetResourcesPanel() { return mPanelResources; }
        PanelBuild& GetBuildPanel() { return mPanelBuild; }
        PanelTileInfo& GetTileInfoPanel() { return mPanelTileInfo; }
        PanelDebug& GetDebugPanel() { return mPanelDebug; }

        void ToggleResourcesPanel() { mPanelResources.ToggleVisible(); }
        void ToggleBuildPanel() { mPanelBuild.ToggleVisible(); }
        void ToggleTileInfoPanel() { mPanelTileInfo.ToggleVisible(); }
        void ToggleDebugPanel() { mPanelDebug.ToggleVisible(); }
    private:
        PanelResources mPanelResources;
        PanelBuild mPanelBuild;
        PanelTileInfo mPanelTileInfo;
        PanelDebug mPanelDebug;
    };
} // namespace RealmFortress
