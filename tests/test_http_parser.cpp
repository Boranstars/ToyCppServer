//
// Created by 杨博元 on 2025/8/19.
//

#include "http_parser.h"
#include <iostream>
int main() {
    // 测试用例
    Buffer buf;
    HttpParser parser;
    // 模拟HTTP请求
    buf.append("POST /submit HTTP/1.1\r\n");
    buf.append("Content-Type: text/plain\r\n");
    buf.append("Content-Length: 11\r\n\r\n");
    buf.append("Hello World");
    // 解析请求
    bool res = parser.parse(buf);
    if (res) {
        const Request& req = parser.getRequest();
        std::cout << "Method: " << req.method << "\n"
                  << "URI: " << req.uri << "\n"
                  << "Body: " << req.body << std::endl;
    }
}