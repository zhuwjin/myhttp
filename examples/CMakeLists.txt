add_executable(http_echo HttpEchoServer.cc)
target_link_libraries(http_echo myhttp)

add_executable(http_hello HttpHelloServer.cc)
target_link_libraries(http_hello myhttp)

add_executable(http_multi HttpMultiServer.cc)
target_link_libraries(http_multi myhttp)