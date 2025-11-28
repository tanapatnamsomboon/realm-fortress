/**
 * @file base.h
 * @brief
 * @date 11/19/2025
 */

#pragma once

#include <memory>
#include <cstdint>

#ifdef RF_DEBUG
#   ifdef RF_PLATFORM_WINDOWS
#       define RF_DEBUGBREAK() __debugbreak()
#   elif defined(RF_PLATFORM_MACOS) || defined(RF_PLATFORM_LINUX)
#       include <signal.h>
#       define RF_DEBUGBREAK() raise(SIGTRAP)
#   else
#       define RF_DEBUGBREAK()
#   endif
// TODO: implement assertion
// #   define RF_ENABLE_ASSERTS
#else
#   define RF_DEBUGBREAK()
#endif

#ifdef RF_ENABLE_ASSERTS
#   define RF_ASSERT(x, ...) { if(!(x)) { RF_ERROR("Assertion Failed: {0}", __VA_ARGS__); RF_DEBUGBREAK(); } }
#   define RF_CORE_ASSERT(x, ...) { if(!(x)) { RF_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); RF_DEBUGBREAK(); } }
#else
#   define RF_ASSERT(x, ...)
#   define RF_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define RF_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace RF
{
    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;

    using usize = size_t;
    using isize = ptrdiff_t;

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
} // namespace RF


