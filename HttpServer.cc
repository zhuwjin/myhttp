#include "HttpServer.h"

#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "base/Logging.h"

void defaultHttpCallback(const HttpRequest &, HttpResponse *resp) {
    resp->setStatusCode(HttpResponse::NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop *loop, const InetAddress &listen_addr, const std::string &name, TcpServer::Option option)
    : server_(loop, listen_addr, name, option),
      http_callback_(defaultHttpCallback) {

    server_.setConnectionCallback([this](const TcpConnectionPtr &conn) {
        onConnection(conn);
    });

    server_.setMessageCallback([this](const TcpConnectionPtr &conn, Buffer *buf, Timestamp t) {
        onMessage(conn, buf, t);
    });
}

void HttpServer::onConnection(const TcpConnectionPtr &conn) {
    if (conn->connect()) {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receive_time) {
    auto *context = std::any_cast<HttpContext>(conn->getMutableContext());

    if (!context->parseRequest(buf, receive_time)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n");
        conn->shutdown();
    }

    if (context->gotAll()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &req) {
    const std::string &connection = req.getHeader("Connection");
    bool close = connection == "close" || (req.getVersion() == HttpRequest::Http10 && connection != "Keep-Alive");
    HttpResponse response(close);
    http_callback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(buf.retrieveAllAsString());
    if (response.closeConnection()) {
        conn->shutdown();
    }
}