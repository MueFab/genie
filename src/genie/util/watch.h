#ifndef GENIE_WATCH_H
#define GENIE_WATCH_H

#include <chrono>

namespace genie {
namespace util {
class Watch {
   private:
    std::chrono::high_resolution_clock::time_point start;

   public:
    Watch() { reset(); }

    void reset() { start = std::chrono::high_resolution_clock::now(); }

    double check() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start)
                   .count() /
               1e6;
    }
};
}  // namespace util
}  // namespace genie

#endif  // GENIE_WATCH_H
