//
// Created by 杨博元 on 2025/8/19.
//

#include <iostream>

#include "response.h"

int main() {
    Response res;
    res.status = 200;
    res.headers["Content-Type"] = "text/plain";
    res.body = "Hello, World!";

    std::string http = res.generateResponse();
    std::cout << "Generated Response:\n" << http << std::endl;
    return 0;
}