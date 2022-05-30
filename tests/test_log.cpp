
#include "LT.h"

#include <unistd.h>

LT::Logger::ptr logger = LT_LOG_ROOT(); // 默认INFO级别

int main(int argc, char *argv[]) {
//    LT::EnvMgr::GetInstance()->init(argc, argv);
//    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

    LT_LOG_FATAL(logger) << "fatal msg";
    LT_LOG_ERROR(logger) << "err msg";
    LT_LOG_INFO(logger) << "info msg";
    LT_LOG_DEBUG(logger) << "debug msg";

    LT_LOG_FMT_FATAL(logger, "fatal %s:%d", __FILE__, __LINE__);
    LT_LOG_FMT_ERROR(logger, "err %s:%d", __FILE__, __LINE__);
    LT_LOG_FMT_INFO(logger, "info %s:%d", __FILE__, __LINE__);
    LT_LOG_FMT_DEBUG(logger, "debug %s:%d", __FILE__, __LINE__);
   
    sleep(1);
    LT::SetThreadName("brand_new_thread");

    logger->setLevel(LT::LogLevel::WARN);
    LT_LOG_FATAL(logger) << "fatal msg";
    LT_LOG_ERROR(logger) << "err msg";
    LT_LOG_INFO(logger) << "info msg"; // 不打印
    LT_LOG_DEBUG(logger) << "debug msg"; // 不打印


    LT::FileLogAppender::ptr fileAppender(new LT::FileLogAppender("../tmp/log.txt"));
    logger->addAppender(fileAppender);
    LT_LOG_FATAL(logger) << "fatal msg";
    LT_LOG_ERROR(logger) << "err msg";
    LT_LOG_INFO(logger) << "info msg"; // 不打印
    LT_LOG_DEBUG(logger) << "debug msg"; // 不打印

    LT::Logger::ptr test_logger = LT_LOG_NAME("test_logger");
    LT::StdoutLogAppender::ptr appender(new LT::StdoutLogAppender);
    LT::LogFormatter::ptr formatter(new LT::LogFormatter("%d:%rms%T%p%T%c%T%f:%l %m%n")); // 时间：启动毫秒数 级别 日志名称 文件名：行号 消息 换行
    appender->setFormatter(formatter);
    test_logger->addAppender(appender);
    test_logger->setLevel(LT::LogLevel::WARN);

    LT_LOG_ERROR(test_logger) << "err msg";
    LT_LOG_INFO(test_logger) << "info msg"; // 不打印

    // 输出全部日志器的配置
//    logger->setLevel(LT::LogLevel::INFO);
    LT_LOG_INFO(logger) << "logger config:" << LT::LoggerMgr::GetInstance()->toYamlString();

    return 0;
}
