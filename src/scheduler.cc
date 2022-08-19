#include <vector>
#include "scheduler.h"
#include "log.h"
#include "macro.h"
#include "hook.h"

namespace LT {


static lsinks lsink{g_sink,c_sink};
static auto g_logger = std::make_shared<spdlog::logger>("scheduler",lsink);


/// 当前线程的调度器，同一个调度器下的所有线程共享同一个实例
    static thread_local Scheduler *t_scheduler = nullptr;
/// 当前线程的调度协程，每个线程都独有一份
    static thread_local Fiber *t_scheduler_fiber = nullptr;///理解为主协程

    Scheduler::Scheduler(size_t threads, bool use_caller, const std::string &name) {
        LT_ASSERT(threads > 0);

        m_useCaller = use_caller;
        m_name      = name;

        if (use_caller) {
            --threads;
            LT::Fiber::GetThis();
            LT_ASSERT(GetThis() == nullptr);
            t_scheduler = this;

            /**
             * caller线程的主协程不会被线程的调度协程run进行调度，而且，线程的调度协程停止时，应该返回caller线程的主协程
             * 在user caller情况下，把caller线程的主协程暂时保存起来，等调度协程结束时，再resume caller协程
             */
            m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, false));

            LT::Thread::SetName(m_name);
            t_scheduler_fiber = m_rootFiber.get();
            m_rootThread      = LT::GetThreadId();
            m_threadIds.push_back(m_rootThread);
        } else {
            m_rootThread = -1;
        }
        m_threadCount = threads;
    }

    Scheduler *Scheduler::GetThis() {
        return t_scheduler;
    }

    Fiber *Scheduler::GetMainFiber() {
        return t_scheduler_fiber;
    }

    void Scheduler::setThis() {
        t_scheduler = this;
    }

    Scheduler::~Scheduler() {
		g_logger->debug("Scheduler::~Scheduler()");
        LT_ASSERT(m_stopping);
        if (GetThis() == this) {
            t_scheduler = nullptr;
        }
    }

    void Scheduler::start() {
		g_logger->info("start");
        MutexType::Lock lock(m_mutex);
        if (m_stopping) {
			g_logger->error("Scheduler is stopped");
            return;
        }
        LT_ASSERT(m_threads.empty());
        m_threads.resize(m_threadCount);
        for (size_t i = 0; i < m_threadCount; i++) {
            m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                                          m_name + "_" + std::to_string(i)));
            m_threadIds.push_back(m_threads[i]->getId());
        }
    }

    bool Scheduler::stopping() {
        MutexType::Lock lock(m_mutex);
        return m_stopping && m_tasks.empty() && m_activeThreadCount == 0;
    }

    void Scheduler::tickle() {
		g_logger->debug("ticlke");
    }

    void Scheduler::idle() {
		g_logger->debug("idle");
        while (!stopping()) {
            LT::Fiber::GetThis()->yield();
        }
    }

    void Scheduler::stop() {
		g_logger->debug("stop");
        if (stopping()) {
            return;
        }
        m_stopping = true;

        /// 如果use caller，那只能由caller线程发起stop
        if (m_useCaller) {
            LT_ASSERT(GetThis() == this);
        } else {
            LT_ASSERT(GetThis() != this);
        }

        for (size_t i = 0; i < m_threadCount; i++) {
            tickle();
        }

        if (m_rootFiber) {
            tickle();
        }

        /// 在use caller情况下，调度器协程结束时，应该返回caller协程
        if (m_rootFiber) {
            m_rootFiber->resume();
			g_logger->debug("m_rootFiber end");
        }

        std::vector<Thread::ptr> thrs;
        {
            MutexType::Lock lock(m_mutex);
            thrs.swap(m_threads);
        }
        for (auto &i : thrs) {
            i->join();
        }
    }

    void Scheduler::run() {
		g_logger->debug("run");
        set_hook_enable(true);
        setThis();

        if (LT::GetThreadId() != m_rootThread) {
            t_scheduler_fiber = LT::Fiber::GetThis().get();
        }

        Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
        Fiber::ptr cb_fiber;

        ScheduleTask task;
        while (true) {
            task.reset();
            bool tickle_me = false; // 是否tickle其他线程进行任务调度
            {
                MutexType::Lock lock(m_mutex);
                auto it = m_tasks.begin();
                // 遍历所有调度任务
                while (it != m_tasks.end()) {
                    if (it->thread != -1 && it->thread != LT::GetThreadId()) {
                        // 指定了调度线程，但不是在当前线程上调度，标记一下需要通知其他线程进行调度，然后跳过这个任务，继续下一个
                        ++it;
                        tickle_me = true;
                        continue;
                    }

                    // 找到一个未指定线程，或是指定了当前线程的任务
                    LT_ASSERT(it->fiber || it->cb);

                    if(it->fiber && it->fiber->getState() == Fiber::RUNNING) {
                        ++it;
                        continue;
                    }

                    // 当前调度线程找到一个任务，准备开始调度，将其从任务队列中剔除，活动线程数加1
                    task = *it;
                    m_tasks.erase(it++);
                    ++m_activeThreadCount;
                    break;
                }
                // 当前线程拿完一个任务后，发现任务队列还有剩余，那么tickle一下其他线程
                tickle_me |= (it != m_tasks.end());
            }

            if (tickle_me) {
                tickle();
            }

            ///这里拿到协程后 通过resume中执行swapcontext，进入回调函数执行 强调：协程绑定或者说包装了回调，通过yield和resume进行控制
            if (task.fiber) {
                // resume协程，resume返回时，协程要么执行完了，要么半路yield了，活跃线程数减一
                task.fiber->resume();
                --m_activeThreadCount;
                task.reset();
            } else if (task.cb) {
                if (cb_fiber) {
                    cb_fiber->reset(task.cb);
                } else {
                    cb_fiber.reset(new Fiber(task.cb));
                }
                task.reset();
                cb_fiber->resume();
                --m_activeThreadCount;
                cb_fiber.reset();
            } else {
                // 进到这个分支情况一定是任务队列空了，调度idle协程即可
                if (idle_fiber->getState() == Fiber::TERM) {
                    // 如果调度器没有调度任务，那么idle协程会不停地resume/yield，不会结束，如果idle协程结束了，那一定是调度器停止了
					g_logger->debug("idle fiber term");
                    break;
                }
                ++m_idleThreadCount;
                idle_fiber->resume();
                --m_idleThreadCount;
            }
        }
		g_logger->debug("Scheduler::run() exit");
    }

} 
