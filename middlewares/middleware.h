//
// Created by 杨博元 on 2025/8/20.
//

#ifndef TOYCPPSERVER_MIDDLEWARE_H
#define TOYCPPSERVER_MIDDLEWARE_H
#include <memory>
#include <utility>


struct Request;
struct Response;

class Middleware {
public:
    virtual ~Middleware() = default;

    // 处理请求，如果返回true表示已处理请求，不再传递给下一个中间件
    virtual bool process(Request& req, Response& res) = 0;

    // 设置下一个中间件
    virtual void setNext(std::shared_ptr<Middleware> next) {
        this->next_ = std::move(next);
    }

    // 将请求传递给下一个中间件
    bool next(Request& req, Response& res) {
        if (next_) {
            return next_->process(req, res);
        }
        return false;
    };
protected:
    std::shared_ptr<Middleware> next_;
};


#endif //TOYCPPSERVER_MIDDLEWARE_H