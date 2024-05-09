#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <memory>
#include <vector>
class Channel;
class Epoller;

// 事件循环类
class EventLoop {
   public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    // 更新channel
    void updateChannel(Channel* channel);
    // 移除channel
    void removeChannel(Channel* channel);

   private:
    using ChannelList = std::vector<Channel*>;

    bool looping_;
    bool quit_;
    bool event_handling_;

    std::unique_ptr<Epoller> epoller_;
    ChannelList active_channels_;
};
#endif  // EVENTLOOP_H