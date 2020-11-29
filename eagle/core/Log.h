#ifndef EAGLE_LOG_H
#define EAGLE_LOG_H

#include "CoreGlobalDefinitions.h"
#include <spdlog/spdlog.h>
#include <memory>

EG_BEGIN

class Log {

public:

    enum LOG_LEVEL{
        TRACE       = SPDLOG_LEVEL_TRACE,
        DEBUG       = SPDLOG_LEVEL_DEBUG,
        INFO        = SPDLOG_LEVEL_INFO,
        WARN        = SPDLOG_LEVEL_WARN,
        ERR         = SPDLOG_LEVEL_ERROR,
        CRITICAL    = SPDLOG_LEVEL_CRITICAL,
        OFF         = SPDLOG_LEVEL_OFF
    };


    static void init(Log::LOG_LEVEL coreLogLevel, Log::LOG_LEVEL clientLogLevel);

    template<typename... Args>
    static void core_log(const char* file, const char* function, int line, LOG_LEVEL level, const char* message, const Args &... args){
        if (!s_coreLogger)
            return;
        std::string format(std::string(file) + "::" + function + "()[" + std::to_string(line) + "]: " + message);
        s_coreLogger->log((spdlog::level::level_enum)level, format.c_str(), args...);
    }

    template<typename... Args>
    static void client_log(const char* file, const char* function, int line, LOG_LEVEL level, const char* message, const Args &... args){
        if (!s_clientLogger)
            return;
        std::string format(std::string(file) + "::" + function + "()[" + std::to_string(line) + "]: " + message);
        s_clientLogger->log((spdlog::level::level_enum)level, format.c_str(), args...);
    }

private:
    static Reference<spdlog::logger> s_coreLogger;
    static Reference<spdlog::logger> s_clientLogger;

};

EG_END

#define EG_CORE_TRACE_F(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::TRACE, message, __VA_ARGS__)
#define EG_CORE_INFO_F(message, ...)     Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::INFO, message, __VA_ARGS__)
#define EG_CORE_DEBUG_F(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::DEBUG, message, __VA_ARGS__)
#define EG_CORE_WARNING_F(message, ...)  Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::WARN, message, __VA_ARGS__)
#define EG_CORE_ERROR_F(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::ERR, message, __VA_ARGS__)
#define EG_CORE_FATAL_F(message, ...)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::CRITICAL, message, __VA_ARGS__)

#define EG_CORE_TRACE(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__,  Eagle::Log::TRACE, message)
#define EG_CORE_INFO(message)     Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__,  Eagle::Log::INFO, message)
#define EG_CORE_DEBUG(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__,  Eagle::Log::DEBUG, message)
#define EG_CORE_WARNING(message)  Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__,  Eagle::Log::WARN, message)
#define EG_CORE_ERROR(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__,  Eagle::Log::ERR, message)
#define EG_CORE_FATAL(message)    Eagle::Log::core_log(FILE_BASENAME, __FUNCTION__, __LINE__,  Eagle::Log::CRITICAL, message)

#define EG_TRACE_F(message, ...)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::LOG_LEVEL::TRACE, message, __VA_ARGS__)
#define EG_INFO_F(message, ...)     Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::LOG_LEVEL::INFO, message, __VA_ARGS__)
#define EG_DEBUG_F(message, ...)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::LOG_LEVEL::DEBUG, message, __VA_ARGS__)
#define EG_WARNING_F(message, ...)  Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::LOG_LEVEL::WARN, message, __VA_ARGS__)
#define EG_ERROR_F(message, ...)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::LOG_LEVEL::ERR, message, __VA_ARGS__)
#define EG_FATAL_F(message, ...)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::LOG_LEVEL::CRITICAL, message, __VA_ARGS__)

#define EG_TRACE(message)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::TRACE, message)
#define EG_INFO(message)     Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::INFO, message)
#define EG_DEBUG(message)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::DEBUG, message)
#define EG_WARNING(message)  Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::WARN, message)
#define EG_ERROR(message)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::ERR, message)
#define EG_FATAL(message)    Eagle::Log::client_log(FILE_BASENAME, __FUNCTION__, __LINE__, Eagle::Log::CRITICAL, message)


#endif //EAGLE_LOG_H
