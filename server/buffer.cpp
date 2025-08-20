//
// Created by 杨博元 on 2025/8/18.
//

#include "buffer.h"

Buffer::Buffer() :read_pos_(0),write_pos_(0){
    buffer_.reserve(BUFSIZE);
}

Buffer::Buffer(Buffer &&other) noexcept :
    read_pos_(other.read_pos_),
    write_pos_(other.write_pos_),
    buffer_(std::move(other.buffer_)) {

    other.read_pos_ = 0;
    other.write_pos_ = 0;
}

Buffer & Buffer::operator=(Buffer &&other) noexcept {
    if (this != &other) {
        read_pos_ = other.read_pos_;
        write_pos_ = other.write_pos_;
        buffer_ = std::move(other.buffer_);

        other.read_pos_ = 0;
        other.write_pos_ = 0;
    }
    return *this;
}

void Buffer::append(const char *data, const size_t len) {
    buffer_.insert(buffer_.end(), data, data + len);
}

void Buffer::append(const std::vector<char> &data) {
    append(data.data(), data.size());
}

void Buffer::append(const std::string &data) {
    append(data.data(), data.length());
}

const char * Buffer::data() const {
    return buffer_.empty() ? nullptr : buffer_.data();
}

void Buffer::clear() {
    buffer_.clear();
}

size_t Buffer::size() const {
    return buffer_.size();
}

void Buffer::consume(const size_t len) {
    if (buffer_.size() < len) {
        buffer_.clear(); // 所需要读取的大小大于当前大小，直接清零
    } else {
        // 从头部移除len长度数据
        buffer_.erase(buffer_.begin(), buffer_.begin() + len);
    }
}

bool Buffer::getline(std::string &line, const char *delim) {
    if (buffer_.empty()) {
        line.clear();
        return false;
    }

    size_t delim_len = strlen(delim);
    if (delim_len == 0) {
        line.clear();
        return false;
    }

    const char *data = buffer_.data();
    size_t size = buffer_.size();

    // std::search查找 [first1, last1) 范围内第一个 [first2, last2) 子序列。这里查找第一个分割符
    const char  *delim_pos = std::search(data, data + size, delim, delim+ delim_len);

    if (delim_pos != data + size) {
        // 在范围中找到了
        const size_t line_len = std::distance(data, delim_pos);
        line.assign(data, line_len);
        consume(line_len + delim_len);
        return true;
    }

    // 没有找到完整行，delim_pos为容器尾部
    line.clear();
    return false;
}
