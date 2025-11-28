/**
 * @file application_event.h
 * @brief
 * @date 11/11/2025
 */

#pragma once

#include "events/event.h"

namespace RF
{
    class WindowResizeEvent final : public Event
    {
    public:
        WindowResizeEvent(unsigned width, unsigned height)
            : mWidth(width), mHeight(height)
        {
        }

        unsigned int GetWidth() const { return mWidth; }
        unsigned int GetHeight() const { return mHeight; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        unsigned int mWidth, mHeight;
    };

    class WindowCloseEvent final : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppTickEvent final : public Event
    {
    public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE(AppTick);
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent final : public Event
    {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent final : public Event
    {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
} // namespace RF
