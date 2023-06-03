#ifndef MYHTTP_SIMPLEHTTPSERVER_H
#define MYHTTP_SIMPLEHTTPSERVER_H
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServer.h"
#include "base/noncopyable.h"
#include <map>

class SimpleHttpServer : noncopyable {
public:
    SimpleHttpServer(InetAddress addr, const std::string &server_name) : loop_(), server_(&loop_, addr, server_name) {
        server_.setHttpCallback([this](const HttpRequest &req, HttpResponse *res) {
            const std::string &_path = req.path();
            auto _r = routers_.find(_path);
            if (_r != routers_.end()) {
                _r->second(req, res);
            } else {
                res->setStatusCode(HttpResponse::NotFound);
                res->setStatusMessage("NotFound");
            }
        });
    }

    SimpleHttpServer(const std::string &ip, uint16_t port, const std::string &server_name) : SimpleHttpServer(InetAddress(port, ip), server_name) {}


    void addRouter(std::string &&router, const std::function<void(const HttpRequest &req, HttpResponse *res)> &cb) {
        routers_[router] = cb;
    }


    void start() {
        server_.start();
        loop_.loop();
    }

private:
    std::map<std::string, std::function<void(const HttpRequest &req, HttpResponse *res)>> routers_;
    EventLoop loop_;
    HttpServer server_;
};

#endif//MYHTTP_SIMPLEHTTPSERVER_H
