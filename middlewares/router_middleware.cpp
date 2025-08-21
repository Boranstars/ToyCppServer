//
// Created by 杨博元 on 2025/8/21.
//

#include "router_middleware.h"

RouterMiddleware::RouterMiddleware(std::shared_ptr<Router> router):router_(std::move(router)) {
}

bool RouterMiddleware::process(Request &req, Response &res) {

    HandlerReturn result = router_->getHandler(req.method, req.uri);
    if (result.success) {
        if (!result.params.empty()) {
            req.params.insert(result.params.begin(), result.params.end());
        }
        // 执行
        result.handler(req,res);
        return true; // 这里就会结束中间件责任链。但是会让后续压缩无法执行。
        // 方案1：修改中间件接口（推荐）
        // 修改中间件接口，使其能够支持"预处理"和"后处理"：
        //方案3：分阶段中间件链（最灵活）
        // 创建两个中间件链：预处理链和响应处理链：
    }
    return false;
}
