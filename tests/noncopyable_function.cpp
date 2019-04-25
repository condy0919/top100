#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "noncopyable_function.hpp"

int add1(int x) {
    return x + 1;
}

struct foo {
    int x;

    int bar(int y) {
        return x += y + 1;
    }
};

TEST_CASE("noncopyable function") {
    using top100::NoncopyableFunction;

    NoncopyableFunction<int()> f1([]() {
        return 1;
    });
    CHECK_EQ(f1(), 1);

    NoncopyableFunction<int(int)> f2(add1);
    CHECK_EQ(f2(1), 2);
    CHECK_EQ(f2(99), 100);

    NoncopyableFunction<int()> f3;
    CHECK_THROWS(f3());

    foo b = { .x = 0 };
    NoncopyableFunction<int(foo&, int)> f4(&foo::bar);
    CHECK_EQ(f4(b, 2), 3);
}
