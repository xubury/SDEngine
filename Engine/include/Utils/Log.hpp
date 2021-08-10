#ifndef SD_LOG_HPP
#define SD_LOG_HPP

#include "Common/Base.hpp"
#include "Common/Export.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace sd {

class SD_API Log {
   public:
    static void init(const std::string &logFileName);

    static Ref<spdlog::logger> &getCoreLogger();

    static Ref<spdlog::logger> &getClientLogger();

   private:
    static Ref<spdlog::logger> s_coreLogger;
    static Ref<spdlog::logger> s_clientLogger;
};

}  // namespace sd

#define SD_CORE_TRACE(...) ::sd::Log::getCoreLogger()->trace(__VA_ARGS__)
#define SD_CORE_INFO(...) ::sd::Log::getCoreLogger()->info(__VA_ARGS__)
#define SD_CORE_WARN(...) ::sd::Log::getCoreLogger()->warn(__VA_ARGS__)
#define SD_CORE_ERROR(...) ::sd::Log::getCoreLogger()->error(__VA_ARGS__)
#define SD_CORE_CRITICAL(...) ::sd::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SD_TRACE(...) ::sd::Log::getClientLogger()->trace(__VA_ARGS__)
#define SD_INFO(...) ::sd::Log::getClientLogger()->info(__VA_ARGS__)
#define SD_WARN(...) ::sd::Log::getClientLogger()->warn(__VA_ARGS__)
#define SD_ERROR(...) ::sd::Log::getClientLogger()->error(__VA_ARGS__)
#define SD_CRITICAL(...) ::sd::Log::getClientLogger()->critical(__VA_ARGS__)

#endif
