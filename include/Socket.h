#ifndef Socket_H
#define Socket_H

class Socket {
   public:
    Socket(int sockfd_);
    ~Socket();

    void listen();
    void shutdownWrite();
    void setKeepAlive(bool flag);

   private:
    int sockfd_;
};

#endif  // Socket_H