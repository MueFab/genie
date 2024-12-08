/**
 * Copyright 2018-2024 The Genie Authors.
 * @file thread_manager.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the ThreadManager class for managing multithreaded
 * execution of the GENIE pipeline.
 *
 * This file contains the declaration of the `ThreadManager` class, which
 * facilitates the execution of the GENIE pipeline using multiple threads. It
 * provides methods for setting up pipeline entry points, running the pipeline
 * in parallel, and safely stopping the execution when needed.
 *
 * @details The `ThreadManager` class is designed to optimize the processing of
 * the GENIE pipeline by distributing the workload across multiple threads. It
 * supports graceful stopping of the pipeline, ensuring that all currently
 * processed blocks are completed before termination, as well as an immediate
 * abort mode for faster shutdown.
 */

#ifndef SRC_GENIE_UTIL_THREAD_MANAGER_H_
#define SRC_GENIE_UTIL_THREAD_MANAGER_H_

// -----------------------------------------------------------------------------

#include <atomic>
#include <mutex>   //NOLINT
#include <thread>  //NOLINT
#include <vector>

#include "genie/util/original_source.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Manages multithreaded execution of the GENIE pipeline.
 *
 * The `ThreadManager` class allows for running the GENIE pipeline in a
 * multithreaded environment. It provides functionality for launching multiple
 * threads, tracking the progress of each thread, and handling controlled
 * shutdown of the pipeline. The class ensures that all threads are synchronized
 * and that the pipeline can be safely stopped when required.
 */
class ThreadManager {
  uint64_t
      counter_;  //!< @brief Counter for tracking the next block identifier.
  std::vector<std::thread>
      threads_;  //!< @brief Storage for all active threads in the pipeline.
  std::atomic<bool>
      stop_flag_;    //!< @brief Indicates whether the threads should stop after
                     //!< completing current blocks.
  bool abort_flag_;  //!< @brief If true, signals to skip final data flushing
                     //!< and terminate immediately.
  std::vector<OriginalSource*>
      source_;  //!< @brief Entry points for feeding data into the pipeline.
  std::mutex lock_;  //!< @brief Mutex for synchronizing access to the
                     //!< `counter` variable.

  /**
   * @brief Executes the main logic for each thread in the pipeline.
   *
   * This function is executed by each thread in the `threads` vector. It
   * continuously processes data from the assigned entry points until the stop
   * or abort flag is set. Each thread increments the `counter` variable to
   * uniquely identify the data blocks being processed.
   *
   * @param id The unique identifier for the thread (used for debugging and
   * logging).
   */
  void Action(size_t id);

 public:
  /**
   * @brief Constructs a `ThreadManager` object with a specified number of
   * threads.
   *
   * Initializes the `ThreadManager` with the given number of threads and sets
   * the initial block identifier (`counter`) to the specified value.
   *
   * @param thread_num The number of threads to be managed by the
   * `ThreadManager`.
   * @param ctr The initial block identifier for the pipeline (default is 0).
   */
  explicit ThreadManager(size_t thread_num, size_t ctr = 0);

  /**
   * @brief Sets the entry points for feeding data into the pipeline.
   *
   * This function configures the `OriginalSource` objects that serve as the
   * entry points for the pipeline. These sources will provide the initial
   * data chunks that are processed by the threads managed by the
   * `ThreadManager`.
   *
   * @param src A vector of pointers to the `OriginalSource` objects.
   */
  void SetSource(std::vector<OriginalSource*> src);

  /**
   * @brief Runs the GENIE pipeline using multiple threads.
   *
   * This function launches all the threads in the `ThreadManager` and starts
   * the execution of the pipeline. It blocks until all threads have completed
   * processing their assigned data chunks.
   *
   * @return The total number of blocks processed by the pipeline.
   */
  uint64_t Run();

  /**
   * @brief Stops the GENIE pipeline after completing the currently processed
   * blocks.
   *
   * This function sets the `stopFlag`, indicating that the threads should
   * complete the current blocks and then terminate. If `abort` is set to
   * true, the function skips final data flushing and terminates immediately.
   *
   * @param abort If true, the pipeline will terminate immediately without
   * flushing data.
   */
  void Stop(bool abort);

  /**
   * @brief Stops the pipeline if running and cleans up resources.
   *
   * The destructor ensures that all active threads are properly joined and
   * resources are cleaned up. It prevents potential data corruption or memory
   * leaks in case the `ThreadManager` object goes out of scope while threads
   * are still running.
   */
  ~ThreadManager();
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_THREAD_MANAGER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
