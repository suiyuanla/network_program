#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>

#include "NonCopyable.h"

template <typename T>
class BlockingQueue : NonCopyable {
   public:
    using queue_type = std::deque<T>;

    void put(const T& x) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify_one();
    }

    void put(T&& x) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back(std::move(x));
        notEmpty_.notify_one();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return !queue_.empty(); });
        T front(std::move(queue_.front()));
        queue_.pop_front();
        return front;
    }

    queue_type takeAll() {
        queue_type result;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            result = std::move(queue_);
        }
        return result;
    }
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

   private:
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    queue_type queue_;
};

#endif  // BLOCKINGQUEUE_H