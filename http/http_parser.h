//
// Created by 杨博元 on 2025/8/18.
//

#ifndef TOYCPPSERVER_HTTP_H
#define TOYCPPSERVER_HTTP_H
#include <string>
#include <unordered_map>
#include <vector>
#include "buffer.h"
struct Request {
    std::string method;
    std::string uri;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::unordered_map<std::string, std::string> params;
    void init () {
        method = "";
        uri = "";
        version = "";
        headers.clear();
        body = "";
        params.clear();
    }
};


class HttpParser {
    public:
    HttpParser();
    ~HttpParser() = default;
    enum ParseState {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH
    };
    bool parse(Buffer& buffer);
    void reset();
    const Request& getRequest();
private:
    ParseState parse_state_;
    Request request_;
    void parseRequestLine(const std::string& line);
    void parseHeaders(const std::string& line);
    void parseBody(Buffer& buffer);
};
#endif //TOYCPPSERVER_HTTP_H