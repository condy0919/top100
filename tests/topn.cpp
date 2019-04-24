#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "topn.hpp"

TEST_CASE("TopN") {
    using top100::Top;

    Top<int, 5> top5;
    for (int i = 0; i < 100; ++i) {
        top5.push(i);
    }

    // 95 96 97 98 99
    CHECK_EQ(top5.top(), 95);
    top5.pop();

    CHECK_EQ(top5.top(), 96);
    top5.pop();

    CHECK_EQ(top5.top(), 97);
    top5.pop();

    CHECK_EQ(top5.top(), 98);
    top5.pop();

    CHECK_EQ(top5.top(), 99);
    top5.pop();

    CHECK(top5.empty());
}
