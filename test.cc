#include "HttpServer.h"

int main() {
    EventLoop loop;
    InetAddress address(8080);
    HttpServer server(&loop, address, "http-server");
    server.start();
    loop.loop();
    return 0;
}
