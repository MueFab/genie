/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_WATCH_H_
#define SRC_GENIE_UTIL_WATCH_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <chrono>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Utility class to measure time.
 */
class Watch {
 private:
    std::chrono::steady_clock::time_point start;  //!< @brief When the current period of time started.
    double offset{};                              //!< @brief Elapsed time from previous time periods.
    bool paused;                                  //!< @brief If the time is currently measured.

 public:
    /**
     *  @brief Default initialize.
     */
    Watch();

    /**
     *  @brief Reset the watch to the default state, measurements are stopped and time set to zero.
     */
    void reset();

    /**
     * @brief
     * @return Elapsed time in all periods where the watch was active.
     */
    double check() const;

    /**
     * @brief Pause the current measurement, time elapsed until resume() is called won't be counted.
     */
    void pause();

    /**
     * @brief Active watch again after a previous call to pause().
     */
    void resume();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_WATCH_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
