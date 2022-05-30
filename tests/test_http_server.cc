#include "LT.h"


static LT::Logger::ptr g_logger = LT_LOG_NAME("system");
#define XX(...) #__VA_ARGS__

LT::IOManager::ptr worker;

void run() {
    g_logger->setLevel(LT::LogLevel::NOTSET);
    LT_LOG_INFO(g_logger)<<"start";
    LT_LOG_INFO(g_logger)<<"start";
    LT_LOG_NOTICE(g_logger)<<"enter";
    //LT::http::HttpServer::ptr server(new LT::http::HttpServer(true, worker.get(), LT::IOManager::GetThis()));
    LT::http::HttpServer::ptr server(new LT::http::HttpServer(true));
    LT::Address::ptr addr = LT::Address::LookupAnyIPAddress("0.0.0.0:8888");
    while (!server->bind(addr)) {
        sleep(1);
    }
    auto sd = server->getServletDispatch();
    sd->addServlet("/LT/xx", [](LT::http::HttpRequest::ptr req, LT::http::HttpResponse::ptr rsp, LT::http::HttpSession::ptr session) {
        rsp->setBody(req->toString());
        return 0;
    });

    sd->addGlobServlet("/LT/*", [](LT::http::HttpRequest::ptr req, LT::http::HttpResponse::ptr rsp, LT::http::HttpSession::ptr session) {
        rsp->setBody("Glob:\r\n" + req->toString());
        return 0;
    });

    sd->addGlobServlet("/LTx/*", [](LT::http::HttpRequest::ptr req, LT::http::HttpResponse::ptr rsp, LT::http::HttpSession::ptr session) {
        rsp->setBody(XX(<html>
                                <head><title> 404 Not Found</ title></ head>
                                <body>
                                <center><h1> 404 Not Found</ h1></ center>
                                <hr><center>
                                    nginx /
                                1.16.0 <
                            / center >
                            </ body>
                            </ html> < !--a padding to disable MSIE and
                        Chrome friendly error page-- >
                            < !--a padding to disable MSIE and
                        Chrome friendly error page-- >
                            < !--a padding to disable MSIE and
                        Chrome friendly error page-- >
                            < !--a padding to disable MSIE and
                        Chrome friendly error page-- >
                            < !--a padding to disable MSIE and
                        Chrome friendly error page-- >
                            < !--a padding to disable MSIE and
                        Chrome friendly error page-- >));
        return 0;
    });

    server->start();
}

int main(int argc, char **argv) {
    LT::EnvMgr::GetInstance()->init(argc, argv);
    LT::Config::LoadFromConfDir(LT::EnvMgr::GetInstance()->getConfigPath());

    LT::LogFormatter::ptr logFormatter(new LT::LogFormatter);
    LT::LogAppender::ptr appender(new LT::StdoutLogAppender);
    appender->setFormatter(logFormatter);
    g_logger->addAppender(appender);
//    g_logger->setLevel(LT::LogLevel::INFO);

    LT::IOManager iom(1, true, "main");
    worker.reset(new LT::IOManager(3, false, "worker"));
    iom.schedule(run);
    return 0;
}
