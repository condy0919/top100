#ifndef TOP100_TOPN_HPP_
#define TOP100_TOPN_HPP_

#include <queue>
#include <cstdint>
#include <functional>

namespace top100 {

template <typename T, std::size_t N, typename Compare = std::greater<T>>
class Top {
public:
    Top() = default;

    bool empty() const noexcept {
        return pq_.empty();
    }

    std::size_t size() const noexcept {
        return pq_.size();
    }

    void push(const T& v) {
        pq_.push(v);
        if (size() > N) {
            pop();
        }
    }

    void push(T&& v) {
        pq_.push(std::forward<T>(v));
        if (size() > N) {
            pop();
        }
    }

    void pop() {
        pq_.pop();
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        pq_.emplace(std::forward<Args>(args)...);
    }

    const T& top() const {
        return pq_.top();
    }

private:
    std::priority_queue<T, std::vector<T>, Compare> pq_;
};

}

#endif
