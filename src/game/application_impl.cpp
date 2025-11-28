/**
 * @file application_impl.cpp
 * @brief
 * @date ${DATA}
 */

#include "core/pch.h"
#include "core/application.h"
#include "game/game_layer.h"

namespace RF
{
    Application* CreateApplication(const ApplicationCommandLineArgs& args)
    {
        ApplicationSpecification spec;
        spec.mName = "Realm Fortress";
        spec.mCommandLineArgs = args;
        Application* app = new Application(spec);
        app->PushLayer(new GameLayer());
        return app;
    }
} // namespace RF
