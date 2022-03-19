#include "Utility/Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace SD {

std::shared_ptr<spdlog::logger> Log::s_core_logger;
std::shared_ptr<spdlog::logger> Log::s_client_logger;

void Log::Init(const std::string &logFileName)
{
    std::vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName, true));

    logSinks[0]->set_pattern("%^[%T] %n: %v%$");
    logSinks[1]->set_pattern("[%T] [%l] %n: %v");

    s_core_logger = std::make_shared<spdlog::logger>("CORE", begin(logSinks),
                                                     end(logSinks));
    spdlog::register_logger(s_core_logger);
    s_core_logger->set_level(spdlog::level::trace);
    s_core_logger->flush_on(spdlog::level::trace);

    s_client_logger =
        std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_client_logger);
    s_client_logger->set_level(spdlog::level::trace);
    s_client_logger->flush_on(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> &Log::GetCoreLogger() { return s_core_logger; }

std::shared_ptr<spdlog::logger> &Log::GetClientLogger()
{
    return s_client_logger;
}

void Log::EmplaceSink(const std::shared_ptr<BasicSink> &sink)
{
    s_client_logger->sinks().emplace_back(sink);
    s_core_logger->sinks().emplace_back(sink);
}

}  // namespace SD
