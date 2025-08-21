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
        http/http_connection.cpp
        http/http_connection.h
        middlewares/Middleware.cpp
        middlewares/Middleware.h
        middlewares/static_file_middleware.cpp
        middlewares/static_file_middleware.h
        middlewares/router_middleware.cpp
        middlewares/router_middleware.h)
target_include_directories(ToyCppServer PRIVATE server http middlewares)

target_link_libraries(ToyCppServer Threads::Threads ZLIB::ZLIB)
add_subdirectory(tests)
