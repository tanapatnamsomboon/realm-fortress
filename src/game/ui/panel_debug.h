/**
 * @file panel_debug.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "game/ui/panel_base.h"
#include "scene/camera.h"

namespace RealmFortress
{
    class PanelDebug final : public PanelBase
    {
    public:
        PanelDebug();

        void OnRender() override;

        void SetCamera(const Camera* camera) { mCamera = camera; }
        void SetTileCount(usize count) { mTileCount = count; }
        void SetBuildingCount(usize count) { mBuildingCount = count; }
        void SetDecorationCount(usize count) { mDecorationCount = count; }
        void SetStructureCount(usize count) { mStructureCount = count; }

    private:
        const Camera* mCamera{ nullptr };
        usize mTileCount{ 0 };
        usize mBuildingCount{ 0 };
        usize mDecorationCount{ 0 };
        usize mStructureCount{ 0 };
    };
} // namespace RealmFortress
