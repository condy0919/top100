#ifndef TOP100_THREAD_POOL_HPP_
#define TOP100_THREAD_POOL_HPP_

#include "noncopyable_function.hpp"
#include "cqueue.hpp"
#include <vector>
#include <atomic>
#include <thread>
#include <future>
#include <algorithm>
#include <type_traits>

namespace top100 {

class ThreadPool {
public:
    ThreadPool(std::size_t nworker = std::thread::hardware_concurrency())
        : done_(false) {
        if (nworker > std::thread::hardware_concurrency()) {
            nworker = std::thread::hardware_concurrency();
        }

        idle_ = nworker;

        for (std::size_t i = 0; i < nworker; ++i) {
            workers_.emplace_back([this]() {
                NoncopyableFunction<void()> task;
                while (!done_) {
                    if (tasks_.tryPop(task)) {
                        --idle_;
                        task();
                        ++idle_;
                    } else {
                        std::this_thread::yield();
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        done_ = true;
        for (auto& w : workers_) {
            if (w.joinable()) {
                w.join();
            }
        }
    }

    bool avail() noexcept {
        return idle_ > 0;
    }

    template <typename F, typename R = std::result_of_t<F()>>
    std::future<R> submit(F f) {
        std::packaged_task<R()> task(std::move(f));

        auto fut = task.get_future();
        tasks_.push(std::move(task));
        return fut;
    }

private:
    std::atomic_bool done_;
    std::atomic_size_t idle_;
    std::vector<std::thread> workers_;
    ConcurrentQueue<NoncopyableFunction<void()>> tasks_;
};

}

#endif
