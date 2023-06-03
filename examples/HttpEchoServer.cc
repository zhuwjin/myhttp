#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServer.h"

class HttpEchoServer {
public:
    HttpEchoServer(EventLoop *loop,
                   const InetAddress &listen_addr)
        : server_(loop, listen_addr, "http_echo_server") {
        server_.setHttpCallback([](const HttpRequest &req, HttpResponse *res) {
            const std::string &body = req.getBody();
            res->setStatusCode(HttpResponse::Ok);
            res->setStatusMessage("Ok");
            res->setContentType("application/json;charset=UTF-8");
            res->setBody(body);
        });
    }
    void start() {
        server_.start();
    }

private:
    HttpServer server_;
};

int main() {
    EventLoop loop;
    InetAddress address(8080);
    HttpEchoServer echo_server(&loop, address);
    echo_server.start();
    loop.loop();
    return 0;
}