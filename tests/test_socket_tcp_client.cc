#include<LT.h>


std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/sockettcp.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();


static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_socket_tcp_c",lsink);


void test_tcp_client() {
    int ret;

    auto socket = LT::Socket::CreateTCPSocket();
    LT_ASSERT(socket);


    std::string serveraddr=LT::JsonMg::GetInstance()->GetVal("../conf/g_Config.json","TcpServer","server","127.0.0.1:8080");
    auto addr = LT::Address::LookupAnyIPAddress(serveraddr);
    LT_ASSERT(addr);

    ret = socket->connect(addr);
    LT_ASSERT(ret);

    g_logger->info("connect success,perr address:{}",socket->getRemoteAddress()->toString());

    std::string buffer;
    buffer.resize(1024);
    socket->recv(&buffer[0], buffer.size());
    g_logger->info("recv: {}",buffer);
    socket->close();

    return;
}

int main(int argc, char *argv[]) {
    LT::IOManager iom;
    iom.schedule(&test_tcp_client);

    return 0;
}
