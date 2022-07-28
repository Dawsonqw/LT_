#include<LT.h>

static LT::Logger::ptr g_logger = LT_LOG_ROOT();

void test_tcp_client() {
    int ret;

    auto socket = LT::Socket::CreateTCPSocket();
    LT_ASSERT(socket);

    auto addr = LT::Address::LookupAnyIPAddress("0.0.0.0:12345");
    LT_ASSERT(addr);

    ret = socket->connect(addr);
    LT_ASSERT(ret);

    LT_LOG_INFO(g_logger) << "connect success, peer address: " << socket->getRemoteAddress()->toString();

    std::string buffer;
    buffer.resize(1024);
    socket->recv(&buffer[0], buffer.size());
    LT_LOG_INFO(g_logger) << "recv: " << buffer;
    socket->close();

    return;
}

int main(int argc, char *argv[]) {
    LT::EnvMgr::GetInstance()->init(argc, argv);
    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

    LT::IOManager iom;
    iom.schedule(&test_tcp_client);

    return 0;
}
