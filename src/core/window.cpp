/**
 * @file window.cpp
 * @brief
 * @date 11/8/2025
 */

#include "window.h"
#include "core/logger.h"
#include "renderer/graphics_context.h"
#include "events/application_event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include <GLFW/glfw3.h>

namespace RealmFortress
{
    static u8 sGLFWWindowCount = 0;

    static bool sGLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        RF_CORE_ERROR("GLFW ERROR ({}): {}", error, description);
    }

    Window::Window(const WindowProps& props)
    {
        Init(props);
    }

    Window::~Window()
    {
        Shutdown();
    }

    void Window::Init(const WindowProps& props)
    {
        mData.mTitle = props.mTitle;
        mData.mWidth = props.mWidth;
        mData.mHeight = props.mHeight;

        RF_CORE_INFO("Creating window {} ({}, {})", props.mTitle, props.mWidth, props.mHeight);

        if (!sGLFWInitialized)
        {
            int success = glfwInit();
            RF_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            sGLFWInitialized = true;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SAMPLES, 4);

#       if defined(RF_PLATFORM_MAC)
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#       endif

        mWindow = glfwCreateWindow(props.mWidth, props.mHeight, mData.mTitle.c_str(), nullptr, nullptr);
        ++sGLFWWindowCount;

        mContext = CreateScope<GraphicsContext>(mWindow);
        mContext->Init();

        glfwSetWindowUserPointer(mWindow, &mData);
        SetVSync(true);

        glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.mWidth = width;
            data.mHeight = height;

            WindowResizeEvent event(width, height);
            data.mEventCallback(event);
        });

        glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            WindowCloseEvent event;
            data.mEventCallback(event);
        });

        glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), false);
                    data.mEventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(static_cast<KeyCode>(key));
                    data.mEventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), true);
                    data.mEventCallback(event);
                    break;
                }
                default: break;
            }
        });

        glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            KeyTypedEvent event(static_cast<KeyCode>(keycode));
            data.mEventCallback(event);
        });

        glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(static_cast<MouseCode>(button));
                    data.mEventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
                    data.mEventCallback(event);
                    break;
                }
                default: break;
            }
        });

        glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            data.mEventCallback(event);
        });

        glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
            data.mEventCallback(event);
        });
    }

    void Window::Shutdown()
    {
        glfwDestroyWindow(mWindow);
        --sGLFWWindowCount;

        if (sGLFWWindowCount == 0)
        {
            glfwTerminate();
        }
    }

    void Window::OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(mWindow);
    }

    void Window::SetVSync(bool enabled)
    {
        mData.mVSync = enabled;
        glfwSwapInterval(mData.mVSync ? 1 : 0);
    }
} // namespace RealmFortress
