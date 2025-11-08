/**
 * @file Window.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include <string>

struct GLFWwindow;

namespace RealmFortress
{
    /**
     * @class Window
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Window
    {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        void PollEvents();
        void SwapBuffers();

        bool ShouldClose() const;
        void SetVSync(bool enabled);
        GLFWwindow* GetNativeHandle() const { return m_Handle; }

    private:
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    private:
        GLFWwindow* m_Handle{ nullptr };
        int m_Width{ 1280 };
        int m_Height{ 720 };
        bool m_VSync{ true };
    };
} // namespace RealmFortress
