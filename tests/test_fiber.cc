#include "LT.h"
#include <string>
#include <vector>


std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/fiber.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_fiber",lsink);



void run_in_fiber2() {
    g_logger->info("r2 begin");
    g_logger->info("r2 end");
}

void run_in_fiber() {
    g_logger->info("before yield");
    LT::Fiber::GetThis()->yield();
    g_logger->info("before end");
    // fiber结束之后会自动返回主协程运行
}

void test_fiber() {
    g_logger->info("test_fiber begin");

    // 初始化线程主协程
    LT::Fiber::GetThis();

    LT::Fiber::ptr fiber(new LT::Fiber(run_in_fiber, 128*1024, false));
    g_logger->info("use count:{}",fiber.use_count());
    g_logger->info("resume");
    fiber->resume();
    g_logger->info("resume end");

    /** 
     * 关于fiber智能指针的引用计数为3的说明：
     * 一份在当前函数的fiber指针，一份在MainFunc的cur指针
     * 还有一份在在run_in_fiber的GetThis()结果的临时变量里
     */

    fiber->resume();
    fiber->reset(run_in_fiber2); // 上一个协程结束之后，复用其栈空间再创建一个新协程
    fiber->resume();
}

int main(int argc, char *argv[]) {

    LT::SetThreadName("main_thread");
    g_logger->info("main begin");

    std::vector<LT::Thread::ptr> thrs;
    for (int i = 0; i < 1; i++) {
        thrs.push_back(LT::Thread::ptr(
            new LT::Thread(&test_fiber, "thread_" + std::to_string(i))));
    }

    for (auto i : thrs) {
        i->join();
    }
    g_logger->info("main end");
    return 0;
}
