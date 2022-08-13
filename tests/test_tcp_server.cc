#include "LT.h"

//static LT::Logger::ptr g_logger = LT_LOG_ROOT();

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
  //  LT_LOG_INFO(g_logger) << "new client: " << client->toString();
    static std::string buf;
    buf.resize(4096);
    client->recv(&buf[0], buf.length()); // 这里有读超时，由tcp_server.read_timeout配置项进行配置，默认120秒
    //LT_LOG_INFO(g_logger) << "recv: " << buf;
    client->close();
}

void run() {
    LT::TcpServer::ptr server(new MyTcpServer); // 内部依赖shared_from_this()，所以必须以智能指针形式创建对象
    auto addr = LT::Address::LookupAny("127.0.0.1:8088");
    LT_ASSERT(addr);
    std::vector<LT::Address::ptr> addrs;
    addrs.push_back(addr);

    std::vector<LT::Address::ptr> fails;
    while(!server->bind(addrs, fails)) {
        sleep(2);
    }
    
    //LT_LOG_INFO(g_logger) << "bind success, " << server->toString();

    server->start();
}

int main(int argc, char *argv[]) {
    LT::IOManager iom(2);
    iom.schedule(&run);

    return 0;
}
