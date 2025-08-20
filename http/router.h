//
// Created by 杨博元 on 2025/8/19.
//

#ifndef TOYCPPSERVER_ROUTER_H
#define TOYCPPSERVER_ROUTER_H
#include <functional>
#include <map>
#include <regex>

#include "http_parser.h"
#include "response.h"
using RouterHandler = std::function<void(Request&, Response&)>;
using RouterMap = std::map<std::string, RouterHandler>;
using ParamRegPairs = std::pair<std::regex,std::vector<std::string>>;
struct DynamicRoute {
    RouterHandler handler;
    std::vector<std::string> paramNames;
    std::regex pattern;
};
struct HandlerReturn {
    RouterHandler handler;
    std::unordered_map<std::string, std::string> params;
    bool success = false;
};

class Router {
    public:
    Router();
    ~Router() = default;

    void registerRoute(const std::string& path,const std::string& method, RouterHandler handler);
    HandlerReturn getHandler(const std::string& method, const std::string& path);

    static const RouterHandler methodNotAllowedHandler_;

    static const RouterHandler defaultHandler_ ;
    static const RouterHandler notFoundHandler_;
    void get(const std::string& path,RouterHandler handler);

private:

    /**
     * {path: {method: handler}}
     */
    std::unordered_map<std::string, RouterMap> routers_;
    // {method : [DynamicRoute]}
    std::unordered_map<std::string, std::vector<DynamicRoute>> dynamicRouters_;

    ParamRegPairs patternToReg(const std::string& routePattern);
};


#endif //TOYCPPSERVER_ROUTER_H