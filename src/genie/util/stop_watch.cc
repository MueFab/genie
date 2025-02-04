/**
 * Copyright 2018-2024 The Genie Authors.
 * @file stop_watch.cc
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Implementation of the Watch utility class for measuring elapsed time.
 *
 * This file contains the implementation of the Watch class, which provides
 * methods to measure elapsed time with the ability to pause, resume, and Reset
 * the timer.
 *
 * @details The Watch class uses std::chrono::steady_clock to measure time
 * intervals and offers functionalities like checking elapsed time, pausing, and
 * resuming the timer.
 */

#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

Watch::Watch() : paused_(false) { Reset(); }

// -----------------------------------------------------------------------------

void Watch::Reset() {
  start_ = std::chrono::steady_clock::now();
  offset_ = 0;
  paused_ = false;
}

// -----------------------------------------------------------------------------

double Watch::Check() const {
  if (paused_) {
    return offset_;
  }
  return static_cast<double>(
             std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - start_)
                 .count()) /
             1e3 +
         offset_;
}

// -----------------------------------------------------------------------------

void Watch::Pause() {
  if (paused_) {
    return;
  }
  paused_ = true;
  offset_ +=
      static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - start_)
                              .count()) /
      1e3;
}

// -----------------------------------------------------------------------------

void Watch::Resume() {
  if (!paused_) {
    return;
  }
  paused_ = false;
  start_ = std::chrono::steady_clock::now();
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
