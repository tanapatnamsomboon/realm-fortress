/**
 * @file entry_point.cpp
 * @brief
 * @date 11/24/2025
 */

#include "core/pch.h"
#include "core/application.h"
#include "core/logger.h"

int main(int argc, char** argv)
{
    RF::Logger::Init();

    RF_CORE_INFO("==============");
    RF_CORE_INFO("Realm Fortress");
    RF_CORE_INFO("==============");

    auto app = RF::CreateApplication({ argc, argv });

    app->Run();

    delete app;

    return 0;
}
