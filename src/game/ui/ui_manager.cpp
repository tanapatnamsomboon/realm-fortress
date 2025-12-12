/**
 * @file ui_manager.cpp
 * @brief
 * @date 12/13/2025
 */

#include "core/pch.h"
#include "ui_manager.h"

namespace RealmFortress
{
    UIManager::UIManager()
    {
    }

    void UIManager::OnRender()
    {
        mPanelResources.OnRender();
        mPanelBuild.OnRender();
        mPanelTileInfo.OnRender();
        mPanelDebug.OnRender();
    }
} // namespace RealmFortress
