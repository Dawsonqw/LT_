#include "LT.h"

static LT::Logger::ptr g_logger = LT_LOG_ROOT();

LT::Timer::ptr timer;
int server_main(int argc, char **argv) {
    LT_LOG_INFO(g_logger) << LT::ProcessInfoMgr::GetInstance()->toString();
    LT::IOManager iom(1);
    timer = iom.addTimer(
        1000, []() {
            LT_LOG_INFO(g_logger) << "onTimer";
            static int count = 0;
            if (++count > 10) {
                exit(1);
            }
        },
        true);
    return 0;
}

int main(int argc, char **argv) {
    return LT::start_daemon(argc, argv, server_main, argc != 1);
}