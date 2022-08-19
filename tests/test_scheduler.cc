#include "LT.h"


std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/scheduler.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();


static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_scheduler",lsink);


/**
 * @brief 演示协程主动yield情况下应该如何操作
 */
void test_fiber1() {
    g_logger->info("begin");
    /**
     * 协程主动让出执行权，在yield之前，协程必须再次将自己添加到调度器任务队列中，
     */
    LT::Scheduler::GetThis()->schedule(LT::Fiber::GetThis());

    LT::Fiber::GetThis()->yield();

    g_logger->info("ex again");
}

/**
 * @brief 演示协程睡眠对主程序的影响
 */
void test_fiber2() {
    /**
     * 一个线程同一时间只能有一个协程在运行，线程调度协程的本质就是按顺序执行任务队列里的协程
     */
    sleep(3);
}

void test_fiber3() {
    g_logger->info("test begin");
    g_logger->info("test end");
}

void test_fiber4() {
    static int count = 0;
    count++;
}


int main() {
    /**
     * 只使用main函数线程进行协程调度，相当于先攒下一波协程，然后切换到调度器的run方法将这些协程
     * 消耗掉，然后再返回main函数往下执行
     */
    LT::Scheduler sc;


    // 添加调度任务，使用函数作为调度对象
    sc.schedule(test_fiber1);
    sc.schedule(test_fiber2);

    // 添加调度任务，使用Fiber类作为调度对象
    LT::Fiber::ptr fiber(new LT::Fiber(test_fiber3));
    sc.schedule(fiber);

    // 创建调度线程，开始任务调度，如果只使用main函数线程进行调度，那start相当于什么也没做
    sc.start();

    /**
     * 只要调度器未停止，就可以添加调度任务
     * 包括在子协程中也可以通过LT::Scheduler::GetThis()->scheduler()的方式继续添加调度任务
     */
    sc.schedule(test_fiber4);

    /**
     * 停止调度，如果未使用当前线程进行调度，那么只需要简单地等所有调度线程退出即可
     * 如果使用了当前线程进行调度，那么要先执行当前线程的协程调度函数，等其执行完后再返回caller协程继续往下执行
     */
    sc.stop();
    return 0;
}
