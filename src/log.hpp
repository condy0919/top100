#ifndef TOP100_LOG_HPP_
#define TOP100_LOG_HPP_

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <unistd.h>

namespace top100 {

class Logger {
public:
    static Logger getInstance() {
        static Logger ins;
        return ins;
    }

    void put(char c) noexcept {
        buf_[buf_len_++] = c;
    }

    void put(double d) {
        char b[16];

        const std::size_t l = std::snprintf(b, sizeof(b), "%lf", d);
        std::memcpy(buf_ + buf_len_, b, l);

        buf_len_ += l;
    }

    template <typename T,
              std::enable_if_t<std::is_integral_v<T>, int> = 0>
    void put(T x) {
        static const char* digits = "9876543210123456789";
        static const char* zero = digits + 9;

        const bool neg = x < 0;

        char b[32];
        std::size_t l = 0;

        do {
            b[l++] = zero[x % 10];
            x /= 10;
        } while (x);

        if (neg) {
            b[l++] = '-';
        }

        std::reverse(b, b + l);
        std::memcpy(buf_ + buf_len_, b, l);

        buf_len_ += l;
    }

    void put(const char* s, std::size_t sz) {
        std::memcpy(buf_ + buf_len_, s, sz);
        buf_len_ += sz;
    }

    void put(const char* s) {
        put(s, std::strlen(s));
    }

    void put(bool b) {
        put(static_cast<char>('0' + b));
    }

    template <typename... Args>
    void log(const char* file, unsigned long long line, Args&&... args) {
        put(file);
        put(':');
        put(' ');
        put(line);

        int swallow[] = {
            (put(std::forward<Args>(args)), 1)...
        };
        (void)swallow;

        flush();
    }

    void flush() {
        write(STDERR_FILENO, &buf_, buf_len_);
        buf_len_ = 0;
    }

private:
    static char buf_[4096];
    static std::size_t buf_len_;
};

}

#define LOG(...)                                                            \
    do {                                                                    \
        top100::Logger::getInstance().log(__FILE__, __LINE__, __VA_ARGS__); \
    } while (false)

#endif
