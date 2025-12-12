/**
 * @file panel_base.h
 * @brief
 * @date 12/13/2025
 */

#pragma once

#include "core/base.h"
#include <string>

namespace RealmFortress
{
    class PanelBase
    {
    public:
        explicit PanelBase(const std::string& name)
            : mName(name), mVisible(true)
        {
        }

        virtual ~PanelBase() = default;

        virtual void OnRender() = 0;

        void SetVisible(bool visible) { mVisible = visible; }
        bool IsVisible() const { return mVisible; }
        void ToggleVisible() { mVisible = !mVisible; }

        const std::string& GetName() const { return mName; }

    protected:
        std::string mName;
        bool mVisible;
    };
} // namespace RealmFortress
