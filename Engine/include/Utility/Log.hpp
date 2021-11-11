#ifndef SD_LOG_HPP
#define SD_LOG_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace SD {

class SD_API Log {
   public:
    static void init(const std::string &logFileName);

    static Ref<spdlog::logger> &getCoreLogger();

    static Ref<spdlog::logger> &getClientLogger();

   private:
    static Ref<spdlog::logger> s_coreLogger;
    static Ref<spdlog::logger> s_clientLogger;
};

}  // namespace SD

#define SD_CORE_TRACE(...) ::SD::Log::getCoreLogger()->trace(__VA_ARGS__)
#define SD_CORE_INFO(...) ::SD::Log::getCoreLogger()->info(__VA_ARGS__)
#define SD_CORE_WARN(...) ::SD::Log::getCoreLogger()->warn(__VA_ARGS__)
#define SD_CORE_ERROR(...) ::SD::Log::getCoreLogger()->error(__VA_ARGS__)
#define SD_CORE_CRITICAL(...) ::SD::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SD_TRACE(...) ::SD::Log::getClientLogger()->trace(__VA_ARGS__)
#define SD_INFO(...) ::SD::Log::getClientLogger()->info(__VA_ARGS__)
#define SD_WARN(...) ::SD::Log::getClientLogger()->warn(__VA_ARGS__)
#define SD_ERROR(...) ::SD::Log::getClientLogger()->error(__VA_ARGS__)
#define SD_CRITICAL(...) ::SD::Log::getClientLogger()->critical(__VA_ARGS__)

#endif
