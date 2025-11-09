/**
 * @file Input.cpp
 * @brief
 * @date 11/9/2025
 */

#include "Input.h"
#include <GLFW/glfw3.h>

namespace RealmFortress
{
    void Input::Init(GLFWwindow *window)
    {
        s_Window = window;
        glfwGetCursorPos(s_Window, &s_MouseX, &s_MouseY);
        s_LastMouseX = s_MouseX;
        s_LastMouseY = s_MouseY;
    }

    void Input::Update()
    {
        s_LastMouseX = s_MouseX;
        s_LastMouseY = s_MouseY;
        glfwGetCursorPos(s_Window, &s_MouseX, &s_MouseY);
    }

    bool Input::IsKeyDown(int key)
    {
        return glfwGetKey(s_Window, key) == GLFW_PRESS;
    }

    bool Input::IsKeyPress(int key)
    {
        static bool wasDown[512] = { false };
        bool now = glfwGetKey(s_Window, key) == GLFW_PRESS;
        bool pressed = now && !wasDown[key];
        wasDown[key] = now;
        return pressed;
    }

    bool Input::IsMouseButtonDown(int button)
    {
        return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
    }

    bool Input::IsMouseButtonPressed(int button)
    {
        static bool wasDown[8] = { false };
        bool now = glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
        bool pressed = now && !wasDown[button];
        wasDown[button] = now;
        return pressed;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        return { static_cast<float>(s_MouseX), static_cast<float>(s_MouseY) };
    }

    std::pair<float, float> Input::GetMouseDelta()
    {
        return {
            static_cast<float>(s_MouseX - s_LastMouseX),
            static_cast<float>(s_MouseY - s_LastMouseY)
        };
    }
} // namespace RealmFortress
