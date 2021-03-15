#ifndef EAGLE_LOG_H
#define EAGLE_LOG_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define EG_CREATE_LOGGER(name) (spdlog::create<spdlog::sinks::stdout_color_sink_mt>(name))

#define EG_LOG(name, level, ...)    SPDLOG_LOGGER_CALL(spdlog::get(name), level, __VA_ARGS__)

#define EG_TRACE(name, ...)         EG_LOG(name, spdlog::level::trace, __VA_ARGS__)
#define EG_INFO(name, ...)          EG_LOG(name, spdlog::level::info, __VA_ARGS__)
#define EG_DEBUG(name, ...)         EG_LOG(name, spdlog::level::debug, __VA_ARGS__)
#define EG_WARNING(name, ...)       EG_LOG(name, spdlog::level::warn, __VA_ARGS__)
#define EG_ERROR(name, ...)         EG_LOG(name, spdlog::level::err, __VA_ARGS__)
#define EG_FATAL(name, ...)         EG_LOG(name, spdlog::level::critical, __VA_ARGS__)

#endif //EAGLE_LOG_H
