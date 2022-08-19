//
// Created by dqw on 2022/4/22.
#include "LT.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/iomanger.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();


static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_iomanger",lsink);
int sockfd;

void iom_cb(){
    g_logger->info("cb start");
    LT::IOManager::GetThis()->delEvent(sockfd,LT::IOManager::READ);
    LT::IOManager::GetThis()->addEvent(sockfd,LT::IOManager::READ,iom_cb);///重复注册一直递归处理
    g_logger->info("cb end");
}
void iom_cb1(){
    g_logger->info("cb start");
    LT::Scheduler::GetThis()->schedule(LT::Fiber::GetThis());
    LT::Fiber::GetThis()->yield();
    g_logger->info("cb end");
}



int main(int argc, char *argv[]) {
    ///注册一个fd   添加读写事件

    sockfd= socket(AF_INET,SOCK_STREAM,0);
    int sock_fd= socket(AF_INET,SOCK_STREAM,0);
    LT_ASSERT(sockfd > 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8087);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);


    fcntl(sock_fd, F_SETFL, O_NONBLOCK);
    sockaddr_in servaddr_;
    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_port = htons(8088);
    inet_pton(AF_INET, "127.0.0.1", &servaddr_.sin_addr.s_addr);

    bind(sockfd,(sockaddr*)&servaddr,sizeof (servaddr));
    bind(sock_fd,(sockaddr*)&servaddr_,sizeof (servaddr_));
    listen(sockfd,5);
    listen(sock_fd,10);
    g_logger->info("main start");


    LT::IOManager iom;
////当回调功能结束后再次将自己加入到任务队列应该可以重复执行才对 需要手动管理

    LT::IOManager::GetThis()->addEvent(sockfd,LT::IOManager::READ,iom_cb);///只能触发一次

    LT::IOManager::GetThis()->addEvent(sock_fd,LT::IOManager::READ,[](){
        g_logger->info("received");
    });

    //iom.cancelAll(sock_fd);
    //iom.delEvent(sockfd,LT::IOManager::READ);
    iom.start();


    iom.stop();
    g_logger->info("main end");
    return 0;
}
//

