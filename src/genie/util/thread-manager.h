/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_THREAD_MANAGER_H_
#define SRC_GENIE_UTIL_THREAD_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include "genie/util/original-source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Allows to run the genie pipeline with multiple threads.
 */
class ThreadManager {
 private:
    uint64_t counter;                  //!< @brief Identifier for next block.
    std::vector<std::thread> threads;  //!< @brief Storage for all the threads.
    std::atomic<bool> stopFlag;        //!< @brief Threads will stop after current block when set.
    bool abortFlag;                    //!< @brief The flushIn signal will be skipped after stop(). Used with stopFlag.
    std::vector<OriginalSource*> source;  //!< @brief Entry points for the pipeline.
    std::mutex lock;                      //!< @brief Mutex protecting the counter variable.

    /**
     * @brief Threading code.
     */
    void action(size_t id);

 public:
    static thread_local size_t threadID;   //!< @brief Each thread will see its own ID here
    static thread_local size_t threadNum;  //!< @brief Each thread will see the number of threads here.

    /**
     * @brief
     * @param thread_num
     * @param counter
     */
    explicit ThreadManager(size_t thread_num, size_t counter = 0);

    /**
     * @brief Set the entry points to the pipeline.
     * @param src Vector of entry points.
     */
    void setSource(std::vector<OriginalSource*> src);

    /**
     * @brief Executes the complete genie pipeline until no input data left
     * @return The total number of blocks processed until the end
     */
    uint64_t run();

    /**
     * @brief The genie pipeline will stop after the blocks currently in the pipeline are processed.
     * @param abort If true, genie will not try to finish the output data to be valid, but will exit as fast as
     * possible.
     */
    void stop(bool abort);

    /**
     * @brief Stop pipeline if running and clean up
     */
    ~ThreadManager();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_THREAD_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
