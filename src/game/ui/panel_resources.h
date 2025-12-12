/**
 * @file panel_resources.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "game/ui/panel_base.h"

namespace RealmFortress
{
    class PanelResources final : public PanelBase
    {
    public:
        PanelResources();

        void OnRender() override;

        void SetWood(i32 wood) { mWood = wood; }
        void SetStone(i32 stone) { mStone = stone; }
        void SetGold(i32 gold) { mGold = gold; }
        void SetFood(i32 food) { mFood = food; }

    private:
        i32 mWood{ 100 };
        i32 mStone{ 100 };
        i32 mGold{ 100 };
        i32 mFood{ 100 };
    };
} // namespace RealmFortress
