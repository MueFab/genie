/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ORDERED_LOCK_H_
#define SRC_GENIE_UTIL_ORDERED_LOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <condition_variable> //NOLINT
#include <mutex> //NOLINT

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Allows to synchronize concurrent threads that the execution order is enforced
 */
class OrderedLock {
 private:
    size_t counter;                    //!< @brief Which thread is currently allowed to execute the protected section
    std::mutex m;                      //!< @brief Waiting mutex
    std::condition_variable cond_var;  //!< @brief Checks if the current thread is allowed to continue

 public:
    /**
     * @brief initialize
     */
    OrderedLock();

    /**
     * @brief Waits until the current thread / data block is allowed to execute
     * @param id Block / thread identifier of this thread
     */
    void wait(size_t id);

    /**
     * @brief Marks current block / thread as finished and triggers waiting threads to continue if allowed to.
     */
    void finished(size_t length);

    /**
     * @brief Reset the internal counter
     */
    void reset();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ORDERED_LOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
