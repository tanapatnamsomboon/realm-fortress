/**
 * @file window.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include "core/base.h"
#include "events/event.h"
#include <string>
#include <functional>

struct GLFWwindow;

namespace RealmFortress
{
    class GraphicsContext;

    struct WindowProps
    {
        std::string mTitle;
        u32         mWidth;
        u32         mHeight;

        explicit WindowProps(const std::string& title = "Realm Fortress",
                             u32 width = 1600,
                             u32 height = 900)
            : mTitle(title), mWidth(width), mHeight(height)
        {
        }
    };

    /**
     * @class Window
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        explicit Window(const WindowProps& props);
        ~Window();

        void OnUpdate();

        u32 GetWidth() const { return mData.mWidth; }
        u32 GetHeight() const { return mData.mHeight; }

        void SetEventCallback(const EventCallbackFn& callback) { mData.mEventCallback = callback; }
        void SetVSync(bool enabled);
        bool IsVSync() const { return mData.mVSync; }

        void* GetNativeWindow() const { return mWindow; }

    private:
        void Init(const WindowProps& props);
        void Shutdown();

    private:
        GLFWwindow* mWindow = nullptr;
        Scope<GraphicsContext> mContext;

        struct WindowData
        {
            std::string mTitle;
            u32 mWidth, mHeight;
            bool mVSync;
            EventCallbackFn mEventCallback;
        };

        WindowData mData;
    };
} // namespace RealmFortress
