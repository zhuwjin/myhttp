#include "HttpRequest.h"
#include "HttpResponse.h"
#include "SimpleHttpServer.h"


int main() {
    SimpleHttpServer echo("127.0.0.1", 8080, "echo");
    echo.setThreadNum(7);
    echo.addRouter("/", [](const HttpRequest &req, HttpResponse *res) {
        res->setContentType(req.getHeader("Content-Type"));
        res->setStatusCode(HttpResponse::Ok);
        res->setStatusMessage("Ok");
        res->setBody(req.getBody());
    });
    echo.start();
    return 0;
}