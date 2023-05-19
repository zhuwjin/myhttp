#include "HttpServer.h"

HttpServer::HttpServer(EventLoop *loop, const InetAddress &listen_addr, const std::string &name, TcpServer::Option option)
    : server_(loop, listen_addr, name, option) {
    server_.setMessageCallback([](const TcpConnectionPtr &conn, Buffer *buf, Timestamp t) {
        std::string data = "Hello world";
        std::string res = "HTTP/1.1 200 OK\r\nContext-Length: ";
        res += std::to_string(data.size()) + "\r\nContext-Type: text/html\r\n\r\n" + data;
        conn->send(res);
    });
    server_.setWriteCompleteCallback([](const TcpConnectionPtr &conn) {
        conn->shutdown();
    });
}