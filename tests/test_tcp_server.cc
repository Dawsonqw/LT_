#include "LT.h"

std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/tcpserver.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();


static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_tcp_server",lsink);

/**
 * @brief 自定义TcpServer类，重载handleClient方法
 */
class MyTcpServer : public LT::TcpServer {
protected:
    virtual void handleClient(LT::Socket::ptr client) override;
};

void MyTcpServer::handleClient(LT::Socket::ptr client) {
    g_logger->info("new client:{}",client->toString());
    static std::string buf;
    buf.resize(4096);
    client->recv(&buf[0], buf.length()); // 这里有读超时，由配置项进行配置，默认120秒
    printf("recv msg:%s\n",buf.c_str());
    g_logger->info("recv:{}",buf);
    client->close();
}

void run() {
    LT::TcpServer::ptr server(new MyTcpServer); // 内部依赖shared_from_this()，所以必须以智能指针形式创建对象
    std::string serveraddr=LT::JsonMg::GetInstance()->GetVal("../conf/g_Config.json","TcpServer","server","127.0.0.1:8080");
    auto addr = LT::Address::LookupAny(serveraddr);

    LT_ASSERT(addr);
    std::vector<LT::Address::ptr> addrs;
    addrs.push_back(addr);

    std::vector<LT::Address::ptr> fails;
    while(!server->bind(addrs, fails)) {
        sleep(2);
    }

    g_logger->info("bind success {}",server->toString());
    server->start();
}

int main(int argc, char *argv[]) {
    g_logger->set_level(spdlog::level::trace);
    LT::IOManager iom(2);
    iom.schedule(&run);

    return 0;
}
