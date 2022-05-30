#include "LT.h"
#include <iostream>

static LT::Logger::ptr g_logger = LT_LOG_ROOT();

void test_pool() {
    LT::http::HttpConnectionPool::ptr pool(new LT::http::HttpConnectionPool(
        "www.midlane.top", "", 80, 10, 1000 * 30, 5));

    LT::IOManager::GetThis()->addTimer(
        1000, [pool]() {
            auto r = pool->doGet("/", 300);
            std::cout << r->toString() << std::endl;
        },
        true);
}

void run() {
    LT::Address::ptr addr = LT::Address::LookupAnyIPAddress("www.midlane.top:80");
    if (!addr) {
        LT_LOG_INFO(g_logger) << "get addr error";
        return;
    }

    LT::Socket::ptr sock = LT::Socket::CreateTCP(addr);
    bool rt                 = sock->connect(addr);
    if (!rt) {
        LT_LOG_INFO(g_logger) << "connect " << *addr << " failed";
        return;
    }

    LT::http::HttpConnection::ptr conn(new LT::http::HttpConnection(sock));
    LT::http::HttpRequest::ptr req(new LT::http::HttpRequest);
    req->setPath("/");
    req->setHeader("host", "www.midlane.top");
    // 小bug，如果设置了keep-alive，那么要在使用前先调用一次init
    req->setHeader("connection", "keep-alive");
    req->init();
    std::cout << "req:" << std::endl
              << *req << std::endl;

    conn->sendRequest(req);
    auto rsp = conn->recvResponse();

    if (!rsp) {
        LT_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    std::cout << "rsp:" << std::endl
              << *rsp << std::endl;

    std::cout << "=========================" << std::endl;

    auto r = LT::http::HttpConnection::DoGet("http://www.midlane.top/wiki/", 300);
    std::cout << "result=" << r->result
              << " error=" << r->error
              << " rsp=" << (r->response ? r->response->toString() : "")
              << std::endl;

    std::cout << "=========================" << std::endl;
    test_pool();
}

int main(int argc, char **argv) {
    LT::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
