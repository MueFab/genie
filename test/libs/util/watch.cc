/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include <genie/util/stop_watch.h>
#include <gtest/gtest.h>

#include <thread>

// -----------------------------------------------------------------------------
void active_sleep(const int64_t milliseconds) {
  const auto start = std::chrono::steady_clock::now();
  while (std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now() - start)
             .count() < milliseconds) {
  }
}

// -----------------------------------------------------------------------------
TEST(WatchTest, Watch) {
  genie::util::Watch watch;
  active_sleep(500);
  watch.Pause();
  EXPECT_GT(watch.Check(), 0.49);
  EXPECT_LT(watch.Check(), 0.51);
  active_sleep(500);
  EXPECT_GT(watch.Check(), 0.49);
  EXPECT_LT(watch.Check(), 0.51);
  watch.Resume();
  active_sleep(500);
  watch.Pause();
  EXPECT_GT(watch.Check(), 0.99);
  EXPECT_LT(watch.Check(), 1.01);
  watch.Reset();
  watch.Pause();
  EXPECT_LT(watch.Check(), 0.01);
}

// -----------------------------------------------------------------------------