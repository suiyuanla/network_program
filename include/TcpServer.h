#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <memory>

class EventLoop;
class Acceptor;
class TcpServer {
   public:
    TcpServer();
    ~TcpServer();

    void start();
    void setConnectionCallback();
    void setMessageCallback();

   private:
    void newConnection();
    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptor_;
};
#endif  // TCP_SERVER_H