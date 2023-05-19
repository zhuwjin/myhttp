cmake_minimum_required(VERSION 3.25)
project(myhttp)

set(CMAKE_CXX_STANDARD 17)

include_directories(third/mymuduo/include)
include_directories(include)

add_subdirectory(third/mymuduo)

add_library(myhttp library.cpp HttpServer.cc)

target_link_libraries(myhttp mymuduo)

add_executable(test2 test.cc)
target_link_libraries(test2 myhttp)
