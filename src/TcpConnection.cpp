#include "TcpConnection.h"

#include <cerrno>
#include <cstddef>
#include <memory>

#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "SocketOps.h"

TcpConnection::TcpConnection(EventLoop* loop, int sockfd)
    : loop_(loop),
      channel_(new Channel(loop, sockfd)),
      socket_(std::make_unique<Socket>(sockfd)),
      status_(kConnecting) {
    // 注册回调函数
    channel_->setReadCallback([this] { handleRead(); });
    channel_->setWriteCallback([this] { handleWrite(); });
    channel_->setCloseCallback([this] { handleClose(); });
    channel_->setErrorCallback([this] { handleError(); });

    socket_->setKeepAlive(true);
}

void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->enableReading();
    // 连接建立的回调
    if (connection_callback_) {
        connection_callback_(shared_from_this());
    }
}

void TcpConnection::connectDestroyed() {
    if (status_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();
        // 连接断开的回调
        if (connection_callback_) {
            connection_callback_(shared_from_this());
        }
    }
    channel_->remove();
}

void TcpConnection::send(const char* data, size_t len) {
    bool is_disconnect = false;
    size_t n_wrote = 0;
    size_t remaining = len;

    // 如果连接已经断开，放弃发送数据
    if (status_ == kDisconnected) {
        return;
    }

    // 如果当前没有在写并且写缓冲为空，直接尝试写数据
    if (channel_->isWriting() && output_buffer_.readableBytes() == 0) {
        n_wrote = SocketOps::write(channel_->fd(), data, len);
        if (n_wrote >= 0) {
            remaining -= n_wrote;
            if (remaining == 0) {
                //  write complete
                if (write_complete_callback_) {
                    write_complete_callback_(shared_from_this());
                }
            }
        } else {
            n_wrote = 0;
            if (errno != EWOULDBLOCK) {
                // 检查是不是连接断开
                if (errno == EPIPE || errno == ECONNRESET) {
                    is_disconnect = true;
                }
            }
        }
    }

    // 如果tcp连接没有断开，并且有数据没有直接写入，将数据写入缓冲区
    if (!is_disconnect && remaining > 0) {
        //  缓冲区缓存过多时的处理
        auto old_len = output_buffer_.readableBytes();
        if (old_len + remaining >= high_water_mark_ &&
            old_len < high_water_mark_ && high_water_callback_) {
            high_water_callback_(shared_from_this(), old_len + remaining);
        }
        output_buffer_.append(data + n_wrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

// 关闭写
void TcpConnection::shutdown() {
    if (status_ == kConnected) {
        setState(kDisconnecting);

        if (!channel_->isWriting()) {
            socket_->shutdownWrite();
        }
    }
}

void TcpConnection::forceClose() {
    if (status_ == kConnected || status_ == kDisconnecting) {
        setState(kDisconnecting);
        handleClose();
    }
}

void TcpConnection::handleRead() {
    int saved_errno = 0;
    ssize_t n = input_buffer_.readFd(channel_->fd(), &saved_errno);
    if (n > 0) {
        //  message callback
        if (message_callback_) {
            message_callback_(shared_from_this(), &input_buffer_, n);
        }
    } else if (n == 0) {
        handleClose();
    } else {
        errno = saved_errno;
        handleError();
    }
}

void TcpConnection::handleWrite() {
    if (channel_->isWriting()) {
        ssize_t n = SocketOps::write(channel_->fd(), output_buffer_.peek(),
                                     output_buffer_.readableBytes());
        if (n > 0) {
            output_buffer_.retrieve(n);
            if (output_buffer_.readableBytes() == 0) {
                channel_->disableWriting();
                // 写完数据的回调
                if (write_complete_callback_) {
                    write_complete_callback_(shared_from_this());
                }
                if (status_ == kDisconnecting) {
                    socket_->shutdownWrite();
                }
            }
        }
    }
}

void TcpConnection::handleClose() {
    setState(kDisconnected);
    channel_->disableAll();
    // 关闭连接的回调
    if (connection_callback_) {
        connection_callback_(shared_from_this());
    }
    if (close_callback_) {
        close_callback_(shared_from_this());
    }
}

void TcpConnection::handleError() {
    // TODO: 错误处理
}