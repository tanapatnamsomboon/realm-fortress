/**
 * @file logger.h
 * @brief
 * @date 11/19/2025
 */

#pragma once

#include "core/base.h"
#include <spdlog/spdlog.h>

namespace RealmFortress
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
} // namespace RealmFortress

#define RF_CORE_TRACE(...)    ::RealmFortress::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define RF_CORE_INFO(...)     ::RealmFortress::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define RF_CORE_WARN(...)     ::RealmFortress::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define RF_CORE_ERROR(...)    ::RealmFortress::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define RF_CORE_CRITICAL(...) ::RealmFortress::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#define RF_TRACE(...)         ::RealmFortress::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define RF_INFO(...)          ::RealmFortress::Logger::GetClientLogger()->info(__VA_ARGS__)
#define RF_WARN(...)          ::RealmFortress::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define RF_ERROR(...)         ::RealmFortress::Logger::GetClientLogger()->error(__VA_ARGS__)
#define RF_CRITICAL(...)      ::RealmFortress::Logger::GetClientLogger()->critical(__VA_ARGS__)
