#ifndef MYHTTP_HTTPSERVER_H
#define MYHTTP_HTTPSERVER_H

#include "base/noncopyable.h"
#include "net/TcpServer.h"

class HttpRequest;
class HttpResponse;

class HttpServer : noncopyable {
public:
    using HttpCallback = std::function<void(const HttpRequest &, HttpResponse *)>;

    HttpServer(EventLoop *loop,
               const InetAddress &listen_addr,
               const std::string &name,
               TcpServer::Option option = TcpServer::NoReusePort);

    void setHttpCallback(const HttpCallback &cb) {
        http_callback_ = cb;
    }

    void setThreadNum(int num_threads) {
        server_.setThreadNum(num_threads);
    }

    void start() {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receive_time);
    void onRequest(const TcpConnectionPtr &, const HttpRequest &);
    TcpServer server_;
    HttpCallback http_callback_;
};

#endif//MYHTTP_HTTPSERVER_H
