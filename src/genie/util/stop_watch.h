/**
 * Copyright 2018-2024 The Genie Authors.
 * @file stop_watch.h
 * @copyright This file is part of Genie
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Watch utility class for measuring elapsed time.
 *
 * This file contains the declaration of the Watch class, which provides methods
 * to measure elapsed time with the ability to pause, resume, and Reset the
 * timer.
 *
 * @details The Watch class uses std::chrono::steady_clock to measure time
 * intervals and offers functionalities like checking elapsed time, pausing, and
 * resuming the timer.
 */

#ifndef SRC_GENIE_UTIL_STOP_WATCH_H_
#define SRC_GENIE_UTIL_STOP_WATCH_H_

// -----------------------------------------------------------------------------

#include <chrono>  //NOLINT

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A class that measures and tracks elapsed time.
 *
 * The Watch class provides functionality to measure and track elapsed time. It
 * uses the steady clock from the chrono library for accurate time measurements.
 *
 * Example Usage:
 * Watch watch;
 * // Do something
 * double elapsedTime = watch.check();
 *
 * @see std::chrono::steady_clock
 */
class Watch {
  /// When the current period of time started.
  std::chrono::steady_clock::time_point start_;

  /// Elapsed time from previous time periods.
  double offset_{};

  /// If the time is currently measured.
  bool paused_;

 public:
  /**
   * @class Watch
   * @brief A class that measures and tracks elapsed time.
   *
   * The Watch class provides functionality to measure and track elapsed time.
   * It uses the steady clock from the chrono library for accurate time
   * measurements.
   *
   * @see std::chrono::steady_clock
   */
  Watch();

  /**
   * @brief Resets the time measurement.
   *
   * This method resets the time measurement. It sets the start time to the
   * current time and clears the offset time.
   */
  void Reset();

  /**
   * @brief Returns the elapsed time in seconds.
   *
   * This method returns the elapsed time in seconds. If the Watch is
   * currently paused, it returns the accumulated offset time since the Watch
   * was last Reset.
   *
   * @return The elapsed time in seconds.
   */
  [[nodiscard]] double Check() const;

  /**
   * @brief Pauses the time measurement.
   *
   * This method pauses the measurement of time. The elapsed time up to the
   * pause will be counted in the total elapsed time.
   */
  void Pause();

  /**
   * @brief Resumes measuring time after a pause.
   *
   * This method resumes the measurement of time after it has been paused
   * using the `pause()` method. The elapsed time between the pause and the
   * resume will not be counted in the total elapsed time.
   */
  void Resume();
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_STOP_WATCH_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
