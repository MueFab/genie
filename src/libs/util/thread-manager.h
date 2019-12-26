/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_THREAD_MANAGER_H
#define GENIE_THREAD_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>
#include "original-source.h"

/**
 * @brief Allows to run the genie pipeline with multiple threads
 */
class ThreadManager {
   private:
    std::atomic<uint32_t> counter;     //!< @brief Identifier for next block
    std::vector<std::thread> threads;  //!< @brief Storage for all the threads
    std::atomic<bool> stopFlag;        //!< @brief Threads will stop after current block when set
    bool abortFlag;                    //!< @brief The dryIn() signal will be skipped after stop(). Used with stopFlag
    OriginalSource* source;            //!< @brief Where to enter the pipeline

    /**
     * @brief Threading code
     */
    void action();

   public:
    /**
     * @brief Constructs a new ThreadManager
     * @param thread_num How many threads ro start
     * @param src Where to enter the pipeline
     */
    ThreadManager(size_t thread_num, OriginalSource* src);

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

#endif  // GENIE_THREAD_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------