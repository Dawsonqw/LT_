/**
* @author dqw
 * @details协程测试
*/

/**
 * yield------>告知程序意味着挂起
 * resume----->告知将挂起的协程重新开始运行 恢复点？
 *
 *     t_cur_running---->对应普通协程 本身是一个裸指针
 *     t_main_finer----->对应GetThis()初始化的主协程  本身是一个指针指向主协程
 *
 */



///流程：
///主协程被创建GetThis()---->子协程被初始化fiber(cb)--(mainfunc通过makecontext绑定到ucontext上下文)
/// ---->子协程在主协程上下文执行resume操作--->子协程运行到swapcontext时运行第二个参数也就是发生交换后的上下文绑定的函数
/// （本协程都是绑定了Mainfunc作为入口函数 进行了一层封装)--->运行Mainfunc方法，执行协程绑定的回调函数
///---->如果回调里面有yield方法:在yield中方法执行该方法---->恢复主协程上下文
///----->如果主协程不再进行resume方法，那么上面这个协程绑定的方法会被遗弃，因此需要后面的协程调度器进行管理
///如果回调里面没有yield方法，那么子协程绑定函数将会运行完成---->
#include "LT.h"
#include "string"
#include "vector"

std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/fiber.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_address",lsink);

////由于是自己分配空间 如果空间分配不够会出错 默认是128*1024字节 也就是128k
void fiber_cb(){
    g_logger->info("cb begin");
///手动挂起 当前协程让出执行权 返回到主协程 也就是进入ucontext的u_next指向的上下文中
    LT::Fiber::GetThis()->yield();
    g_logger->info("cb end");
}

void fiber_cb_reset(){
    g_logger->info("cbreset begin");
    LT::Fiber::GetThis()->yield();
    g_logger->info("cbreset end");
}

void fiber_cb_test(){
    g_logger->info("cbreset_test start");
    LT::Fiber::GetThis()->yield();///当前协程主动yield
    g_logger->info("cbreset_test end");
}

void test_cb(){
    g_logger->info("test begin");
    LT::Fiber::GetThis();///返回当前线程正在执行的协程 如果没有就创建一个主协程作为当前运行的协程也就是被调用的普通协程（此时两者指向同一个协程对象）

    ///构造一个普通被调用协程
    LT::Fiber::ptr fiber(new LT::Fiber(fiber_cb,128*1024, false));

    g_logger->info("test resume");

    fiber->resume();///首先将当前上下文保存起来 也就是包存到主协程的上下文环境后进入fiber协程指向的普通协程上下文

///由于已经有了一个主协程 此时再通过LT::Fiber::GetThis()拿到的是一个当前运行的协程
//    LT::Fiber::ptr test=LT::Fiber::GetThis();///拿到的是主协程吧这里 这里是主协程吧

    g_logger->info("test resume");
    fiber->resume();
//    fiber->resume();


///复用已经创建好的协程
    fiber->reset(fiber_cb_reset);
    fiber->resume();///resume把fiber的控制权交给绑定的回调  yield把控制权交还
    g_logger->info("resume aggain");
    fiber->resume();

    LT::Fiber::ptr fiber1(new LT::Fiber(fiber_cb_test,128*1024,0));
    fiber1->resume();
    fiber1->resume();
}

int main(int argc,char* argv[]){
    std::vector<LT::Thread::ptr>thrs;

    g_logger->info("main begin");
    for(int i=0;i<15;i++){
        thrs.push_back(LT::Thread::ptr(new LT::Thread(&test_cb,"thread_"+std::to_string(i))));
    }

    for(int i=0;i<15;i++){
        thrs[i]->join();
    }

    g_logger->info("main end");
    return 0;
}