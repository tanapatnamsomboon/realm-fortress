/**
 * @file application.cpp
 * @brief
 * @date 11/8/2025
 */

#include "core/pch.h"
#include "core/application.h"
#include "core/logger.h"
#include "core/input.h"
#include "renderer/renderer.h"
#include <GLFW/glfw3.h>

namespace RF
{
    Application* Application::sInstance = nullptr;

    Application::Application(const ApplicationSpecification& specification)
    {
        RF_CORE_ASSERT(!sInstance, "application already exists!");
        sInstance = this;

        mWindow = CreateScope<Window>(WindowProps(specification.mName));
        mWindow->SetEventCallback(RF_BIND_EVENT_FN(Application::OnEvent));

        Renderer::Init();
    }

    Application::~Application()
    {
        Renderer::Shutdown();
    }

    void Application::Run()
    {
        while (mRunning)
        {
            float time = static_cast<float>(glfwGetTime());
            Timestep timestep = time - mLastFrameTime;
            mLastFrameTime = time;

            if (!mMinimized)
            {
                for (Layer* layer : mLayerStack)
                    layer->OnUpdate(timestep);
            }

            mWindow->OnUpdate();
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(RF_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(RF_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
        {
            if (event.mHandled)
                break;
            (*it)->OnEvent(event);
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        mLayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* layer)
    {
        mLayerStack.PushOverlay(layer);
    }

    void Application::Close()
    {
        mRunning = false;
    }

    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        mRunning = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& event)
    {
        if (event.GetWidth() == 0 || event.GetHeight() == 0)
        {
            mMinimized = true;
            return false;
        }

        mMinimized = false;
        Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

        return false;
    }
} // namespace RF
