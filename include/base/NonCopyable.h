#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class NonCopyable {
   public:
    NonCopyable() = default;
    ~NonCopyable() = default;

    // 禁止拷贝
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif  // NONCOPYABLE_H