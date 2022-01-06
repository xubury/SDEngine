#ifndef SD_LOG_HPP
#define SD_LOG_HPP

#include "Utility/Export.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace SD {

class SD_UTILITY_API Log {
   public:
    static void Init(const std::string& logFileName);

    static std::shared_ptr<spdlog::logger>& GetCoreLogger();

    static std::shared_ptr<spdlog::logger>& GetClientLogger();

   private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;
};

}  // namespace SD

template <glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const glm::vec<L, T, Q>& input, FormatContext& ctx)
        -> decltype(ctx.out()) {
        return format_to(ctx.out(), glm::to_string(input));
    }
};

template <typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const glm::qua<T, Q>& input, FormatContext& ctx)
        -> decltype(ctx.out()) {
        return format_to(ctx.out(), "quat({}, {}, {}, {})", input.w, input.x,
                         input.y, input.z);
    }
};

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C, R, T, Q>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const glm::mat<C, R, T, Q>& input, FormatContext& ctx)
        -> decltype(ctx.out()) {
        return format_to(ctx.out(), glm::to_string(input));
    }
};

#define SD_CORE_TRACE(...) ::SD::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SD_CORE_INFO(...) ::SD::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SD_CORE_WARN(...) ::SD::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SD_CORE_ERROR(...) ::SD::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SD_CORE_CRITICAL(...) ::SD::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SD_TRACE(...) ::SD::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SD_INFO(...) ::SD::Log::GetClientLogger()->info(__VA_ARGS__)
#define SD_WARN(...) ::SD::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SD_ERROR(...) ::SD::Log::GetClientLogger()->error(__VA_ARGS__)
#define SD_CRITICAL(...) ::SD::Log::GetClientLogger()->critical(__VA_ARGS__)

#endif
