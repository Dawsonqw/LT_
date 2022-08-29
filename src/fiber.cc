#include <atomic>
#include "fiber.h"
#include "log.h"
#include "marco.h"
#include "scheduler.h"

namespace LT {

static lsinks lsink{g_sink,c_sink};
static auto g_logger = std::make_shared<spdlog::logger>("fiber",lsink);


/// 全局静态变量，用于生成协程id
static std::atomic<uint64_t> s_fiber_id{0};
/// 全局静态变量，用于统计当前的协程数
static std::atomic<uint64_t> s_fiber_count{0};

/// 线程局部变量，当前线程正在运行的协程
static thread_local Fiber *p_cur_running_fiber = nullptr;
/// 线程局部变量，当前线程的主协程，切换到这个协程，就相当于切换到了主线程中运行，智能指针形式
static thread_local Fiber::ptr main_fiber = nullptr;

/**
 * @brief malloc栈内存分配器
 */
class MallocStackAllocator {
public:
    static void *Alloc(size_t size) { return malloc(size); }
    static void Dealloc(void *vp, size_t size) { return free(vp); }
};

using StackAllocator = MallocStackAllocator;

///当前运行协程id
uint64_t Fiber::GetFiberId() {
    if (p_cur_running_fiber) {
        return p_cur_running_fiber->getId();
    }
    return 0;
}

Fiber::Fiber() {
    SetThis(this);
    m_state = RUNNING;

    if (getcontext(&m_ctx)) {
        LT_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;
    m_id = s_fiber_id++; // 协程id从0开始，用完加1
	g_logger->debug("Fiber::Fiber() main id = {}",m_id);
}

void Fiber::SetThis(Fiber *fiber) {
    p_cur_running_fiber = fiber;
}

/**
 * 获取当前协程，同时充当初始化当前线程主协程的作用，这个函数在使用协程之前要调用一下
 * todo:这里将创建和返回主协程写在一起 可以分开
 */
Fiber::ptr Fiber::GetThis() {
    if (p_cur_running_fiber) {
        return p_cur_running_fiber->shared_from_this();
    }

    ///没有主协程存在
    Fiber::ptr m_fiber(new Fiber);
    LT_ASSERT(p_cur_running_fiber == m_fiber.get());
     main_fiber= m_fiber;
    return p_cur_running_fiber->shared_from_this();
}

/**
 * 带参数的构造函数用于创建其他协程，需要分配栈
 */
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler)
    : m_id(s_fiber_id++)
    , m_cb(cb)
    , m_runInScheduler(run_in_scheduler) {
    ///回调实际上这里是作为一个私有数据成员 调用时机是在makecontext的入口函数中，也就是在发生swapcontext时
    ///而上下文的初始化仅仅设计栈空间 栈大小以及下一个指向 这里下一个指向没有赋值 因为无法控制
    ++s_fiber_count;
    m_stacksize = 128*1024;
    m_stack     = StackAllocator::Alloc(m_stacksize);

    if (getcontext(&m_ctx)) {
        LT_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link          = nullptr;
    m_ctx.uc_stack.ss_sp   = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    //入口函数：MAinFunc
    makecontext(&m_ctx, &Fiber::MainFunc, 0);

	g_logger->debug("Fiber::Fiber() id = {}", m_id);
}

/**
 * 线程的主协程析构时需要特殊处理，因为主协程没有分配栈和cb
 */
Fiber::~Fiber() {
	g_logger->debug("Fiber::~Fiber() id = {}", m_id);
    --s_fiber_count;
    if (m_stack) {
        // 有栈，说明是子协程，需要确保子协程一定是结束状态
        LT_ASSERT(m_state == TERM);
        StackAllocator::Dealloc(m_stack, m_stacksize);
		g_logger->debug("dealloc stack, id = {}", m_id);
    } else {
        // 没有栈，说明是线程的主协程
        LT_ASSERT(!m_cb);              // 主协程没有cb
        LT_ASSERT(m_state == RUNNING); // 主协程一定是执行状态

        Fiber *cur = p_cur_running_fiber; // 当前协程就是自己
        if (cur == this) {
            SetThis(nullptr);
        }
    }
}

/**
 * 这里为了简化状态管理，强制只有TERM状态的协程才可以重置，但其实刚创建好但没执行过的协程也应该允许重置的
 */
void Fiber::reset(std::function<void()> cb) {
    LT_ASSERT(m_stack);
    LT_ASSERT(m_state == TERM);
    m_cb = cb;
    if (getcontext(&m_ctx)) {
        LT_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link          = nullptr;
    m_ctx.uc_stack.ss_sp   = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = READY;
}

///恢复到子协程执行：逻辑：保存调用swapcontext 保存当前cpu到主协程环境，加载当前协程环境 明确：yield和resume执行主体都是子fiber
///mainfunc中执行时机swapcontext或者setcontext 因此这里resume以及yield时都会切换到函数主体中进行执行
void Fiber::resume() {
    LT_ASSERT(m_state != TERM && m_state != RUNNING);
    SetThis(this);
    m_state = RUNNING;

    // 如果协程参与调度器调度，那么应该和调度器的主协程进行swap，而不是线程主协程
    if (m_runInScheduler) {
        if (swapcontext(&(Scheduler::GetMainFiber()->m_ctx), &m_ctx)) {
            LT_ASSERT2(false, "swapcontext");
        }
    } else {
        if (swapcontext(&(main_fiber->m_ctx), &m_ctx)) {
            LT_ASSERT2(false, "swapcontext");
        }
    }
}

void Fiber::yield() {
    /// 协程运行完之后会自动yield一次，用于回到主协程，此时状态已为结束状态
    LT_ASSERT(m_state == RUNNING || m_state == TERM);
    SetThis(main_fiber.get());
    if (m_state != TERM) {
        m_state = READY;
    }

    // 如果协程参与调度器调度，那么应该和调度器的主协程进行swap，而不是线程主协程
    if (m_runInScheduler) {
        if (swapcontext(&m_ctx, &(Scheduler::GetMainFiber()->m_ctx))) {
            LT_ASSERT2(false, "swapcontext");
        }
    } else {
        if (swapcontext(&m_ctx, &(main_fiber->m_ctx))) {
            LT_ASSERT2(false, "swapcontext");
        }
    }
}

/**
 * 函数入口
 */
void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis(); // GetThis()的shared_from_this()方法让引用计数加1
    //LT_ASSERT(cur);

    //执行回调函数 结束执行后改变状态
    cur->m_cb();
    cur->m_cb    = nullptr;
    cur->m_state = TERM;

    auto raw_ptr = cur.get(); // 手动让p_cur_running_fiber的引用计数减1
    cur.reset();
    raw_ptr->yield();
}

} // namespace LT
