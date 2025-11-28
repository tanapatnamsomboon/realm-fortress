/**
 * @file key_event.h
 * @brief
 * @date 11/11/2025
 */

#pragma once

#include "events/event.h"
#include "core/key_codes.h"

namespace RF
{
    class KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return mKeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        explicit KeyEvent(KeyCode keycode)
            : mKeyCode(keycode)
        {
        }

        KeyCode mKeyCode;
    };

    class KeyPressedEvent final : public KeyEvent
    {
    public:
        explicit KeyPressedEvent(KeyCode keycode, bool isRepeat = false)
            : KeyEvent(keycode), mIsRepeat(isRepeat)
        {
        }

        bool IsRepeat() const { return mIsRepeat; }

        std::string ToString() const override
        {
            return std::format("KeyPressedEvent: {} (repeat: {})", mKeyCode, mIsRepeat);
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        int mIsRepeat;
    };

    class KeyReleasedEvent final : public KeyEvent
    {
    public:
        explicit KeyReleasedEvent(KeyCode keycode)
            : KeyEvent(keycode)
        {
        }

        std::string ToString() const override
        {
            return std::format("KeyReleasedEvent: {}", mKeyCode);
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent final : public KeyEvent
    {
    public:
        explicit KeyTypedEvent(KeyCode keycode)
            : KeyEvent(keycode)
        {
        }

        std::string ToString() const override
        {
            return std::format("KeyTypedEvent: {}", mKeyCode);
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
} // namespace RF
