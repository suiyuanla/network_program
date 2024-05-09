#include "Channel.h"

#include <sys/epoll.h>

#include "EventLoop.h"

Channel::Channel(EventLoop* loop, int fd__) : loop_(loop), fd_(fd__) {}

Channel::~Channel() {}

void Channel::enableReading() {
    listen_events_ |= EPOLLIN;
    update();
}

void Channel::disableReading() {
    listen_events_ &= ~EPOLLIN;
    update();
}

void Channel::enableWriting() {
    listen_events_ |= EPOLLOUT;
    update();
}

void Channel::disableWriting() {
    listen_events_ &= ~EPOLLOUT;
    update();
}

void Channel::disableAll() {
    listen_events_ = 0;
    update();
}

bool Channel::isWriting() const { return listen_events_ & EPOLLOUT; }

bool Channel::isReading() const { return listen_events_ & EPOLLIN; }

void Channel::handleEvent() {
    event_handling_ = true;
    if (result_events_ & EPOLLIN) {
        if (readCallback_) readCallback_();
    }
    if (result_events_ & EPOLLOUT) {
        if (writeCallback_) writeCallback_();
    }
    if (result_events_ & EPOLLERR) {
        if (errorCallback_) errorCallback_();
    }
    if (result_events_ & EPOLLHUP) {
        if (closeCallback_) closeCallback_();
    }
    event_handling_ = false;
}

void Channel::update() { loop_->updateChannel(this); }