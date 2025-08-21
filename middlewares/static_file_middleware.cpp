//
// Created by 杨博元 on 2025/8/21.
//

#include "static_file_middleware.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>


#include "http_parser.h"
#include "response.h"


// [高优先级] 大文件处理优化
// TODO: 实现流式文件传输，避免大文件完全加载到内存
// 方案：在Response中添加文件流支持，使用分块传输或sendfile系统调用

// [中优先级] 缓存机制
// TODO: 添加ETag和Last-Modified头支持缓存验证
// TODO: 实现内存缓存高频访问的小文件（如CSS、JS）

// [可选] 零拷贝优化
// TODO: 在支持的系统上使用sendfile()系统调用




StaticFileMiddleware::StaticFileMiddleware(const std::string_view baseDir) :baseDir_(baseDir){
    // 默认MIME类型
    mimeTypes_[".html"] = "text/html";
    mimeTypes_[".htm"] = "text/html";
    mimeTypes_[".css"] = "text/css";
    mimeTypes_[".js"] = "application/javascript";
    mimeTypes_[".png"] = "image/png";
    mimeTypes_[".jpg"] = "image/jpeg";
    mimeTypes_[".jpeg"] = "image/jpeg";
    mimeTypes_[".gif"] = "image/gif";
    mimeTypes_[".svg"] = "image/svg+xml";
    mimeTypes_[".ico"] = "image/x-icon";
    mimeTypes_[".txt"] = "text/plain";
    mimeTypes_[".json"] = "application/json";

    // 默认索引文件
    indexFiles_ = {"index.html", "index.htm"};
}

bool StaticFileMiddleware::process(Request &req, Response &res) {
    // 文件加载类只处理GET和HEAD
    if (req.method != "GET" && req.method != "HEAD") {
        return next_->process(req, res); // 传递给下一个中间件
    }

    if (serveFile(req.uri,res)) {
        return true; // 已经处理请求，返回
    }

    return next(req,res); // 文件请求失败？交给下一个中间件，如路由处理
}



void StaticFileMiddleware::addMimeType(const std::string &extension, const std::string &mimeType) {
    mimeTypes_[extension] = mimeType;
}

void StaticFileMiddleware::setIndexFiles(const std::vector<std::string> &indexFiles) {
    indexFiles_ = indexFiles;
}

bool StaticFileMiddleware::serveFile(const std::string &path, Response &res) {
    // 1. 构建完整文件路径：baseDir_ + path
    // 2. 安全检查：使用 isPathSafe() 防止目录遍历攻击
    // 3. 检查是否是目录：如果是，尝试提供索引文件
    // 4. 检查文件是否存在且可读
    // 5. 读取文件内容到 res.body
    // 6. 设置正确的 Content-Type 和状态码
    // 7. 返回 true 表示成功处理


    std::string fullPath = baseDir_ + urlDecode(path);

    if (!isPathSafe(fullPath)) {
        // 安全检查，防止目录遍历攻击
        return false;
    }

    // 检查是否为一个目录
    if (std::filesystem::is_directory(fullPath)) {
        // 如果请求路径不以斜杠结尾，重定向到带斜杠的版本
        if (path.empty() || path.back() != '/') {
            res.status = 301;
            res.headers["Location"] = path + "/";
            return true;
        }

        // 先尝试提供目录下的索引文件
        for (const auto & indexFile: indexFiles_) {
            std::string indexPath = fullPath + "/" + indexFile;
            if (std::filesystem::exists(indexPath) && std::filesystem::is_regular_file(indexPath)) {
                return serveSingleFile(indexPath,res);
            }

        }
        return false; // 目录没有索引文件
    }

    // 不是目录，检查是否为一个普通文件
    if (!std::filesystem::exists(fullPath) ||
        !std::filesystem::is_regular_file(fullPath)) {
        return false; // 文件不存在或不是普通文件
        }

    // 尝试提供这个文件
    return serveSingleFile(fullPath,res);

}

bool StaticFileMiddleware::serveSingleFile(const std::string &path, Response &res) {
    try {
        // 打开文件
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // 读取内容
        std::stringstream buffer;
        buffer << file.rdbuf();
        res.body = buffer.str();

        //设置响应
        res.status = 200;
        std::filesystem::path pathObj(path);
        std::string extension = pathObj.extension().string();
        res.headers["Content-Type"] = getMimeType(extension);
        // 虽然根据response类的实现，如果不存在Content-Length会自动设置，但是这里为了确保还是设置一下
        res.headers["Content-Length"] = std::to_string(res.body.size());

        // 设置缓存头（可选），调试的时候关了
        res.headers["Cache-Control"] = "public, max-age=3600"; // 缓存1小时

        return true;

    } catch (const std::exception &e) {
        // 读取失败
        return false;
    }
}

std::string StaticFileMiddleware::getMimeType(const std::string &extension) {
    auto it = mimeTypes_.find(extension);
    if (it != mimeTypes_.end()) {
        return it->second;
    }
    return "application/octet-stream"; // 默认类型，为二进制，一般会触发下载
}

bool StaticFileMiddleware::isPathSafe(const std::string &requestedPath) const {
    // 关键：确保请求的路径不会跳出 baseDir_ 目录
    // 方法：使用 std::filesystem::canonical() 获取规范化路径后比较
    // 返回 true 如果路径安全，false 如果可能造成目录遍历攻击

    try {
        // 获取规范化路径。规范路径是指消除路径中的符号链接、相对路径等，得到一个唯一的绝对路径
        std::filesystem::path canonicalRequestedPath = std::filesystem::canonical(requestedPath);
        std::filesystem::path basePath = std::filesystem::canonical(baseDir_);

        auto [mismatch, _] = std::mismatch(
            basePath.begin(),
            basePath.end(),
            canonicalRequestedPath.begin());
        return mismatch == basePath.end();
    } catch (const std::exception &e) {
        return false;
    }

}

std::string StaticFileMiddleware::urlDecode(const std::string &str) {

    std::string result;
    result.reserve(str.length());

    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            const char hex[3] = {str[i+1], str[i+2], '\0'};
            char* end;
            const long val = std::strtol(hex, &end, 16);

            if (end == hex + 2) {
                result += static_cast<char>(val);
                i += 2;
                continue;
            }
        }
        result += (str[i] == '+') ? ' ' : str[i];
    }
    return result;

}
