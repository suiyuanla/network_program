#include "EventLoop.h"

#include "Channel.h"
#include "Epoller.h"

constexpr int kPollTimeMs = 10000;

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      epoller_(new Epoller(this)),
      event_handling_(false) {}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
    looping_ = true;
    // FIXME: 确定quit_在此设置为false的必要性
    quit_ = false;
    while (!quit_) {
        epoller_->poll(kPollTimeMs, &active_channels_);

        event_handling_ = true;
        for (auto channel : active_channels_) {
            channel->handleEvent();
        }
        event_handling_ = false;
    }
    looping_ = false;
}

void EventLoop::quit() { quit_ = true; }

void EventLoop::updateChannel(Channel* channel) {
    epoller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    epoller_->removeChannel(channel);
}