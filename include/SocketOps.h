#ifndef SOCKETOP_H
#define SOCKETOP_H

#include <arpa/inet.h>
#include <sys/socket.h>

namespace SocketOps {

// 创建一个非阻塞的socketfd
int noneBlockSocket(sa_family_t family);

int connect(int sockfd, const struct sockaddr* addr);

void bind(int sockfd, const struct sockaddr* addr);

void listen(int sockfd);

int accept(int sockfd, struct sockaddr_in* addr);

void close(int sockfd);

void shutdownWrite(int fd);

ssize_t read(int sockfd, void* buf, size_t count);
// ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
ssize_t write(int sockfd, const void* buf, size_t count);

// struct sockaddr_in6 getLocalAddr(int sockfd);
// struct sockaddr_in6 getPeerAddr(int sockfd);

}  // namespace SocketOps

#endif  // SOCKETOP_H