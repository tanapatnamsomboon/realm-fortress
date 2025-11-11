/**
 * @file Window.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include <string>
#include <functional>

struct GLFWwindow;

namespace RealmFortress
{
    class Event;

    struct WindowProps
    {
        std::string Title;
        uint32_t    Width;
        uint32_t    Height;

        explicit WindowProps(const std::string& title  = "Realm Fortress",
                             unsigned int       width  = 1600,
                             unsigned int       height = 900)
            : Title(title), Width(width), Height(height)
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
    using EventCallbackFn = std::function<void(Event&)>;

    public:
        explicit Window(const WindowProps& props);
        ~Window();

        void PollEvents();
        void SwapBuffers();

        bool ShouldClose() const;
        void SetVSync(bool enabled);
        GLFWwindow* GetNativeHandle() const { return m_Window; }

        int GetWidth() const { return m_Data.Width; }
        int GetHeight() const { return m_Data.Height; }

        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

    private:
        void Init(const WindowProps& props);
        void Shutdown();

    private:
        GLFWwindow* m_Window{ nullptr };

        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
} // namespace RealmFortress
