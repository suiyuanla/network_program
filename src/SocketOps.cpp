#include "SocketOps.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Log.h"

int SocketOps::noneBlockSocket(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                          IPPROTO_TCP);
    if (sockfd < 0) {
        LOG("socket create error!");
    }
    return sockfd;
}

void SocketOps::bind(int sockfd, const struct sockaddr* addr) {
    int ret = ::bind(sockfd, addr,
                     static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    if (ret < 0) {
        LOG("bind error!");
    }
}

void SocketOps::listen(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);

    if (ret < 0) {
        LOG("listen error!");
    }
}

int SocketOps::accept(int sockfd, struct sockaddr_in* addr) {
    auto addrlen = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    int connfd = ::accept4(sockfd, (sockaddr*)(addr), &addrlen,
                           SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        LOG("accept error!");
    }
    return connfd;
}

int SocketOps::connect(int sockfd, const struct sockaddr* addr) {
    return ::connect(sockfd, addr,
                     static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

void SocketOps::close(int sockfd) {
    if (::close(sockfd) < 0) {
        LOG("close error!");
    }
}

void SocketOps::shutdownWrite(int fd) {
    if (::shutdown(fd, SHUT_WR) < 0) {
        LOG("shutdown error!");
    }
}

ssize_t SocketOps::read(int sockfd, void* buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t SocketOps::write(int sockfd, const void* buf, size_t count) {
    return ::write(sockfd, buf, count);
}
