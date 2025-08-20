//
// Created by 杨博元 on 2025/8/18.
//

#include "http_parser.h"

#include <sstream>
void trim_(std::string &str) {
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}
HttpParser::HttpParser() {
    parse_state_ = REQUEST_LINE;
    request_.clear();
}

bool HttpParser::parse(Buffer &buffer) {
    if (buffer.size() <= 0) {
        return false; //没有可读数据
    }
    while (buffer.size() > 0 && parse_state_ != FINISH) {
        constexpr char CRLF[] = "\r\n";
        std::string line;
        if (parse_state_ == REQUEST_LINE || parse_state_ == HEADERS) {
            if (!buffer.getline(line, CRLF)) {
                return false; // 行不完整，需要更多数据
            }
        }


        // 请求行和头按行处理，消息体按二进制处理
        switch (parse_state_) {
            case REQUEST_LINE:
                parseRequestLine(line);
                break;
            case HEADERS:
                parseHeaders(line);
                break;
            case BODY:
                parseBody(buffer);
                break;

            default:
                return false;
                break;

        }
    }
    // 解析完成

    return true;
}

void HttpParser::reset() {
    parse_state_ = REQUEST_LINE;
    request_.clear();
}

const Request& HttpParser::getRequest() {
    return request_;
}

void HttpParser::parseRequestLine(const std::string &line) {
    std::stringstream ss(line);
    ss >> request_.method >> request_.uri >> request_.version;
    if (request_.method.empty() || request_.uri.empty() || request_.version.empty()) {
        return;
    }

    parse_state_ = HEADERS;

}

void HttpParser::parseHeaders(const std::string &line) {
    if (line.empty()) {
        // 到达结尾
        if (
            request_.method != "GET" && request_.method != "HEAD" &&
                (request_.headers.contains("Content-Length") ||
                request_.headers.contains("Transfer-Encoding"))
            ) {
            // 需要消息体
            parse_state_ = BODY;
        }else {
            parse_state_ = FINISH;
        }
    }

    size_t colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        // 去除首尾空格
        trim_(key);
        trim_(value);
        request_.headers[key] = value;
    }



}

void HttpParser::parseBody(Buffer& buffer) {
    if (buffer.size() <= 0) {
        return;
    }
    size_t body_size = 0;
    if (request_.headers.contains("Content-Length")) {
        // 有指定消息体长度？
        body_size = std::stoul(request_.headers["Content-Length"]);
        if (buffer.size() < body_size) {
            // 数据不足，等待后续数据
            return;
        }
        request_.body.assign(buffer.data(), body_size);
        buffer.consume(body_size);
    } else {
        // 默认全部读取
        request_.body.assign(buffer.data(), buffer.size());
    }
    parse_state_ = FINISH;
}




