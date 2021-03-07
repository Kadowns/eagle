#ifndef EAGLE_LOG_H
#define EAGLE_LOG_H

#include "CoreGlobalDefinitions.h"
#include <spdlog/spdlog.h>
#include <memory>

namespace eagle {

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
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;

};

}

#define EG_STRINGNIZE(s) #s

#define EG_FILENAME EG_STRINGNIZE(FILE_BASENAME)

#define EG_CORE_TRACE_F(message, ...)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::TRACE, message, __VA_ARGS__)
#define EG_CORE_INFO_F(message, ...)     eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::INFO, message, __VA_ARGS__)
#define EG_CORE_DEBUG_F(message, ...)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::DEBUG, message, __VA_ARGS__)
#define EG_CORE_WARNING_F(message, ...)  eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::WARN, message, __VA_ARGS__)
#define EG_CORE_ERROR_F(message, ...)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::ERR, message, __VA_ARGS__)
#define EG_CORE_FATAL_F(message, ...)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::CRITICAL, message, __VA_ARGS__)

#define EG_CORE_TRACE(message)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__,  eagle::Log::TRACE, message)
#define EG_CORE_INFO(message)     eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__,  eagle::Log::INFO, message)
#define EG_CORE_DEBUG(message)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__,  eagle::Log::DEBUG, message)
#define EG_CORE_WARNING(message)  eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__,  eagle::Log::WARN, message)
#define EG_CORE_ERROR(message)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__,  eagle::Log::ERR, message)
#define EG_CORE_FATAL(message)    eagle::Log::core_log(EG_FILENAME, __FUNCTION__, __LINE__,  eagle::Log::CRITICAL, message)

#define EG_TRACE_F(message, ...)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::LOG_LEVEL::TRACE, message, __VA_ARGS__)
#define EG_INFO_F(message, ...)     eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::LOG_LEVEL::INFO, message, __VA_ARGS__)
#define EG_DEBUG_F(message, ...)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::LOG_LEVEL::DEBUG, message, __VA_ARGS__)
#define EG_WARNING_F(message, ...)  eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::LOG_LEVEL::WARN, message, __VA_ARGS__)
#define EG_ERROR_F(message, ...)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::LOG_LEVEL::ERR, message, __VA_ARGS__)
#define EG_FATAL_F(message, ...)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::LOG_LEVEL::CRITICAL, message, __VA_ARGS__)

#define EG_TRACE(message)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::TRACE, message)
#define EG_INFO(message)     eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::INFO, message)
#define EG_DEBUG(message)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::DEBUG, message)
#define EG_WARNING(message)  eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::WARN, message)
#define EG_ERROR(message)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::ERR, message)
#define EG_FATAL(message)    eagle::Log::client_log(EG_FILENAME, __FUNCTION__, __LINE__, eagle::Log::CRITICAL, message)


#endif //EAGLE_LOG_H
