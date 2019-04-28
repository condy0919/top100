#include "counter.hpp"

namespace top100 {

void Counter::count(std::string s) {
    if (s != last_) {
        if (cnt_) {
            top100_.push(std::make_pair(cnt_, std::move(last_)));
        }

        cnt_ = 1;
        last_ = s;
    } else {
        ++cnt_;
    }
}

void Counter::flush() {
    if (cnt_) {
        top100_.push(std::make_pair(cnt_, std::move(last_)));
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
