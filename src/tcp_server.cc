#include <string.h>
#include <sstream>
#include "tcp_server.h"
#include "log.h"

namespace LT {

static lsinks lsink{g_sink,c_sink};
static auto g_logger = std::make_shared<spdlog::logger>("tcpserver",lsink);


TcpServer::TcpServer(LT::IOManager* io_worker,
                    LT::IOManager* accept_worker)
    :m_ioWorker(io_worker)
    ,m_acceptWorker(accept_worker)
    ,m_recvTimeout(60*2*1000)
    ,m_name("LT/1.0.0")
    ,m_type("tcp")
    ,m_isStop(true) {
}

TcpServer::~TcpServer() {
    for(auto& i : m_socks) {
        i->close();
    }
    m_socks.clear();
}

bool TcpServer::bind(LT::Address::ptr addr) {
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails);
}

bool TcpServer::bind(const std::vector<Address::ptr>& addrs
                        ,std::vector<Address::ptr>& fails ) {
    for(auto& addr : addrs) {
        Socket::ptr sock = Socket::CreateTCP(addr);
        if(!sock->bind(addr)) {
			g_logger->error("bind fail errno={} errstr={} addr=[{}]",errno,strerror(errno),addr->toString());
            fails.push_back(addr);
            continue;
        }
        if(!sock->listen()) {
			g_logger->error("listen fail errno={} errstr={} addr=[{}]", errno, strerror(errno), addr->toString());
            fails.push_back(addr);
            continue;
        }
        m_socks.push_back(sock);
    }

    //存在绑定失败的就直接返回 对于成功的不做处理
    if(!fails.empty()) {
        m_socks.clear();
        return false;
    }

    for(auto& i : m_socks) {
		g_logger->info("type={},name={},server bind success",m_type,m_name);
    }
    return true;
}

void TcpServer::startAccept(Socket::ptr sock) {
    while(!m_isStop) {
        ///封装后api逻辑一致 返回的是接收到的客户端信息
        Socket::ptr client = sock->accept();
        if(client) {
            //设置心跳 添加到调度器中等待调度 回调时handleclient 可以进行定制
            client->setRecvTimeout(m_recvTimeout);
            //对客户端socket进行处理
            m_ioWorker->schedule(std::bind(&TcpServer::handleClient,
                        shared_from_this(), client));
        } else {
			g_logger->error("accept errno ={} errstr={}", errno, strerror(errno));
        }
    }
}

bool TcpServer::start() {
    if(!m_isStop) {
        return true;
    }
    m_isStop = false;
    ///开启接收任务处理
    for(auto& sock : m_socks) {
        ///服务端口对于所有监听socket的处理
        m_acceptWorker->schedule(std::bind(&TcpServer::startAccept,
                    shared_from_this(), sock));
    }
    return true;
}

//添加任务进行回收？
void TcpServer::stop() {
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self]() {
        for(auto& sock : m_socks) {
            sock->cancelAll();
            sock->close();
        }
        m_socks.clear();
    });
}

///作为虚函数子类进行重写
void TcpServer::handleClient(Socket::ptr client) {
	//g_logger->info("handleClient {}",*client);
}

std::string TcpServer::toString(const std::string& prefix) {
    std::stringstream ss;
    ss << prefix << "[type=" << m_type
       << " name=" << m_name
       << " io_worker=" << (m_ioWorker ? m_ioWorker->getName() : "")
       << " accept=" << (m_acceptWorker ? m_acceptWorker->getName() : "")
       << " recv_timeout=" << m_recvTimeout << "]" << std::endl;
    std::string pfx = prefix.empty() ? "    " : prefix;
    for(auto& i : m_socks) {
        ss << pfx << pfx << *i << std::endl;
    }
    return ss.str();
}

}
