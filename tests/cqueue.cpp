#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cqueue.hpp"
#include <thread>

TEST_CASE("thread safe queue using mutex") {
    using top100::CQueue;

    std::vector<int> xs;

    bool done = false;
    CQueue<int> cq;

    std::thread t1([&]() {
        int v = -1;
        while (!done) {
            if (cq.tryPop(v)) {
                xs.push_back(v);
            } else {
                __builtin_ia32_pause();
            }
        }
    });

    std::thread t2([&]() {
        for (int i = 0; i < 1000; ++i) {
            cq.push(i);
        }
    });

    if (t1.joinable()) {
        t1.join();
    }

    if (t2.joinable()) {
        t2.join();
    }

    CHECK(std::is_sorted(xs.begin(), xs.end()));
}
