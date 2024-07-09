#ifndef TEST_CONFORMANCE_HELPERS_H
#define TEST_CONFORMANCE_HELPERS_H

#include <string>
#include <chrono>
#include <cstddef>


class MeasureTime {
public:
    MeasureTime() { start = std::chrono::high_resolution_clock::now(); }
    ~MeasureTime() {
        using namespace std::chrono_literals;
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        auto sec = diff / 1s;
        auto milli = diff / 1ms;
        auto minutes = diff / 1min;
        auto millisec = milli - sec * 1000;
        auto secmin = sec - minutes * 60;
        std::cerr << "Total geno Time(ns): " << minutes << " min " << secmin << " sec " << millisec << " millisec" << '\n';
    }

private:
    std::chrono::high_resolution_clock::time_point start;
};

namespace util_tests {

std::string exec(const std::string &cmd);

}  // namespace util_tests

#endif  // TEST_CONFORMANCE_HELPERS_H
