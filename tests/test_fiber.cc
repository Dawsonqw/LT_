#include "LT.h"
#include <string>
#include <vector>

static LT::Logger::ptr g_logger = LT_LOG_ROOT();

void run_in_fiber2() {
    LT_LOG_INFO(g_logger) << "run_in_fiber2 begin";
    LT_LOG_INFO(g_logger) << "run_in_fiber2 end";
}

void run_in_fiber() {
    LT_LOG_INFO(g_logger) << "run_in_fiber begin";

    LT_LOG_INFO(g_logger) << "before run_in_fiber yield";
    LT::Fiber::GetThis()->yield();
    LT_LOG_INFO(g_logger) << "after run_in_fiber yield";

    LT_LOG_INFO(g_logger) << "run_in_fiber end";
    // fiber结束之后会自动返回主协程运行
}

void test_fiber() {
    LT_LOG_INFO(g_logger) << "test_fiber begin";

    // 初始化线程主协程
    LT::Fiber::GetThis();

    LT::Fiber::ptr fiber(new LT::Fiber(run_in_fiber, 0, false));
    LT_LOG_INFO(g_logger) << "use_count:" << fiber.use_count(); // 1

    LT_LOG_INFO(g_logger) << "before test_fiber resume";
    fiber->resume();
    LT_LOG_INFO(g_logger) << "after test_fiber resume";

    /** 
     * 关于fiber智能指针的引用计数为3的说明：
     * 一份在当前函数的fiber指针，一份在MainFunc的cur指针
     * 还有一份在在run_in_fiber的GetThis()结果的临时变量里
     */
    LT_LOG_INFO(g_logger) << "use_count:" << fiber.use_count(); // 3

    LT_LOG_INFO(g_logger) << "fiber status: " << fiber->getState(); // READY

    LT_LOG_INFO(g_logger) << "before test_fiber resume again";
    fiber->resume();
    LT_LOG_INFO(g_logger) << "after test_fiber resume again";

    LT_LOG_INFO(g_logger) << "use_count:" << fiber.use_count(); // 1
    LT_LOG_INFO(g_logger) << "fiber status: " << fiber->getState(); // TERM

    fiber->reset(run_in_fiber2); // 上一个协程结束之后，复用其栈空间再创建一个新协程
    fiber->resume();

    LT_LOG_INFO(g_logger) << "use_count:" << fiber.use_count(); // 1
    LT_LOG_INFO(g_logger) << "test_fiber end";
}

int main(int argc, char *argv[]) {
    LT::EnvMgr::GetInstance()->init(argc, argv);
    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

    LT::SetThreadName("main_thread");
    LT_LOG_INFO(g_logger) << "main begin";

    std::vector<LT::Thread::ptr> thrs;
    for (int i = 0; i < 2; i++) {
        thrs.push_back(LT::Thread::ptr(
            new LT::Thread(&test_fiber, "thread_" + std::to_string(i))));
    }

    for (auto i : thrs) {
        i->join();
    }

    LT_LOG_INFO(g_logger) << "main end";
    return 0;
}
