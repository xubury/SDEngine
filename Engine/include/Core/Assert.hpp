#ifndef SD_ASSERT_HPP
#define SD_ASSERT_HPP

#include <filesystem>

#include "Core/Log.hpp"
#include "Core/PlatformDetection.hpp"

#ifdef SD_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and
// "NO_MSG" and provide support for custom formatting by concatenating the
// formatting string instead of having the format inside the default message
#define SD_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
    {                                                  \
        if (!(check)) {                                \
            SD##type##ERROR(msg, __VA_ARGS__);         \
            SD_DEBUGBREAK();                           \
        }                                              \
    }
#define SD_INTERNAL_ASSERT_WITH_MSG(type, check, ...) \
    SD_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define SD_INTERNAL_ASSERT_NO_MSG(type, check)            \
    SD_INTERNAL_ASSERT_IMPL(                              \
        type, check, "Assertion '{0}' failed at {1}:{2}", \
        SD_STRINGIFY_MACRO(check),                        \
        std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define SD_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define SD_INTERNAL_ASSERT_GET_MACRO(...)              \
    SD_EXPAND_MACRO(SD_INTERNAL_ASSERT_GET_MACRO_NAME( \
        __VA_ARGS__, SD_INTERNAL_ASSERT_WITH_MSG, SD_INTERNAL_ASSERT_NO_MSG))

// Currently accepts at least the condition and one additional parameter (the
// message) being optional
#define SD_ASSERT(...) \
    SD_EXPAND_MACRO(SD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define SD_CORE_ASSERT(...) \
    SD_EXPAND_MACRO(        \
        SD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define SD_ASSERT(...)
#define SD_CORE_ASSERT(...)

#endif

#endif
