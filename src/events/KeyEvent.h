/**
 * @file KeyEvent.h
 * @brief
 * @date 11/11/2025
 */

#pragma once

#include "events/Event.h"

namespace RealmFortress
{
    class KeyEvent : public Event
    {
    public:
        int GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        explicit KeyEvent(int key)
            : m_KeyCode(key)
        {
        }

        int m_KeyCode;
    };

    class KeyPressedEvent final : public KeyEvent
    {
    public:
        KeyPressedEvent(int key, int repeatCount)
            : KeyEvent(key), m_RepeatCount(repeatCount)
        {
        }

        int GetRepeatCount() const { return m_RepeatCount; }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        int m_RepeatCount;
    };

    class KeyReleasedEvent final : public KeyEvent
    {
    public:
        explicit KeyReleasedEvent(int key)
            : KeyEvent(key)
        {
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };
} // namespace RealmFortress
