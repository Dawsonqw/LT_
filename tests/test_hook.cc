#include "LT.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Hook.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_hook",lsink);



/**
 * @brief 测试sleep被hook之后的浆果
 */
void test_sleep() {
    g_logger->info("sleep begin");
    LT::IOManager iom;
    
    /**
     * 这里的两个协程sleep是同时开始的，一共只会睡眠3秒钟，第一个协程开始sleep后，会yield到后台，
     * 第二个协程会得到执行，最终两个协程都会yield到后台，并等待睡眠时间结束，相当于两个sleep是同一起点开始的
     */
    iom.schedule([] {
        sleep(2);
        g_logger->info("sleep 2");
    });

    iom.schedule([] {
        sleep(3);
        g_logger->info("sleep 3");
    });
    g_logger->info("sleep end");
}

/**
 * 测试socket api hook
 */
void test_sock() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    g_logger->info("connect ");
    int rt = connect(sock, (const sockaddr *) &addr, sizeof(addr));
    g_logger->info("connect rt={} errno={}", rt, errno);

    if (rt) {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    g_logger->info("send rt={} errno={}", rt, errno);
    if (rt <= 0) {
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    g_logger->info("recv rt={} errno={}", rt, errno);
    if (rt <= 0) {

        if (rt <= 0) {
            return;
        }

        buff.resize(rt);
        g_logger->info("buf:{}", buff);
    }
}

int main(int argc, char *argv[]) {

    // test_sleep();

    // 只有以协程调度的方式运行hook才能生效
    LT::IOManager iom;
    iom.schedule(test_sock);
  //  iom.schedule(test_sleep);

    g_logger->info("begin");
    return 0;
}
