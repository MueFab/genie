/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "ordered-lock.h"

// ---------------------------------------------------------------------------------------------------------------------

void OrderedLock::wait(size_t id) {
    std::unique_lock<std::mutex> lock(m);
    if (id == counter) {
        return;
    }
    cond_var.wait(lock, [&]() -> bool { return id == counter; });
}

// ---------------------------------------------------------------------------------------------------------------------

void OrderedLock::finished() {
    {
        std::unique_lock<std::mutex> lock(m);
        counter++;
    }
    cond_var.notify_all();
}

// ---------------------------------------------------------------------------------------------------------------------

OrderedLock::OrderedLock() : counter(0) {}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------