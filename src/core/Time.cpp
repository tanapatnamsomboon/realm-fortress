/**
 * @file Time.cpp
 * @brief
 * @date 11/9/2025
 */

#include "Time.h"
#include <GLFW/glfw3.h>

namespace RealmFortress
{
    void Time::BeginFrame()
    {
        s_CurrentTime = glfwGetTime();
        s_Delta = static_cast<float>(s_CurrentTime - s_LastTime);
        s_LastTime = s_CurrentTime;
    }
} // RealmFortress
