#ifndef BUFFER_H
#define BUFFER_H
#include <sys/types.h>

#include <cstddef>
#include <vector>
class Buffer {
   public:
    Buffer();
    ~Buffer();

    size_t readableBytes() const;
    size_t wirtableBytes() const;

    void append(const char* data, size_t len);
    void retrieve(size_t len);
    void retrieveAll();
    ssize_t readFd(int fd, int* savedErrno);
    // 返回缓冲区中第一个可读数据的指针
    const char* peek() const;

    bool empty() const { return read_index_ == write_index_; }

   private:
    char* begin() { return buffer_.data(); }
    std::vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;
};
#endif  // BUFFER_H