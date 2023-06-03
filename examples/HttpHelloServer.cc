#include "HttpRequest.h"
#include "HttpResponse.h"
#include "SimpleHttpServer.h"


constexpr char html[] = R"(<!DOCTYPE html>
<html lang="zh-cn">
<head>
    <meta charset="UTF-8">
</head>
<body>
    <h1>你好，%s。</h1>
</body>
</html>
)";

void hello(const HttpRequest &req, HttpResponse *res) {
    char buf[65535] = {};
    if (auto name = req.getQuery("name"); name) {
        sprintf(buf, html, name.value().c_str());
    } else {
        sprintf(buf, html, "世界");
    }
    res->setStatusCode(HttpResponse::Ok);
    res->setStatusMessage("Ok");
    res->setContentType("text/html;charset=utf-8");
    res->setBody(buf);
}

int main() {
    SimpleHttpServer hello_server("127.0.0.1", 8080, "hello");
    hello_server.addRouter("/hello", [](const HttpRequest &req, HttpResponse *res) { hello(req, res); });
    hello_server.start();
    return 0;
}
