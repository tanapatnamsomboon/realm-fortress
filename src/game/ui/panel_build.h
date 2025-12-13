/**
 * @file panel_build.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "game/ui/panel_base.h"
#include "game/building/building.h"
#include "game/structure/structure_types.h"
#include "game/decoration/decoration_type.h"

namespace RealmFortress
{
    class PanelBuild final : public PanelBase
    {
    public:
        PanelBuild();

        void OnRender() override;

        void SetOnBuildingSelected(const std::function<void(BuildingType)>& callback) { mOnBuildingSelected = callback; }
        void SetOnDecorationSelected(const std::function<void(DecorationType)>& callback) { mOnDecorationSelected = callback; }
        void SetOnStructureSelected(const std::function<void(StructureType)>& callback) { mOnStructureSelected = callback; }

    private:
        void RenderBuildingButtons();
        void RenderDecorationButtons();
        void RenderStructureButtons();

    private:
        std::function<void(BuildingType)> mOnBuildingSelected;
        std::function<void(DecorationType)> mOnDecorationSelected;
        std::function<void(StructureType)> mOnStructureSelected;
    };
} // namespace RealmFortress
