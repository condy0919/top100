#ifndef TOP100_COUNTER_HPP_
#define TOP100_COUNTER_HPP_

#include <fstream>
#include <vector>
#include "prefetched.hpp"
#include "topn.hpp"

namespace top100 {

class Counter {
public:
    Counter() = default;

    void count(std::string s);

    void flush();

    std::vector<std::pair<std::size_t, std::string>> getResult();

private:
    static const std::size_t N = 100;

    std::size_t cnt_ = 0;

    std::string last_;

    Top<std::pair<std::size_t, std::string>, N> top100_;
};

}

#endif
