#include "LT.h"

static LT::Logger::ptr g_logger = LT_LOG_ROOT();

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
    LT_LOG_INFO(g_logger) << "test_ifaces: " << family2str(family);

    std::multimap<std::string, std::pair<LT::Address::ptr, uint32_t>> results;
    bool v = LT::Address::GetInterfaceAddresses(results, family);
    if (!v) {
        LT_LOG_ERROR(g_logger) << "GetInterfaceAddresses fail";
        return;
    }
    for (auto &i : results) {
        LT_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString() << " - "
                                 << i.second.second;
    }
    
    LT_LOG_INFO(g_logger) << "\n";
}

/**
 * @brief 查询指定网卡
 * @param[in] iface 网卡名称
 * @param[in] family 地址类型
 */
void test_iface(const char *iface, int family) {
    LT_LOG_INFO(g_logger) << "test_iface: " << iface << ", " << family2str(family);

    std::vector<std::pair<LT::Address::ptr, uint32_t>> result;
    bool v = LT::Address::GetInterfaceAddresses(result, iface, family);
    if(!v) {
        LT_LOG_ERROR(g_logger) << "GetInterfaceAddresses fail";
        return;
    }
    for(auto &i : result) {
        LT_LOG_INFO(g_logger) << i.first->toString() << " - " << i.second;
    }

    LT_LOG_INFO(g_logger) << "\n";
}

/**
 * @brief 测试网络地址解析
 * @param[] host 网络地址描述，包括字符串形式的域名/主机名或是数字格式的IP地址，支持端口和服务名解析
 * @note 这里没有区分不同的套接字类型，所以会有重复值
 */
void test_lookup(const char *host) {
    LT_LOG_INFO(g_logger) << "test_lookup: " << host;

    LT_LOG_INFO(g_logger) <<"Lookup:";
    std::vector<LT::Address::ptr> results;
    bool v = LT::Address::Lookup(results, host, AF_INET);
    if(!v) {
        LT_LOG_ERROR(g_logger) << "Lookup fail";
        return;
    }
    for(auto &i : results) {
        LT_LOG_INFO(g_logger) << i->toString();
    }
    
    LT_LOG_INFO(g_logger) <<"LookupAny:";
    auto addr2 = LT::Address::LookupAny(host);
    LT_LOG_INFO(g_logger) << addr2->toString();

    LT_LOG_INFO(g_logger) <<"LookupAnyIPAddress:";
    auto addr1 = LT::Address::LookupAnyIPAddress(host);
    LT_LOG_INFO(g_logger) << addr1->toString();

    LT_LOG_INFO(g_logger) << "\n";
}

/**
 * @brief IPv4地址类测试
 */
void test_ipv4() {
    LT_LOG_INFO(g_logger) << "test_ipv4";

    auto addr = LT::IPAddress::Create("192.168.1.120");
    if (!addr) {
        LT_LOG_ERROR(g_logger) << "IPAddress::Create error";
        return;
    }
    LT_LOG_INFO(g_logger) << "addr: " << addr->toString();
    LT_LOG_INFO(g_logger) << "family: " << family2str(addr->getFamily());  
    LT_LOG_INFO(g_logger) << "port: " << addr->getPort();  
    LT_LOG_INFO(g_logger) << "addr length: " << addr->getAddrLen(); 

    LT_LOG_INFO(g_logger) << "broadcast addr: " << addr->broadcastAddress(24)->toString();
    LT_LOG_INFO(g_logger) << "network addr: " << addr->networkAddress(24)->toString();
    LT_LOG_INFO(g_logger) << "subnet mask addr: " << addr->subnetMask(24)->toString();

    LT_LOG_INFO(g_logger) << "\n";
}

/**
 * @brief IPv6地址类测试
 */
void test_ipv6() {
    LT_LOG_INFO(g_logger) << "test_ipv6";

    auto addr = LT::IPAddress::Create("fe80::215:5dff:fe88:d8a");
    if (!addr) {
        LT_LOG_ERROR(g_logger) << "IPAddress::Create error";
        return;
    }
    LT_LOG_INFO(g_logger) << "addr: " << addr->toString();
    LT_LOG_INFO(g_logger) << "family: " << family2str(addr->getFamily());  
    LT_LOG_INFO(g_logger) << "port: " << addr->getPort();  
    LT_LOG_INFO(g_logger) << "addr length: " << addr->getAddrLen(); 

    LT_LOG_INFO(g_logger) << "broadcast addr: " << addr->broadcastAddress(64)->toString();
    LT_LOG_INFO(g_logger) << "network addr: " << addr->networkAddress(64)->toString();
    LT_LOG_INFO(g_logger) << "subnet mask addr: " << addr->subnetMask(64)->toString();
    LT_LOG_INFO(g_logger) << "\n";
}

/**
 * @brief Unix套接字解析
 */
void test_unix() {
    LT_LOG_INFO(g_logger) << "test_unix";
    
    auto addr = LT::UnixAddress("/tmp/test_unix.sock");
    LT_LOG_INFO(g_logger) << "addr: " << addr.toString();
    LT_LOG_INFO(g_logger) << "family: " << family2str(addr.getFamily());  
    LT_LOG_INFO(g_logger) << "path: " << addr.getPath(); 
    LT_LOG_INFO(g_logger) << "addr length: " << addr.getAddrLen(); 

    LT_LOG_INFO(g_logger) << "\n";
}

int main(int argc, char *argv[]) {
    LT::EnvMgr::GetInstance()->init(argc, argv);
    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

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
