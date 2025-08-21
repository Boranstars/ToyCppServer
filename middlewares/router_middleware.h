//
// Created by 杨博元 on 2025/8/21.
//

#ifndef TOYCPPSERVER_ROUTER_MIDDLEWARE_H
#define TOYCPPSERVER_ROUTER_MIDDLEWARE_H
#include "middleware.h"
#include "router.h"

// 路由类的中间件包装类
class RouterMiddleware :public Middleware{
public:
    explicit RouterMiddleware(std::shared_ptr<Router> router);
    bool process(Request &req, Response &res) override;

private:
    std::shared_ptr<Router> router_;
};


#endif //TOYCPPSERVER_ROUTER_MIDDLEWARE_H