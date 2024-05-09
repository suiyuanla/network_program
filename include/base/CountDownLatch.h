#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include <condition_variable>
#include <mutex>

class CountDownLatch {
   public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

   private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    int count_;
};
#endif  // COUNTDOWNLATCH_H