#include "LT.h"

static LT::Logger::ptr g_logger = LT_LOG_ROOT();

static int timeout = 1000;
static LT::Timer::ptr s_timer;

void g_callback() {
    LT_LOG_INFO(g_logger) << "timer callback, timeout = " << timeout;
    timeout += 1000;
    if(timeout < 5000) {
        s_timer->reset(timeout, true);
    } else {
        s_timer->cancel();
    }
}

void test_timer() {
    LT::IOManager iom;

    // 循环定时器
    s_timer = iom.addTimer(1000, g_callback, true);
    
    // 单次定时器
    iom.addTimer(500, []{
        LT_LOG_INFO(g_logger) << "500ms timeout";
    });
    iom.addTimer(5000, []{
        LT_LOG_INFO(g_logger) << "5000ms timeout";
    });
}

int main(int argc, char *argv[]) {
    LT::EnvMgr::GetInstance()->init(argc, argv);
    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

    test_timer();

    LT_LOG_INFO(g_logger) << "end";

    return 0;
}
