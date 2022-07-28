#include <LT.h>

static LT::Logger::ptr g_logger = LT_LOG_ROOT();

void test_tcp_server() {
    int ret;

    auto addr = LT::Address::LookupAnyIPAddress("0.0.0.0:12345");
    LT_ASSERT(addr);

    auto socket = LT::Socket::CreateTCPSocket();
    LT_ASSERT(socket);

    ret = socket->bind(addr);
    LT_ASSERT(ret);
    
    LT_LOG_INFO(g_logger) << "bind success";

    ret = socket->listen();
    LT_ASSERT(ret);

    LT_LOG_INFO(g_logger) << socket->toString() ;
    LT_LOG_INFO(g_logger) << "listening...";

    while(1) {
        auto client = socket->accept();
        LT_ASSERT(client);
        LT_LOG_INFO(g_logger) << "new client: " << client->toString();
        client->send("hello world", strlen("hello world"));
        client->close();
    }
}

int main(int argc, char *argv[]) {
    LT::EnvMgr::GetInstance()->init(argc, argv);
    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

    LT::IOManager iom(2);
    iom.schedule(&test_tcp_server);

    return 0;
}
