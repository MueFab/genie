/**
* Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_BARRIER_H_
#define SRC_GENIE_READ_SPRING_BARRIER_H_

#include <mutex>
#include <condition_variable>

namespace genie::read::spring {
class Barrier {
 public:
  explicit Barrier(const int num_threads)
      : num_threads_(num_threads), count_(0), generation_(0) {}

  void wait() {
    std::unique_lock lock(mutex_);
    int current_generation = generation_;

    if (++count_ == num_threads_) {
      // Last thread to arrive at the barrier
      count_ = 0;
      generation_++;
      cond_.notify_all();
    } else {
      // Wait for all threads to reach the barrier
      cond_.wait(lock, [this, current_generation] {
        return generation_ != current_generation;
      });
    }
  }

 private:
  std::mutex mutex_;
  std::condition_variable cond_;
  int num_threads_;
  int count_;
  int generation_;
};
}  // namespace genie::read::spring

#endif  // SRC_GENIE_READ_SPRING_BARRIER_H_
