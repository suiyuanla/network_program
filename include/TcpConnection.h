#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <functional>
#include <memory>

#include "Buffer.h"

class TcpConnection;

using ConnectionPtr = std::shared_ptr<TcpConnection>;
using MessageCallback = std::function<void(TcpConnection*, Buffer*, size_t)>;
using ConnectionCallback = std::function<void(const ConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const ConnectionPtr&)>;

class EventLoop;
class Channel;
class Socket;
class TcpConnection {
   public:
    TcpConnection(EventLoop* loop, int sockfd);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    // bool connected() const { return status_ == 1; }

    void send(const char* data, size_t len);
    //  void send(Buffer&& buffer);
    //  void send(std::string&& buffer);

    void shutdown();
    void forceClose();

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
};

#endif  // TCP_CONNECTION_H