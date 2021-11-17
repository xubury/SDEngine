#ifndef SD_LOG_HPP
#define SD_LOG_HPP

#include "Utility/Export.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace SD {

class SD_API Log {
   public:
    static void Init(const std::string &logFileName);

    static std::shared_ptr<spdlog::logger> &GetCoreLogger();

    static std::shared_ptr<spdlog::logger> &GetClientLogger();

   private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;
};

}  // namespace SD

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
