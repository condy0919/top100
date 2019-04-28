#ifndef TOP100_SORTER_HPP_
#define TOP100_SORTER_HPP_

#include <fstream>
#include "thread_pool.hpp"

namespace top100 {

class ExternalSorter {
public:
    ExternalSorter(std::ifstream ifs, std::ofstream ofs) noexcept
        : ifs_(std::move(ifs)), ofs_(std::move(ofs)) {}

    void sort();

private:
    std::vector<std::string> getContent(std::ifstream ifs);

private:
    std::ifstream ifs_;
    std::ofstream ofs_;
    ThreadPool thread_pool_;
};

}

#endif
