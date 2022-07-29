#ifndef LOG_H
#define LOG_H
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/daily_file_sink.h"//每天增加一个日志文件
#include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging

///全局sink 各个类中分别创建logger
static std::shared_ptr<spdlog::sinks::daily_file_sink_mt>g_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("Logs/DayLogs.txt", 2, 0);

//g_sink->set_level(spdlog::level::trace);
//g_sink->set_pattern("%Y - %m - %d %H:%M:%S (%F) [信息] %v");

#endif
