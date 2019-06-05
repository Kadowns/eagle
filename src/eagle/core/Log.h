#ifndef EAGLE_LOG_H
#define EAGLE_LOG_H

#include "Core.h"
#include <spdlog/spdlog.h>
#include <memory>

_EAGLE_BEGIN

class Log {

public:

    static void init();

    template<typename... Args>
    static void core_log(const char* file, const char* function, int line, spdlog::level::level_enum level, const char* message, const Args &... args){
        std::string format(std::string(file) + "::" + function + "()[" + std::to_string(line) + "]: " + message);
        s_coreLogger->log(level, format.c_str(), args...);
    }

    template<typename... Args>
    static void client_log(const char* file, const char* function, int line, spdlog::level::level_enum level, const char* message, const Args &... args){
        std::string format(std::string(file) + "::" + function + "()[" + std::to_string(line) + "]: " + message);
        s_clientLogger->log(level, format.c_str(), args...);
    }

private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;

};

_EAGLE_END

#define EG_TRACE_F(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::trace, message, __VA_ARGS__)
#define EG_INFO_F(message, ...)     Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::info, message, __VA_ARGS__)
#define EG_DEBUG_F(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::debug, message, __VA_ARGS__)
#define EG_WARNING_F(message, ...)  Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::warn, message, __VA_ARGS__)
#define EG_ERROR_F(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::err, message, __VA_ARGS__)
#define EG_FATAL_f(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::critical, message, __VA_ARGS__)

#define EG_TRACE(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::trace, message)
#define EG_INFO(message)     Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::info, message)
#define EG_DEBUG(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::debug, message)
#define EG_WARNING(message)  Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::warn, message)
#define EG_ERROR(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::err, message)
#define EG_FATAL(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, spdlog::level::critical, message)


#define DEBUG_TRACE(...)        Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, __VA_ARGS__, spdlog::level::trace)
#define DEBUG_INFO(...)         Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, __VA_ARGS__, spdlog::level::info)
#define DEBUG_WARNING(...)      Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, __VA_ARGS__, spdlog::level::debug)
#define DEBUG_ERROR(...)        Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, __VA_ARGS__, spdlog::level::warn)
#define DEBUG_FATAL(...)        Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, __VA_ARGS__, spdlog::level::err)

#endif //EAGLE_LOG_H
