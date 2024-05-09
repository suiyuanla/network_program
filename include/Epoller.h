#ifndef EPOLLER_H
#define EPOLLER_H

#include <vector>

class EventLoop;
class Channel;

class Epoller {
   public:
    using ChannelList = std::vector<Channel*>;

    Epoller(EventLoop* loop);
    ~Epoller();

    void poll(int timeout_ms, ChannelList* active_channels);
    void updateChannel(Channel*);
    void removeChannel(Channel*);

   private:
    void update(int op, Channel* channel);
    void fillActiveChannels(int num_events, ChannelList* active_channels) const;

    using EventList = std::vector<struct epoll_event>;

    static constexpr int kInitEventListSize = 16;
    EventLoop* owner_loop_;
    int epoll_fd_;

    ChannelList channels_;
    EventList events_;
};

#endif  // EPOLLER_H