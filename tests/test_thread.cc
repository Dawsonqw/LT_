#include "LT.h"

///注意：需要在关优化的情况下进行测试 否则优化后的代码与原来语义可能有差别

LT::Logger::ptr g_logger = LT_LOG_ROOT();

long count = 0;
//LT::NullMutex s_mutex;
//LT::RWMutex s_mutex;
LT::Mutex s_mutex;

void func1(void *arg) {
    LT_LOG_INFO(g_logger) << "name:" << LT::Thread::GetName()
                          << " this.name:" << LT::Thread::GetThis()->getName()
                          << " thread name:" << LT::Thread::GetName()
                          << " id:" << LT::GetThreadId()
                          << " this.id:" << LT::Thread::GetThis()->getId();
    LT_LOG_INFO(g_logger) << "arg: " << *(int*)arg;
    for(int i = 0; i < 1000000; i++) {
//        LT::RWMutex::WriteLock lock(s_mutex);///写锁 2ms  完成在3603ms
//            LT::NullMutex::Lock lock(s_mutex);///没有锁的情况  结果显然不对 花费31  223ms
        LT::Mutex::Lock lock(s_mutex);///普通锁 2ms 完成在1639ms
//              LT::RWMutex::ReadLock  lock(s_mutex);///读写锁 读锁 不正确并且时间长

        ++count;
    }
}


int num=0;
LT::NullMutex m_mutex;
void test(void *arg){
    for(int i=0;i<10000;i++){///
        LT::NullMutex::Lock lock(m_mutex);
        num++;
    }
}

int main(int argc, char *argv[]) {
    LT::EnvMgr::GetInstance()->init(argc, argv);
    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

    std::vector<LT::Thread::ptr> thrs;
    int arg = 123456;
    for(int i = 0; i < 100; i++) {
        // 带参数的线程用std::bind进行参数绑定
        LT::Thread::ptr thr(new LT::Thread(std::bind(func1, &arg), "g_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for(int i = 0; i < 100; i++) {
        thrs[i]->join();
    }

    LT_LOG_INFO(g_logger) << "count = " <<count;
    return 0;
}

