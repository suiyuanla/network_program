#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

class EventLoop;

class Channel {
   public:
    using EventCallback = std::function<void()>;

    enum { kNew = -1, kAdded = 1, kDeleted = 2 };

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // 处理事件
    void handleEvent();

    // 设置对应的事件回调函数
    void setReadCallback(EventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    int fd() const { return fd_; }
    int listenEvents() const { return listen_events_; }
    bool isNoneEvent() const { return listen_events_ == 0; }

    auto status() const { return status_; }
    auto setStatus(int status) { status_ = status; }

    // 设置监听事件、活跃事件
    void setListenEvents(int evt) { listen_events_ = evt; }
    void setResultEvents(int evt) { result_events_ = evt; }

    // 打开或关闭对应的事件监听
    void enableReading();
    void disableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    // 将channel从epoller中移除
    void remove();

    // 是否正在监听事件
    bool isWriting() const;
    bool isReading() const;

    EventLoop* ownerLoop() { return loop_; }

   private:
    void update();

    EventLoop* loop_;
    // channel对应的事件描述符
    int fd_;
    // channel监听的事件
    int listen_events_ = 0;
    // epoll_wait返回的事件
    int result_events_ = 0;

    // 是否正在处理事件
    bool event_handling_ = false;
    // 是否添加到epoller中
    int status_ = kNew;

    // 事件回调函数
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif  // CHANNEL_H