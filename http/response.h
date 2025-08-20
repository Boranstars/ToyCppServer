//
// Created by 杨博元 on 2025/8/19.
//

#ifndef TOYCPPSERVER_RESPONSE_H
#define TOYCPPSERVER_RESPONSE_H
#include <map>
#include <string>
#include <unordered_map>
#include <sstream>

struct Response {
    int status;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    static const std::map<int, std::string> status_texts;
    std::string generateResponse();
};
const Response okResponse {
    200,
    {{"Content-Type","text/html"}},
    "<html> \
            <head><title>Toy Cpp Server</title></head> \
            <body> \
            <center><h1>Toy Cpp Server</h1></center> \
            <hr><center>BoranStars</center> \
            </body> \
            </html>"

};


#endif //TOYCPPSERVER_RESPONSE_H