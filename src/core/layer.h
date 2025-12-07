/**
 * @file layer.h
 * @brief
 * @date 11/25/2025
 */

#pragma once

#include "core/base.h"
#include "core/timestep.h"
#include "events/event.h"
#include <vector>

namespace RealmFortress
{
    class Layer
    {
    public:
        explicit Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnImGuiRender() {}
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return mDebugName; }

    protected:
        std::string mDebugName;
    };
} // namespace RealmFortress
