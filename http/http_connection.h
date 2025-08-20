//
// Created by 杨博元 on 2025/8/20.
//

#ifndef TOYCPPSERVER_HTTPCONNECTION_H
#define TOYCPPSERVER_HTTPCONNECTION_H
#include "router.h"
#include "http_parser.h"
#include "response.h"
#include "buffer.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
class HttpConnection {
public:
    HttpConnection(int client_fd,const Router & router);
    HttpConnection(const HttpConnection &) = delete;
    HttpConnection &operator=(const HttpConnection &) = delete;
    HttpConnection& operator=(HttpConnection&&) = delete;
    HttpConnection(HttpConnection &&other) noexcept;
    ~HttpConnection();
    void process();
    void close_connection();
private:
    int client_fd_;
    const Router& router_;
    Buffer read_buffer_;
    Buffer write_buffer_;
    Request request_;
    Response response_;
    HttpParser parser_;
    bool isClosed_ = false;
    bool isKeepAlive_ = false; // TODO 目前暂时不支持，之后支持

    void recv_data();

};


#endif //TOYCPPSERVER_HTTPCONNECTION_H