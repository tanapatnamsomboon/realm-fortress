/**
 * @file Event.h
 * @brief
 * @date 11/11/2025
 */

#pragma once

#include <string>
#include <sstream>
#include <functional>

namespace RealmFortress
{
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize,
        KeyPressed, KeyReleased,
        MouseButtonPressed, MouseButtonReleased,
        MouseMoved, MouseScrolled
    };

    enum EventCategory
    {
        EventCategoryNone        = 0,
        EventCategoryApp         = 1 << 0,
        EventCategoryInput       = 1 << 1,
        EventCategoryKeyboard    = 1 << 2,
        EventCategoryMouse       = 1 << 3,
        EventCategoryMouseButton = 1 << 4,
    };

#   define EVENT_CLASS_TYPE(type) static  EventType   GetStaticType()               { return EventType::type; } \
                                  virtual EventType   GetEventType() const override { return GetStaticType(); } \
                                  virtual const char* GetName()      const override { return #type; }

#   define EVENT_CLASS_CATEGORY(cat) virtual int GetCategoryFlags() const override { return cat; }

    /**
     * @class Event
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Event
    {
    public:
        virtual ~Event() = default;

        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }
    };

    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        explicit EventDispatcher(Event& e)
            : m_Event(e)
        {
        }

        template<typename T>
        bool Dispatch(EventFn<T> func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled = func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };
} // namespace RealmFortress
