/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "watch.h"

namespace genie {
namespace util {

Watch::Watch() { reset(); }

void Watch::reset() {
    start = std::chrono::high_resolution_clock::now();
    offset = 0;
    paused = false;
}

double Watch::check() const {
    if (paused) {
        return offset;
    } else {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start)
                       .count() /
                   1e6 +
               offset;
    }
}

void Watch::pause() {
    if (paused) {
        return;
    }
    paused = true;
    offset += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start)
                  .count() /
              1e6;
}

void Watch::resume() {
    if (!paused) {
        return;
    }
    paused = false;
    start = std::chrono::high_resolution_clock::now();
}
}  // namespace util
}  // namespace genie