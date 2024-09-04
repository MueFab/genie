/**
 * @file stop-watch.cc
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/mitogen/genie for more details.
 *
 * @brief Implementation of the Watch utility class for measuring elapsed time.
 *
 * This file contains the implementation of the Watch class, which provides methods
 * to measure elapsed time with the ability to pause, resume, and reset the timer.
 *
 * @details The Watch class uses std::chrono::steady_clock to measure time intervals
 * and offers functionalities like checking elapsed time, pausing, and resuming the timer.
 */

#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

Watch::Watch() : paused(false) { reset(); }

// ---------------------------------------------------------------------------------------------------------------------

void Watch::reset() {
    start = std::chrono::steady_clock::now();
    offset = 0;
    paused = false;
}

// ---------------------------------------------------------------------------------------------------------------------

double Watch::check() const {
    if (paused) {
        return offset;
    }
    return static_cast<double>(
               std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start)
                   .count()) /
               1e3 +
           offset;
}

// ---------------------------------------------------------------------------------------------------------------------

void Watch::pause() {
    if (paused) {
        return;
    }
    paused = true;
    offset +=
        static_cast<double>(
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()) /
        1e3;
}

// ---------------------------------------------------------------------------------------------------------------------

void Watch::resume() {
    if (!paused) {
        return;
    }
    paused = false;
    start = std::chrono::steady_clock::now();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
