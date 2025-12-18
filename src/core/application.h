/**
 * @file application.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include "core/base.h"
#include "core/window.h"
#include "core/layer_stack.h"
#include "core/timestep.h"
#include "core/logger.h"
#include "events/event.h"
#include "events/application_event.h"
#include "imgui/imgui_layer.h"

namespace RealmFortress
{
    struct ApplicationCommandLineArgs
    {
        int mCount = 0;
        char** mArgs = nullptr;

        const char* operator[](int index) const
        {
            RF_CORE_ASSERT(index < mCount);
            return mArgs[index];
        }
    };

    struct ApplicationSpecification
    {
        std::string mName = "Realm Fortress";
        std::string mWorkingDirectory;
        ApplicationCommandLineArgs mCommandLineArgs;
    };

    /**
     * @class Application
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Application
    {
    public:
        Application(const ApplicationSpecification& specification);
        virtual ~Application();

        void Run();
        void OnEvent(Event& event);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        Window& GetWindow() { return *mWindow; }

        void Close();

        static Application& Get() { return *sInstance; }

    private:
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

    private:
        Scope<Window> mWindow;
        bool mRunning{ true };
        bool mMinimized{ false };
        LayerStack mLayerStack;
        f32 mLastFrameTime{ 0.0f };

        ImGuiLayer* mImGuiLayer;

    private:
        static Application* sInstance;
    };

    Application* CreateApplication(const ApplicationCommandLineArgs& args);
} // namespace RealmFortress
