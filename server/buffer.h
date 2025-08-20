//
// Created by 杨博元 on 2025/8/18.
//

#ifndef TOYCPPSERVER_BUFFER_H
#define TOYCPPSERVER_BUFFER_H
#include <vector>
#define BUFSIZE 1024

class Buffer {
private:
    std::vector<char> buffer_;
    // 读写指针为未来设计使用，目前无用。
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;
public:
    Buffer();
    ~Buffer() = default;
    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;
    Buffer(Buffer && other) noexcept;
    Buffer &operator=(Buffer && other) noexcept;

public:
    void append(const char* data, size_t len);
    void append(const std::vector<char>& data);
    void append(const std::string& data);
    [[nodiscard]] const char* data() const;
    void clear();
    [[nodiscard]] size_t size() const;
    void consume(size_t len);
    bool getline(std::string& line, const char* delim = "\r\n");

};


#endif //TOYCPPSERVER_BUFFER_H