#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <cstddef>
#include <functional>
#include <memory>

#include "Buffer.h"

class TcpConnection;

using ConnectionPtr = std::shared_ptr<TcpConnection>;
using MessageCallback =
    std::function<void(const ConnectionPtr&, Buffer*, size_t)>;
using ConnectionCallback = std::function<void(const ConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const ConnectionPtr&)>;
using CloseCallback = std::function<void(const ConnectionPtr&)>;
using HighWaterCallback = std::function<void(const ConnectionPtr&, size_t)>;

class EventLoop;
class Channel;
class Socket;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
   public:
    TcpConnection(EventLoop* loop, int sockfd);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    // bool connected() const { return status_ == 1; }

    void connectEstablished();
    void connectDestroyed();
    void send(const char* data, size_t len);
    //  void send(Buffer&& buffer);
    //  void send(std::string&& buffer);

    void shutdown();
    void forceClose();

    void setMessageCallback(const MessageCallback& cb) {
        message_callback_ = cb;
    }
    void setConnectionCallback(const ConnectionCallback& cb) {
        connection_callback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        write_complete_callback_ = cb;
    }
    void setCloseCallback(const CloseCallback& cb) { close_callback_ = cb; }
    void setHighWaterCallback(const HighWaterCallback& cb,
                              size_t high_water_mark) {
        high_water_callback_ = cb;
        high_water_mark_ = high_water_mark;
    }

    Buffer* getInputBuffer() { return &input_buffer_; }
    Buffer* getOutputBuffer() { return &output_buffer_; }

   private:
    enum Status { kConnecting, kConnected, kDisconnecting, kDisconnected };

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    void setState(Status s) { status_ = s; }
    EventLoop* loop_;

    //  int status_;
    std::unique_ptr<Channel> channel_;
    std::unique_ptr<Socket> socket_;

    Buffer input_buffer_;
    Buffer output_buffer_;

    Status status_;

    // Callbacks
    MessageCallback message_callback_;
    ConnectionCallback connection_callback_;
    WriteCompleteCallback write_complete_callback_;
    CloseCallback close_callback_;
    HighWaterCallback high_water_callback_;

    size_t high_water_mark_;
};

#endif  // TCP_CONNECTION_H