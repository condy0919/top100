#ifndef TOP100_CONCURRENTQUEUE_HPP_
#define TOP100_CONCURRENTQUEUE_HPP_

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

namespace top100 {

template <typename T>
class ConcurrentQueue {
public:
    ConcurrentQueue() = default;

    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

    void push(const T& v) {
        std::lock_guard<std::mutex> g(mtx_);
        q_.push(v);
        cv_.notify_all();
    }

    void push(T&& v) {
        std::lock_guard<std::mutex> g(mtx_);
        q_.push(std::move(v));
        cv_.notify_one();
    }

    bool tryPop(T& v) {
        std::unique_lock<std::mutex> g(mtx_);
        if (cv_.wait_for(g, std::chrono::milliseconds(100),
                         [&]() { return !q_.empty(); })) {
            v = std::move(q_.front());
            q_.pop();
            return true;
        }
        return false;
    }


private:
    std::mutex mtx_;
    std::queue<T> q_;
    std::condition_variable cv_;
};

}


#endif
