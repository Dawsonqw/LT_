#include "LT.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>


std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/iomanger.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();


static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_iomanger",lsink);

int sockfd;
void watch_io_read();

// 写事件回调，只执行一次，用于判断非阻塞套接字connect成功
void do_io_write() {
    g_logger->info("write cb");
    int so_err;
    socklen_t len = size_t(so_err);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_err, &len);
    if(so_err) {
        g_logger->info("connect failed");
        return;
    }
    g_logger->info("connect suc");
}

// 读事件回调，每次读取之后如果套接字未关闭，需要重新添加
void do_io_read() {
    g_logger->info("read cb");
    char buf[1024] = {0};
    int readlen = 0;
    readlen = read(sockfd, buf, sizeof(buf));
    if(readlen > 0) {
        buf[readlen] = '\0';
        g_logger->info("readlen :{},buf:{}",readlen,buf);
    } else if(readlen == 0) {
        g_logger->info("close");
        close(sockfd);
        return;
    } else {
        g_logger->info("errno:{} errstr:{}",errno, strerror(errno));
        close(sockfd);
        return;
    }
    // read之后重新添加读事件回调，这里不能直接调用addEvent，因为在当前位置fd的读事件上下文还是有效的，直接调用addEvent相当于重复添加相同事件
    LT::IOManager::GetThis()->schedule(watch_io_read);
}

void watch_io_read() {
    g_logger->info("read");
    LT::IOManager::GetThis()->addEvent(sockfd, LT::IOManager::READ, do_io_read);
}

void test_io() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    LT_ASSERT(sockfd > 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);

    int rt = connect(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr));
    if(rt != 0) {
        if(errno == EINPROGRESS) {
            g_logger->info("einprogress");
            // 注册写事件回调，只用于判断connect是否成功
            // 非阻塞的TCP套接字connect一般无法立即建立连接，要通过套接字可写来判断connect是否已经成功
            LT::IOManager::GetThis()->addEvent(sockfd, LT::IOManager::WRITE, do_io_write);
            // 注册读事件回调，注意事件是一次性的
            LT::IOManager::GetThis()->addEvent(sockfd, LT::IOManager::READ, do_io_read);
        } else {
            g_logger->info("connect error:{} errstr {}",errno, strerror(errno));
        }
    } else {
        g_logger->info("connect error:{} errstr {}",errno, strerror(errno));
    }
}

void test_iomanager() {
    LT::IOManager iom;
    // LT::IOManager iom(10); // 演示多线程下IO协程在不同线程之间切换
    iom.schedule(test_io);
}

int main(int argc, char *argv[]) {

    test_iomanager();

    return 0;
}
