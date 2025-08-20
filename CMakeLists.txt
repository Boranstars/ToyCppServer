cmake_minimum_required(VERSION 4.0)
project(ToyCppServer)

set(CMAKE_CXX_STANDARD 20)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(ToyCppServer server/server.cpp
        http/http_parser.cpp
        http/http_parser.h
        server/buffer.cpp
        server/buffer.h
        http/router.cpp
        http/router.h
        http/response.cpp
        http/response.h
        server/server.h
        http/http_task.cpp
        http/http_task.h)
target_include_directories(ToyCppServer PRIVATE server http)

target_link_libraries(ToyCppServer Threads::Threads ZLIB::ZLIB)
add_subdirectory(tests)
