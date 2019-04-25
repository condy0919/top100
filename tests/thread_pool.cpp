#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "thread_pool.hpp"
#include <string>

TEST_CASE("thread pool") {
    using top100::ThreadPool;

    ThreadPool thread_pool(2);

    auto fut1 = thread_pool.submit([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        return std::string("123456789");
    });

    auto fut2 = thread_pool.submit([]() {
        return std::string("abcdefghijklmnopqrstuvwxyz");
    });

    CHECK_EQ(fut1.get() + fut2.get(), "123456789abcdefghijklmnopqrstuvwxyz");
}
