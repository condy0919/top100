#include "counter.hpp"

namespace top100 {

void Counter::count() {
    std::size_t cnt = 0;
    std::string last;

    while (true) {
        const auto& [avail, s] = prefetched_.get();
        if (!avail) {
            break;
        }

        if (s != last) {
            if (cnt) {
                top100_.push(std::make_pair(cnt, std::move(last)));
            }

            cnt = 1;
            last = s;
        } else {
            ++cnt;
        }
    }

    if (cnt) {
        top100_.push(std::make_pair(cnt, std::move(last)));
    }
}

std::vector<std::pair<std::size_t, std::string>> Counter::getResult() {
    std::vector<std::pair<std::size_t, std::string>> ret;
    while (!top100_.empty()) {
        ret.push_back(std::move(top100_.top()));
        top100_.pop();
    }
    return ret;
}

}
