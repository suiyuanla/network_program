#include "Epoller.h"

#include <pstl/glue_algorithm_defs.h>
#include <sys/epoll.h>

#include <algorithm>

#include "Channel.h"

Epoller::Epoller(EventLoop* loop)
    : owner_loop_(loop),
      epoll_fd_(epoll_create1(EPOLL_CLOEXEC)),
      channels_(kInitEventListSize) {}

void Epoller::poll(int timeout_ms, ChannelList* active_channels) {
    int num_events =
        epoll_wait(epoll_fd_, events_.data(), events_.size(), timeout_ms);

    if (num_events > 0) {
        fillActiveChannels(num_events, active_channels);
    }
}

void Epoller::fillActiveChannels(int num_events,
                                 ChannelList* active_channels) const {
    for (int i = 0; i < num_events; ++i) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setResultEvents(events_[i].events);
        active_channels->push_back(channel);
    }
}

void Epoller::update(int op, Channel* channel) {
    struct epoll_event event;
    event.events = channel->listenEvents();
    event.data.ptr = channel;
    epoll_ctl(epoll_fd_, op, channel->fd(), &event);
}

void Epoller::updateChannel(Channel* channel) {
    int op;

    if (channel->status() == Channel::kNew ||
        channel->status() == Channel::kDeleted) {
        // 把新的channel添加到epoller中
        if (channel->status() == Channel::kNew) {
            channels_.push_back(channel);
        }
        op = EPOLL_CTL_ADD;
        channel->setStatus(Channel::kAdded);
    } else {
        // 如果channel没有监听事件。就从epoll_ctl中删除
        if (channel->isNoneEvent()) {
            op = EPOLL_CTL_DEL;
            channel->setStatus(Channel::kDeleted);
        } else {
            op = EPOLL_CTL_MOD;
        }
    }
    update(op, channel);
}

void Epoller::removeChannel(Channel* channel) {
    if (channel->status() == Channel::kAdded ||
        channel->status() == Channel::kDeleted) {
        if (channel->status() == Channel::kAdded) {
            update(EPOLL_CTL_DEL, channel);
        }
        channels_.erase(
            std::remove(channels_.begin(), channels_.end(), channel),
            channels_.end());
        channel->setStatus(Channel::kNew);
    }
}