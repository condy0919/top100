#ifndef TOP100_PREFETCHED_HPP_
#define TOP100_PREFETCHED_HPP_

#include <fstream>
#include <string>
#include <tuple>
#include <queue>

namespace top100 {

class Prefetched {
public:
    Prefetched() = default;
    Prefetched(std::ifstream ifs) noexcept : ifs_(std::move(ifs)) {}

    Prefetched(Prefetched&& rhs) noexcept
        : ifs_(std::move(rhs.ifs_)), fetched_(std::move(rhs.fetched_)) {}

    Prefetched& operator=(Prefetched&& rhs) noexcept {
        ifs_ = std::move(rhs.ifs_);
        fetched_ = std::move(rhs.fetched_);

        return *this;
    }

    Prefetched(const Prefetched&) = delete;
    Prefetched& operator=(const Prefetched&) = delete;

    std::tuple<bool, const std::string&> peak() {
        fetch();
        if (fetched_.empty()) {
            return std::make_tuple(false, "");
        }

        return std::make_tuple(true, fetched_.front());
    }

    std::tuple<bool, std::string> get() {
        fetch();
        if (fetched_.empty()) {
            return std::make_tuple(false, "");
        }

        std::string ret = std::move(fetched_.front());
        fetched_.pop();
        return std::make_tuple(true, std::move(ret));
    }

private:
    void fetch() {
        const std::size_t PREFETCH_LINES = 100;
        if (fetched_.empty()) {
            std::string line;
            for (std::size_t i = 0; i < PREFETCH_LINES && std::getline(ifs_, line); ++i) {
                line.shrink_to_fit();

                fetched_.push(std::move(line));
            }
        }
    }

private:
    std::ifstream ifs_;
    std::queue<std::string> fetched_;
};

}


#endif
