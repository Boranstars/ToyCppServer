//
// Created by 杨博元 on 2025/8/19.
//

#include "response.h"


const std::map<int, std::string> Response::status_texts = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {503, "Service Unavailable"}
};

std::string Response::generateResponse() {
    std::ostringstream oss;
    // status line
    const std::string version = "HTTP/1.1 ";
    oss << version << status << " " <<
    (status_texts.contains(status) ? status_texts.at(status) : "UNKNOWN");
    oss << "\r\n";
    // headers
    if (!headers.contains("Content-Length")) {
        headers["Content-Length"] = std::to_string(body.size());
    }
    for (const auto &[k,v] : headers) {
        oss  << k << ": " << v << "\r\n";
    }

    oss << "\r\n";

    // body
    oss << body;

    return oss.str();
}

void Response::clear() {
    headers.clear();
    body.clear();
    status = 0;
}
