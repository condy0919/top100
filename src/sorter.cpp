#include "sorter.hpp"

namespace top100 {

std::vector<std::string> ExternalSorter::getContent(std::ifstream ifs) {
    std::string line;
    std::vector<std::string> ret;
    while (std::getline(ifs, line)) {
        line.shrink_to_fit();
        ret.push_back(std::move(line));
    }
    ret.shrink_to_fit();

    return ret;
}

}
