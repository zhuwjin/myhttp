#include "HttpRequest.h"
#include "HttpResponse.h"
#include "SimpleHttpServer.h"
#include <fstream>

constexpr char hello_html[] = R"(<!DOCTYPE html>
<html lang="zh-cn">
<head>
    <meta charset="UTF-8">
</head>
<body>
    <h1>你好，%s。</h1>
</body>
</html>
)";

void echo(const HttpRequest &req, HttpResponse *res) {
    res->setContentType(req.getHeader("Content-Type"));
    res->setStatusCode(HttpResponse::Ok);
    res->setStatusMessage("Ok");
    res->setBody(req.getBody());
}

void hello(const HttpRequest &req, HttpResponse *res) {
    char buf[65535] = {};
    if (auto name = req.getQuery("name"); name) {
        sprintf(buf, hello_html, name.value().c_str());
    } else {
        sprintf(buf, hello_html, "世界");
    }
    res->setStatusCode(HttpResponse::Ok);
    res->setStatusMessage("Ok");
    res->setContentType("text/html;charset=utf-8");
    res->setBody(buf);
}

void index_(const HttpRequest &req, HttpResponse *res) {
    std::ifstream f("./index.html");
    std::string body;
    std::string line;
    while (std::getline(f, line)) {
        body += line + "\n";
    }
    res->setStatusCode(HttpResponse::Ok);
    res->setStatusMessage("Ok");
    res->setContentType("text/html;charset=utf-8");
    res->setBody(body);
}


int main() {
    SimpleHttpServer server("127.0.0.1", 8080, "multi");
    server.setThreadNum(7);
    server.addRouter("/echo", echo);
    server.addRouter("/hello", hello);
    server.addRouter("/", index_);
    server.start();
    return 0;
}
