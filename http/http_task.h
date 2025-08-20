//
// Created by 杨博元 on 2025/8/20.
//

#ifndef TOYCPPSERVER_HTTPTASK_H
#define TOYCPPSERVER_HTTPTASK_H
#include "router.h"

class HttpTask {
    public:
    HttpTask(int client_fd,Router& router);
    HttpTask(const HttpTask &) = delete;
    HttpTask &operator=(const HttpTask &) = delete;


};


#endif //TOYCPPSERVER_HTTPTASK_H