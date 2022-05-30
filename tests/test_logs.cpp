//
// Created by dqw on 2022/4/21.
//

#include "iostream"
#include "LT.h"
int main(){
    LT::Logger::ptr logger= LT_LOG_NAME("test");
    LT::LogFormatter::ptr formatter(new LT::LogFormatter);
    LT::LogAppender::ptr appender(new LT::StdoutLogAppender);
    appender->setFormatter(formatter);
    logger->addAppender(appender);
    logger->setLevel(LT::LogLevel::NOTSET);
    LT_LOG_FMT_DEBUG(logger,"fatal %s: %d",__FILE__,__LINE__);
    LT_LOG_FATAL(logger)<<"FATAL is ok";
    LT_LOG_ERROR(logger)<<"error is ok";
    LT_LOG_NOTICE(logger)<<"notice is ok";
    LT_LOG_DEBUG(logger)<<"debug is ok";

    LT::Logger::ptr root=LT_LOG_ROOT();
    LT::LogFormatter::ptr rootfor(new LT::LogFormatter);
    LT::LogAppender::ptr rappender(new LT::StdoutLogAppender);
    rappender->setFormatter(rootfor);
    root->addAppender(rappender);
    root->setLevel(LT::LogLevel::NOTICE);///
    LT_LOG_ERROR(root)<<"root error is ok";
    LT_LOG_DEBUG(root)<<"root debug is ok";
    LT_LOG_FATAL(root)<<"root fatal is ok";
    LT_LOG_WARN(root)<<"root warn is ok";


    return 0;
}