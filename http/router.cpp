//
// Created by 杨博元 on 2025/8/19.
//

#include "router.h"

#include <iostream>
#include <utility>

Router::Router() {

}

void Router::registerRoute(const std::string &path, const std::string &method, RouterHandler handler) {
    if (path.find(':') != std::string::npos) {
        // 为动态路由格式，如/echo/:content
        auto [regexPattern, paramNames] = patternToReg(path);

        // DynamicRoute dynamicRoute {handler,paramNames,regexPattern};
        dynamicRouters_[method].emplace_back(
            std::move(handler),
            std::move(paramNames),
            std::move(regexPattern)
        );
    } else {
        // 静态路由
        routers_[path][method] = std::move(handler);
    }


}

HandlerReturn Router::getHandler(const std::string &method, const std::string &path) {

    HandlerReturn retVal;
    // 尝试静态路由匹配
    auto pathIt = routers_.find(path);
    if (pathIt != routers_.end()) {
        const RouterMap &routers = pathIt->second;
        auto methodIt = routers.find(method);
        if (methodIt != routers.end()) {
            retVal.handler = methodIt->second;
            retVal.success = true;
            return retVal;
        }
        retVal.handler = defaultHandler_;
        retVal.success = true;
        return retVal; //方法不匹配
    }
    // 动态路由匹配
    auto methodIt = dynamicRouters_.find(method);
    if (methodIt != dynamicRouters_.end()) {
        const std::vector<DynamicRoute> &dynamicRouters = methodIt->second;
        for (const auto &router : dynamicRouters) {
            std::smatch match;
            if (std::regex_match(path, match, router.pattern)) {
                for (size_t i = 0; i < router.paramNames.size(); ++i) {
                    if (i+1 < match.size()) {
                        retVal.params[router.paramNames[i]] = match[i+1].str();
                    }
                }
                retVal.success = true;
                retVal.handler = router.handler;
                return retVal;
            }

        }
    }
    // 匹配失败：
    retVal.handler = defaultHandler_;
    retVal.success = true;
    return retVal;


}

void Router::get(const std::string &path, RouterHandler handler) {
    registerRoute(path, "GET", std::move(handler));
}

ParamRegPairs Router::patternToReg(const std::string &routePattern) {
    std::vector<std::string> paramNames;
    std::string regexPattern = "^/";

    std::istringstream istream(routePattern);
    std::string segment;
    bool first = true;
    // routePattern可能是这样的形式：/userid/:id/postid/:postid，/echo/:content
    while (std::getline(istream, segment,'/')) {
        if (segment.empty()) {
            continue; // 空段
        }
        if (segment.at(0) == ':') {
            // 参数段，如 :content
            paramNames.emplace_back(segment.substr(1));
            regexPattern += "/([^/]+)";

        } else {
            // 静态段，如echo
            if (first) {
                // 第一个段不需要前置斜杠（因为 regexPattern 以 ^ 开头）
                regexPattern += segment;
                first = false;
            } else {
                regexPattern += "/" + segment;
            }

        }


    }
    regexPattern += "$";  // 正则表达式以$结束

    // 编译正则表达式
    try {
        std::regex compiledRegex(regexPattern);
        return {compiledRegex, paramNames};
    } catch (const std::regex_error& e) {
        // 处理正则表达式编译错误
        std::cerr << "Regex error: " << e.what() << " for pattern: " << regexPattern << std::endl;
        throw;  // 重新抛出异常或返回默认值
    }
}


void defaultHandler(const Request &req, Response &res) {
    res.status = 501;
    res.headers["Content-Type"] = "text/html; charset=utf-8";

    res.headers["Connection"] = "close";
    res.body = "<!DOCTYPE html>\
                <html lang=\"en\">\
                <head>\
                    <meta charset=\"UTF-8\">\
                    <title>501 Not Implemented</title>\
                </head>\
                <body>\
                <center><h1>501 Not Implemented</h1></center>\
                <hr>\
                <center><p>Toy Cpp Server</p></center>\
                </body>\
                </html>";
    res.headers["Content-Length"] = std::to_string(res.body.size());
}
const RouterHandler Router::defaultHandler_ = defaultHandler;
// 404 Not Found
const RouterHandler Router::notFoundHandler_ = [](const Request&, Response& res) {
    res.status = 404;
    res.body = "<h1>404 Not Found</h1>";
    res.headers["Content-Length"] = std::to_string(res.body.size());
};

// 405 Method Not Allowed


