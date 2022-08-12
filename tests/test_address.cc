#include "LT.h"

std::shared_ptr<spdlog::sinks::basic_file_sink_mt>lg_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/address.txt");
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> lc_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

static lsinks lsink{lg_sink,lc_sink};
static auto g_logger = std::make_shared<spdlog::logger>("test_address",lsink);



const char *family2str(int family) {
    switch (family) {
#define XX(name) case (name): return #name;
        XX(AF_INET)
        XX(AF_INET6)
        XX(AF_UNIX)
        XX(AF_UNSPEC)
#undef XX
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief 查询所有网卡
 * @param[in] family 地址类型
 */
void test_ifaces(int family) {
    g_logger->info("test frace:{}", family2str(family));

    std::multimap<std::string, std::pair<LT::Address::ptr, uint32_t>> results;
    bool v = LT::Address::GetInterfaceAddresses(results, family);
    if (!v) {
        g_logger->info("GetInterfaceAddresses fail");
        return;
    }
    for (auto &i : results) {
        g_logger->info("i.fist{}  -i.second.second{}",i.first,i.second.second);
    }
}

/**
 * @brief 查询指定网卡
 * @param[in] iface 网卡名称
 * @param[in] family 地址类型
 */
void test_iface(const char *iface, int family) {
    g_logger->info("test frace:{}", family2str(family));
    std::vector<std::pair<LT::Address::ptr, uint32_t>> result;
    bool v = LT::Address::GetInterfaceAddresses(result, iface, family);
    if(!v) {
        g_logger->error("GetInterfaceAddresses fail");
        return;
    }
    for(auto &i : result) {
        g_logger->info("i.fist {} - i.second {}",i.first->toString(),i.second);
    }
}

/**
 * @brief 测试网络地址解析
 * @param[] host 网络地址描述，包括字符串形式的域名/主机名或是数字格式的IP地址，支持端口和服务名解析
 * @note 这里没有区分不同的套接字类型，所以会有重复值
 */
void test_lookup(const char *host) {
    g_logger->info("loop up");
    std::vector<LT::Address::ptr> results;
    bool v = LT::Address::Lookup(results, host, AF_INET);
    if(!v) {
        g_logger->error("look up failed");
        return;
    }
    for(auto &i : results) {
        g_logger->error("{}",i->toString());
    }

    g_logger->info("lookup any");
    auto addr2 = LT::Address::LookupAny(host);
    g_logger->info("{}",addr2->toString());

    g_logger->info("LookupAnyIPAddress:");
    auto addr1 = LT::Address::LookupAnyIPAddress(host);
    g_logger->info("{}",addr1->toString());
}

/**
 * @brief IPv4地址类测试
 */
void test_ipv4() {
    g_logger->info("ipv4");

    auto addr = LT::IPAddress::Create("192.168.1.120");
    if (!addr) {
        g_logger->error("IPAddress::Create error");
        return;
    }
    g_logger->info("addr {} family {} port {} addr len {} baddr {} naddr {} smask addr{}",
                   addr->toString(), family2str(addr->getFamily()),addr->getPort(),addr->getAddrLen(),
                   addr->broadcastAddress(24)->toString(),addr->networkAddress(24)->toString(),
                   addr->subnetMask(24)->toString());
}

/**
 * @brief IPv6地址类测试
 */
void test_ipv6() {
    g_logger->info("ipv6");

    auto addr = LT::IPAddress::Create("fe80::215:5dff:fe88:d8a");
    if (!addr) {
        g_logger->error("IPAddress::Create error");
        return;
    }
    g_logger->info("addr {} family {} port {} addr len {} baddr {} naddr {} smask addr{}",
                   addr->toString(), family2str(addr->getFamily()),addr->getPort(),addr->getAddrLen(),
                   addr->broadcastAddress(24)->toString(),addr->networkAddress(24)->toString(),
                   addr->subnetMask(24)->toString());
}

/**
 * @brief Unix套接字解析
 */
void test_unix() {
    g_logger->info("unix");

    auto addr = LT::UnixAddress("/tmp/test_unix.sock");
    g_logger->info("addr {} family {} path {} addrlen {}",addr.toString(), family2str(addr.getFamily()),
                   addr.getPath(),addr.getAddrLen());
}

int main(int argc, char *argv[]) {
    // 获取本机所有网卡的IPv4地址和IPv6地址以及掩码长度
    test_ifaces(AF_INET);
    test_ifaces(AF_INET6);

    // 获取本机eth0网卡的IPv4地址和IPv6地址以及掩码长度
    test_iface("eth0", AF_INET);
    test_iface("eth0", AF_INET6);

    // ip域名服务解析
    test_lookup("127.0.0.1");
    test_lookup("127.0.0.1:80");
    test_lookup("127.0.0.1:http");
    test_lookup("127.0.0.1:ftp");
    test_lookup("localhost");
    test_lookup("localhost:80");
    test_lookup("www.baidu.com");
    test_lookup("www.baidu.com:80");
    test_lookup("www.baidu.com:http");

    // IPv4地址类测试
    test_ipv4();

    // IPv6地址类测试
    test_ipv6();

    // Unix套接字地址类测试
    test_unix();

    return 0;
}
