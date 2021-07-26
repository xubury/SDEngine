#ifndef SD_BASE_HPP
#define SD_BASE_HPP

#include <memory>
#include "Core/PlatformDetection.hpp"

namespace sd {

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> createScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> createRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}  // namespace sd

#ifdef DEBUG_BUILD
#if defined(SD_PLATFORM_WINDOWS)
#define SD_DEBUGBREAK() __debugbreak()
#elif defined(SD_PLATFORM_LINUX)
#include <signal.h>
#define SD_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define SD_ENABLE_ASSERTS
#else
#define SD_DEBUGBREAK()
#endif

#define SD_EXPAND_MACRO(x) x
#define SD_STRINGIFY_MACRO(x) #x

#endif
