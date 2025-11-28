/**
 * @file logger.h
 * @brief
 * @date 11/19/2025
 */

#pragma once

#include "core/base.h"
#include <spdlog/spdlog.h>

namespace RF
{
    class Logger
    {
    public:
        static void Init();

        static Ref<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
        static Ref<spdlog::logger>& GetClientLogger() { return sClientLogger; }

    private:
        static Ref<spdlog::logger> sCoreLogger;
        static Ref<spdlog::logger> sClientLogger;
    };
} // namespace RF

#define RF_CORE_TRACE(...)    ::RF::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define RF_CORE_INFO(...)     ::RF::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define RF_CORE_WARN(...)     ::RF::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define RF_CORE_ERROR(...)    ::RF::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define RF_CORE_CRITICAL(...) ::RF::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#define RF_TRACE(...)         ::RF::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define RF_INFO(...)          ::RF::Logger::GetClientLogger()->info(__VA_ARGS__)
#define RF_WARN(...)          ::RF::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define RF_ERROR(...)         ::RF::Logger::GetClientLogger()->error(__VA_ARGS__)
#define RF_CRITICAL(...)      ::RF::Logger::GetClientLogger()->critical(__VA_ARGS__)
