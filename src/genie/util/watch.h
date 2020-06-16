#ifndef GENIE_WATCH_H
#define GENIE_WATCH_H

#include <chrono>

namespace genie {
namespace util {
class Watch {
   private:
    std::chrono::high_resolution_clock::time_point start;
    double offset{};
    bool paused;

   public:
    Watch() { reset(); }

    void reset() {
        start = std::chrono::high_resolution_clock::now();
        offset = 0;
        paused = false;
    }

    double check() const {
        if (paused) {
            return offset;
        } else {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() -
                                                                         start)
                           .count() /
                       1e6 +
                   offset;
        }
    }

    void pause() {
        if (paused) {
            return;
        }
        paused = true;
        offset +=
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start)
                .count() /
            1e6;
    }

    void resume() {
        if (!paused) {
            return;
        }
        paused = false;
        start = std::chrono::high_resolution_clock::now();
    }
};
}  // namespace util
}  // namespace genie

#endif  // GENIE_WATCH_H
