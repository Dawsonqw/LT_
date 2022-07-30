#include "LT.h"

auto g_logger=std::make_shared<spdlog::logger>("gLog", g_sink);

static int timeout = 1000;
static LT::Timer::ptr s_timer;

void g_callback() {
    g_logger->info("timer cb timeout:{}",timeout);
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
        g_logger->info("cb");
    });
    iom.addTimer(5000, []{
        g_logger->info("5000 cb");
    });
}

int main(int argc, char *argv[]) {

    test_timer();

    return 0;
}
