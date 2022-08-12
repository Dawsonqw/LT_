#include "thread.h"
#include "log.h"
#include "util.h"

namespace LT {

static lsinks lsink{g_sink,c_sink};
static auto g_logger = std::make_shared<spdlog::logger>("thread",lsink);

    ///线程局部变量：thread_local 线程开始时创建变量 结束时释放变量 全局有效而不会被其它线程影响
static thread_local Thread *t_thread          = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

Thread *Thread::GetThis() {
    return t_thread;
}

const std::string &Thread::GetName() {
    return t_thread_name;
}

void Thread::SetName(const std::string &name) {
    if (name.empty()) {
        return;
    }
    if (t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string &name)
    : m_cb(cb)
    , m_name(name) {
    if (name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
		g_logger->error("pthread_create thread fail, rt={} name={}",rt,name);
        throw std::logic_error("pthread_create error");
    }
    ///这里信号量用于等待线程完全开启后退出构造
    m_semaphore.wait();
}

Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::join() {
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
			g_logger->error("pthread_join thread fail, rt={} name={}", rt, m_name);
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

///函数签名void*(void*) 这里是一个静态方法 由于需要使用到类中的方法 而静态方法是无法访问到类中的非静态变量的 因此需要传入this指针进行访问
void *Thread::run(void *arg) {
    ///线程局部变量的使用 当前线程有效
    ///这里传入的指针是this指针 进而实现了静态方法中使用到非静态数据成员
    Thread *thread = (Thread *)arg;
    t_thread       = thread;
    t_thread_name  = thread->m_name;
    thread->m_id   = LT::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

	//显示释放
    std::function<void()> cb;
    cb.swap(thread->m_cb);

    ///通知构造完成
    thread->m_semaphore.notify();

    cb();
    return 0;
}

} // namespace LT
