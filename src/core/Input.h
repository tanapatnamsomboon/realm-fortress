/**
 * @file Input.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include <utility>

struct GLFWwindow;

namespace RealmFortress
{
    /**
     * @class Input
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Input
    {
    public:
        static void Init(GLFWwindow* window);
        static void Update();

        static bool IsKeyDown(int key);
        static bool IsKeyPress(int key);
        static bool IsMouseButtonDown(int button);
        static bool IsMouseButtonPressed(int button);

        static std::pair<float, float> GetMousePosition();
        static std::pair<float, float> GetMouseDelta();

    private:
        static inline GLFWwindow* s_Window{ nullptr };
        static inline double s_MouseX{ 0.0 }, s_MouseY{ 0.0 };
        static inline double s_LastMouseX{ 0.0 }, s_LastMouseY{ 0.0 };
    };
} // namespace RealmFortress
