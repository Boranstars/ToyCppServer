//
// Created by 杨博元 on 2025/8/21.
//

#ifndef TOYCPPSERVER_STATIC_FILE_MIDDLEWARE_H
#define TOYCPPSERVER_STATIC_FILE_MIDDLEWARE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "middleware.h"
class StaticFileMiddleware :public Middleware{
public:
    explicit StaticFileMiddleware(std::string_view baseDir);


    bool process(Request &req, Response &res) override;

    // void setNext(std::shared_ptr<Middleware> next) override;

    /**
     *
     * @param extension 文件后缀
     * @param mimeType 对应的mimeType
     */
    void addMimeType(const std::string& extension, const std::string& mimeType);
    void setIndexFiles(const std::vector<std::string>& indexFiles);
private:
    std::string baseDir_;
    std::vector<std::string> indexFiles_;
    std::unordered_map<std::string, std::string> mimeTypes_;

    // 辅助方法
    /**
     * @brief 尝试请求文件或者路径
     * @param path 请求的路径
     * @param res 需要加载到的响应
     * @return 是否成功
     */
    bool serveFile(const std::string& path, Response& res);

    /**
     * @brief 请求单个文件
     * @param path 请求的文件路径
     * @param res 需要加载到的响应
     * @return 是否成功
     */
    bool serveSingleFile(const std::string& path, Response& res);
    std::string getMimeType(const std::string& extension);
    [[nodiscard]] bool isPathSafe(const std::string& requestedPath) const;
    static std::string urlDecode(const std::string& str);
};


#endif //TOYCPPSERVER_STATIC_FILE_MIDDLEWARE_H