//
// Created by 杨博元 on 2025/8/21.
//
#include <iostream>
#include "middleware.h"
#include "static_file_middleware.h"
#include "router_middleware.h"
#include "response.h"
#include "http_parser.h"
int main() {
    Request request;
    Response response;
    std::shared_ptr<Router> router = std::make_shared<Router>();
    router->get("/ys",[](Request& req, Response& res) {
        res.status = 302;
        res.headers["Location"] = "https://www.yuanshen.com/";

    });
    request.method = "GET";
    request.uri = "/tmp";
    auto static_file_middleware = std::make_shared<StaticFileMiddleware>("/Users/boran/CLionProjects/ToyCppServer");
    auto router_middleware = std::make_shared<RouterMiddleware>(router);
    std::shared_ptr<Middleware> middleware = static_file_middleware;
    static_file_middleware->setNext(router_middleware);
    middleware->process(request, response);
    std::cout << response.body << std::endl;
    for (const auto& [k,v]: response.headers) {
        std::cout << k << ": " << v << std::endl;
    }

}