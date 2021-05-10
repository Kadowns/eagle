#ifndef EAGLE_LOG_H
#define EAGLE_LOG_H

#include <spdlog/spdlog.h>


#ifdef EG_PLATFORM_ANDROID
#include <spdlog/sinks/android_sink.h>
#define EG_SINK_TYPE spdlog::sinks::android_sink_mt
#else
#include <spdlog/sinks/stdout_color_sinks.h>
#define EG_SINK_TYPE spdlog::sinks::stdout_color_sink_mt

//from minwindef.h
#undef far
#undef near

#endif

#define EG_LOG_CREATE(name) (spdlog::create<EG_SINK_TYPE>(name))
#define EG_LOG_PATTERN(pattern) (spdlog::set_pattern(pattern))
#define EG_LOG_LEVEL(level) (spdlog::set_level(level))

#define EG_LOG(name, level, ...)    SPDLOG_LOGGER_CALL(spdlog::get(name), level, __VA_ARGS__)

#define EG_TRACE(name, ...)         EG_LOG(name, spdlog::level::trace, __VA_ARGS__)
#define EG_INFO(name, ...)          EG_LOG(name, spdlog::level::info, __VA_ARGS__)
#define EG_DEBUG(name, ...)         EG_LOG(name, spdlog::level::debug, __VA_ARGS__)
#define EG_WARNING(name, ...)       EG_LOG(name, spdlog::level::warn, __VA_ARGS__)
#define EG_ERROR(name, ...)         EG_LOG(name, spdlog::level::err, __VA_ARGS__)
#define EG_CRITICAL(name, ...)         EG_LOG(name, spdlog::level::critical, __VA_ARGS__)

#endif //EAGLE_LOG_H
