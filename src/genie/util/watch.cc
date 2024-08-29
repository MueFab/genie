/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/watch.h"

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
    } else {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() /
                   1e3 +
               offset;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Watch::pause() {
    if (paused) {
        return;
    }
    paused = true;
    offset +=
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1e3;
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
