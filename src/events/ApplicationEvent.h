/**
 * @file ApplicationEvent.h
 * @brief
 * @date 11/11/2025
 */

#pragma once

#include "events/Event.h"

namespace RealmFortress
{
    class WindowResizeEvent final : public Event
    {
    public:
        WindowResizeEvent(unsigned w, unsigned h)
            : m_Width(w), m_Height(h)
        {
        }

        unsigned GetWidth() const { return m_Width; }
        unsigned GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApp)

    private:
        int m_Width, m_Height;
    };

    class WindowCloseEvent final : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApp)
    };
} // namespace RealmFortress
