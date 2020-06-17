/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_THREAD_MANAGER_H
#define GENIE_THREAD_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <atomic>
#include <thread>
#include <utility>
#include <vector>
#include "original-source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Allows to run the genie pipeline with multiple threads
 */
class ThreadManager {
   private:
    size_t counter;                    //!< @brief Identifier for next block
    std::vector<std::thread> threads;  //!< @brief Storage for all the threads
    std::atomic<bool> stopFlag;        //!< @brief Threads will stop after current block when set
    bool abortFlag;  //!< @brief The flushIn(size_t& pos) signal will be skipped after stop(). Used with stopFlag
    std::vector<OriginalSource*> source;  //!< @brief Where to enter the pipeline
    std::mutex lock;

    /**
     * @brief Threading code
     */
    void action(size_t id);

   public:
    static thread_local size_t threadID;
    static thread_local size_t threadNum;
    static thread_local ThreadManager* localManager;

    /**
     * @brief Constructs a new ThreadManager
     * @param thread_num How many threads ro start
     * @param src Where to enter the pipeline
     */
    ThreadManager(size_t thread_num, size_t counter = 0);

    void setSource(std::vector<OriginalSource*> src);

    /**
     * @brief Executes the complete genie pipeline until no input data left
     * @return The total number of blocks processed until the end
     */
    uint32_t run();

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

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_THREAD_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------