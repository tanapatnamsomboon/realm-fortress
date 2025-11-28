/**
 * @file mouse_event.h
 * @brief
 * @date 11/11/2025
 */

#pragma once

#include "events/event.h"
#include "core/mouse_codes.h"

namespace RF
{
    class MouseMovedEvent final : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : mMouseX(x), mMouseY(y)
        {
        }

        float GetX() const { return mMouseX; }
        float GetY() const { return mMouseY; }

        std::string ToString() const override
        {
            return std::format("MouseMovedEvent: {}, {}", mMouseX, mMouseY);
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float mMouseX, mMouseY;
    };

    class MouseScrolledEvent final : public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : mXOffset(xOffset), mYOffset(yOffset)
        {
        }

        float GetXOffset() const { return mXOffset; }
        float GetYOffset() const { return mYOffset; }

        std::string ToString() const override
        {
            return std::format("MouseScrolledEvent: {}, {}", mXOffset, mYOffset);
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float mXOffset, mYOffset;
    };

    class MouseButtonEvent : public Event
    {
    public:
        MouseCode GetMouseButton() const { return mButton; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

    protected:
        explicit MouseButtonEvent(MouseCode button)
            : mButton(button)
        {
        }

        MouseCode mButton;
    };

    class MouseButtonPressedEvent final : public MouseButtonEvent
    {
    public:
        explicit MouseButtonPressedEvent(MouseCode button)
            : MouseButtonEvent(button)
        {
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent final : public MouseButtonEvent
    {
    public:
        explicit MouseButtonReleasedEvent(MouseCode button)
            : MouseButtonEvent(button)
        {
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
} // namespace RF
