#include <LT.h>

std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/sockettcps.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();


static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_socket_tcp_s",lsink);


void test_tcp_server() {
    int ret;

    std::string serveraddr=LT::JsonMg::GetInstance()->GetVal("../conf/g_Config.json","TcpServer","server","127.0.0.1:8080");

    auto  addr= LT::Address::LookupAnyIPAddress(serveraddr);
    LT_ASSERT(addr);

    auto socket = LT::Socket::CreateTCPSocket();
    LT_ASSERT(socket);

    ret = socket->bind(addr);
    LT_ASSERT(ret);
    
    g_logger->info("bind success");

    ret = socket->listen();
    LT_ASSERT(ret);

    g_logger->info("{} ",socket->toString());

    g_logger->info("listening....");

    while(1) {
        auto client = socket->accept();
        LT_ASSERT(client);
        g_logger->info("new client arrive:{}",client->toString());
        client->send("hello world", strlen("hello world"));
        client->close();
    }
}

int main(int argc, char *argv[]) {
    LT::IOManager iom(2);
    iom.schedule(&test_tcp_server);

    return 0;
}
