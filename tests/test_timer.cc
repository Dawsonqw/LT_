#include "LT.h"

std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/timer.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();


static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_timer",lsink);


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
