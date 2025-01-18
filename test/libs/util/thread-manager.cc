/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/thread_manager.h"
#include <gtest/gtest.h>

#include <future>

// -----------------------------------------------------------------------------
class ThreadManagerDummySource final : public genie::util::OriginalSource {
  size_t id_;
  size_t counter_;
  int64_t flushpos_;

 public:
  bool Pump(uint64_t& t_id, std::mutex& lock) override {
    std::lock_guard guard(lock);

    // Simulate an access and different data sizes
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (id_ == 1 && counter_ == 10) {
      return false;
    }
    if (id_ == 2 && counter_ == 5) {
      return false;
    }
    t_id++;
    counter_++;
    return true;
  }
  void FlushIn(uint64_t& pos) override { flushpos_ = pos; }

  explicit ThreadManagerDummySource(const size_t id)
      : id_(id), counter_(0), flushpos_(-1) {}

  size_t GetCounter() const { return counter_; }

  int64_t GetFlushpos() const { return flushpos_; }
};

// -----------------------------------------------------------------------------
TEST(ThreadManagerTest, run) {
  genie::util::ThreadManager manager(4);
  ThreadManagerDummySource source1(1);
  ThreadManagerDummySource source2(2);
  const std::vector<genie::util::OriginalSource*> srcvec = {&source1, &source2};
  manager.SetSource(srcvec);
  EXPECT_EQ(manager.Run(), 15);
  EXPECT_EQ(source1.GetCounter(), 10);
  EXPECT_EQ(source2.GetCounter(), 5);
  EXPECT_EQ(source1.GetFlushpos(), 15);
  EXPECT_EQ(source2.GetFlushpos(), -1);
}

// -----------------------------------------------------------------------------
TEST(ThreadManagerTest, stop) {
  genie::util::ThreadManager manager(4);
  ThreadManagerDummySource source1(3);
  const std::vector<genie::util::OriginalSource*> srcvec = {&source1};
  manager.SetSource(srcvec);
  std::future<uint64_t> f2 =
      std::async(std::launch::async, [&] { return manager.Run(); });

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  manager.Stop(false);
  f2.wait();
  EXPECT_GT(f2.get(), 0);
  EXPECT_GT(source1.GetCounter(), 0);
  EXPECT_GT(source1.GetFlushpos(), -1);
}

// -----------------------------------------------------------------------------
TEST(ThreadManagerTest, abort) {
  genie::util::ThreadManager manager(4);
  ThreadManagerDummySource source1(3);
  const std::vector<genie::util::OriginalSource*> srcvec = {&source1};
  manager.SetSource(srcvec);
  std::future<uint64_t> f2 =
      std::async(std::launch::async, [&] { return manager.Run(); });

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  manager.Stop(true);
  f2.wait();
  EXPECT_GT(f2.get(), 0);
  EXPECT_GT(source1.GetCounter(), 0);
  EXPECT_EQ(source1.GetFlushpos(), -1);
}

// -----------------------------------------------------------------------------