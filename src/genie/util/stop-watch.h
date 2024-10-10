/**
 * @file stop-watch.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Watch utility class for measuring elapsed time.
 *
 * This file contains the declaration of the Watch class, which provides methods
 * to measure elapsed time with the ability to pause, resume, and reset the timer.
 *
 * @details The Watch class uses std::chrono::steady_clock to measure time intervals
 * and offers functionalities like checking elapsed time, pausing, and resuming the timer.
 */

#ifndef SRC_GENIE_UTIL_STOP_WATCH_H_
#define SRC_GENIE_UTIL_STOP_WATCH_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <chrono>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A class that measures and tracks elapsed time.
 *
 * The Watch class provides functionality to measure and track elapsed time. It uses the steady clock from the chrono
 * library for accurate time measurements.
 *
 * Example Usage:
 * Watch watch;
 * // Do something
 * double elapsedTime = watch.check();
 *
 * @see std::chrono::steady_clock
 */
class Watch {
 private:
    std::chrono::steady_clock::time_point start;  //!< @brief When the current period of time started.
    double offset{};                              //!< @brief Elapsed time from previous time periods.
    bool paused;                                  //!< @brief If the time is currently measured.

 public:
    /**
     * @class Watch
     * @brief A class that measures and tracks elapsed time.
     *
     * The Watch class provides functionality to measure and track elapsed time. It uses the steady clock from the
     * chrono library for accurate time measurements.
     *
     * @see std::chrono::steady_clock
     */
    Watch();

    /**
     * @brief Resets the time measurement.
     *
     * This method resets the time measurement. It sets the start time to the current time and clears the offset time.
     */
    void reset();

    /**
     * @brief Returns the elapsed time in seconds.
     *
     * This method returns the elapsed time in seconds. If the Watch is currently paused, it returns the accumulated
     * offset time since the Watch was last reset.
     *
     * @return The elapsed time in seconds.
     */
    [[nodiscard]] double check() const;

    /**
     * @brief Pauses the time measurement.
     *
     * This method pauses the measurement of time. The elapsed time up to the pause will be counted in the total elapsed
     * time.
     */
    void pause();

    /**
     * @brief Resumes measuring time after a pause.
     *
     * This method resumes the measurement of time after it has been paused using the `pause()` method. The elapsed time
     * between the pause and the resume will not be counted in the total elapsed time.
     */
    void resume();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_STOP_WATCH_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
