//
// Created by 杨博元 on 2025/8/20.
//

#include "http_connection.h"

#include <iostream>
#include <utility>

#include "middleware.h"




HttpConnection::HttpConnection(int client_fd, std::shared_ptr<Middleware> middleware):client_fd_(client_fd),middlewareChain_(std::move(middleware)) {
}

HttpConnection::HttpConnection(HttpConnection &&other) noexcept:
    client_fd_(other.client_fd_),
    read_buffer_(std::move(other.read_buffer_)),
    write_buffer_(std::move(other.write_buffer_)),
    middlewareChain_(std::move(other.middlewareChain_)),
    request_(std::move(other.request_)),
    response_(std::move(other.response_)),
    parser_(other.parser_),
    isClosed_(other.isClosed_),
    isKeepAlive_(other.isKeepAlive_)
{
    other.client_fd_ = -1;
    other.isClosed_ = true;
}

HttpConnection::~HttpConnection() {
    close_connection();
}


void HttpConnection::process() {
    while (!isClosed_) {

        // 接受数据到缓冲区
        recv_data();
        if (isClosed_) {
            break;
        }

        // 解析数据

        if (!parser_.parse(read_buffer_)) {
            continue;
        }
        request_ = parser_.getRequest();
        parser_.reset(); // 解析完了重置解析器

        std::cout << "Method: " << request_.method << "\n"
            << "URI: " << request_.uri << "\n"
            << "Body: " << request_.body << std::endl;

        // // 获取Handler
        // HandlerReturn result = router_.getHandler(request_.method, request_.uri);
        // if (result.success) {
        //     if (!result.params.empty()) {
        //         request_.params = result.params;
        //     }
        //     // 执行
        //     result.handler(request_,response_);
        // }
        if (!middlewareChain_ || !middlewareChain_->process(request_, response_)) {
            response_.status = 404;
            response_.body = "404 Not Found";
            response_.headers["Content-Type"] = "text/plain";
        }

        std::string response_string = response_.generateResponse();
        std::cout << response_string << std::endl;
        ssize_t sent =  send(client_fd_, response_string.c_str(), response_string.size(), 0);
        if (sent < 0) {
            close_connection();
        }

        request_.clear();
        response_.clear();
        // 目前不实现长连接
        if (!isKeepAlive_) {
            close_connection();
            break;
        }
    }


}

void HttpConnection::close_connection() {
    if (isClosed_ == false) {
        isClosed_ = true;
        if (client_fd_ != -1) {
            close(client_fd_);
            std::cout << "Client disconnected\n";
        }
    }

}

void HttpConnection::recv_data() {
    std::vector<char> temp_buffer(4096);
    ssize_t recv_size = recv(client_fd_, temp_buffer.data(), 4096, 0);
    if (recv_size > 0) {
        temp_buffer.resize(recv_size);
        read_buffer_.append(temp_buffer.data(), recv_size);
    } else if (recv_size == 0) {
        close_connection(); // 客户端关闭连接
    } else {
        // 错误处理
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            close_connection();
        }
    }

}


