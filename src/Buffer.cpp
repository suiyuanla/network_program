#include "Buffer.h"

#include <sys/types.h>
#include <unistd.h>

#include <cerrno>

Buffer::Buffer() : read_index_(0), write_index_(0) {}

Buffer::~Buffer() {}

size_t Buffer::readableBytes() const { return write_index_ - read_index_; }

size_t Buffer::wirtableBytes() const { return buffer_.size() - write_index_; }

void Buffer::append(const char* data, size_t len) {
    auto writable = wirtableBytes();
    // 处理空间不够的情况
    if (writable < len) {
        if (read_index_ == 0) {
            // buffer_.resize(2 * buffer_.size());
            buffer_.resize(buffer_.size() + len);
        } else {
            // 如果读指针不在0位置
            // 此时如果读指针前的空间和可写空间的和大于len，则将数据向前移动
            if (read_index_ + writable < len) {
                // 数据向前移动
                std::copy(begin() + read_index_, begin() + write_index_,
                          begin());
                write_index_ -= read_index_;
                read_index_ = 0;
            } else {
                // 空间不够则重新分配空间
                // 重新分配空间
                std::vector<char> new_buffer(write_index_ - read_index_ + len);
                std::copy(begin() + read_index_, begin() + write_index_,
                          new_buffer.begin());
                buffer_.swap(new_buffer);
                write_index_ -= read_index_;
                read_index_ = 0;
            }
        }
    }
    // 拷贝数据
    std::copy(data, data + len, begin() + write_index_);
    write_index_ += len;
}

void Buffer::retrieve(size_t len) {
    if (len < readableBytes()) {
        read_index_ += len;
        if (read_index_ == write_index_) {
            read_index_ = 0;
            write_index_ = 0;
        }
    } else {
        read_index_ = write_index_ = 0;
    }
}

void Buffer::retrieveAll() {
    read_index_ = 0;
    write_index_ = 0;
}

ssize_t Buffer::readFd(int fd, int* savedErrno) {
    char buf[65536];
    ssize_t n = ::read(fd, buf, sizeof(buf));
    if (n >= 0) {
        append(buf, n);
    } else {
        *savedErrno = errno;
    }
    return n;
}

const char* Buffer::peek() const { return &buffer_[read_index_]; }