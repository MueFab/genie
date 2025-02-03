/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/stop_watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

Watch::Watch() : paused(false) { Reset(); }

// ---------------------------------------------------------------------------------------------------------------------

void Watch::Reset() {
    start = std::chrono::steady_clock::now();
    offset = 0;
    paused = false;
}

// ---------------------------------------------------------------------------------------------------------------------

double Watch::Check() const {
    if (paused) {
        return offset;
    } else {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() /
                   1e3 +
               offset;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Watch::Pause() {
    if (paused) {
        return;
    }
    paused = true;
    offset +=
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1e3;
}

// ---------------------------------------------------------------------------------------------------------------------

void Watch::Resume() {
    if (!paused) {
        return;
    }
    paused = false;
    start = std::chrono::steady_clock::now();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
