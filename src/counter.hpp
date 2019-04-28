#ifndef TOP100_COUNTER_HPP_
#define TOP100_COUNTER_HPP_

#include <fstream>
#include <vector>
#include "prefetched.hpp"
#include "topn.hpp"

namespace top100 {

class Counter {
public:
    Counter(std::ifstream ifs) noexcept : prefetched_(std::move(ifs)) {}

    void count();

    std::vector<std::pair<std::size_t, std::string>> getResult();

private:
    static const std::size_t N = 100;

    Prefetched prefetched_;
    Top<std::pair<std::size_t, std::string>, N> top100_;
};

}

#endif
