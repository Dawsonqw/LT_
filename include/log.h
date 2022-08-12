#ifndef LOG_H
#define LOG_H
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"//每天增加一个日志文件
#include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging
#include "spdlog/common.h"

    // ///全局sink 各个类中分别创建logger
   static  std::shared_ptr<spdlog::sinks::daily_file_sink_mt>g_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("Logs/DayLogs.txt", 2, 0);
    static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> c_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    using sptr = std::shared_ptr<spdlog::sinks::sink>;
    using lsinks = std::initializer_list<sptr>;
    //std::vector<spdlog::sink_ptr>g_SinkVec{g_sink,c_sink};
#endif
