#include "Socket.h"

#include <unistd.h>

#include "SocketOps.h"

Socket::Socket(int sockfd_) : sockfd_(sockfd_) {}

Socket::~Socket() { ::close(sockfd_); }

void Socket::listen() { SocketOps::listen(sockfd_); }

void Socket::shutdownWrite() { SocketOps::shutdownWrite(sockfd_); }

void Socket::setKeepAlive(bool flag) {
    int optval = flag ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval,
                 static_cast<socklen_t>(sizeof(optval)));
}