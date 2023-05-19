#ifndef MYHTTP_HTTPSERVER_H
#define MYHTTP_HTTPSERVER_H

#include "base/noncopyable.h"
#include "net/TcpServer.h"

class HttpServer : noncopyable {
public:
    HttpServer(EventLoop *loop,
               const InetAddress &listen_addr,
               const std::string &name,
               TcpServer::Option option = TcpServer::NoReusePort);

    void start() {
        server_.start();
    }

private:
    TcpServer server_;
};

#endif//MYHTTP_HTTPSERVER_H
