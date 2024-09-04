/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/stop-watch.h>
#include <gtest/gtest.h>
#include <thread>

// ---------------------------------------------------------------------------------------------------------------------

void active_sleep(int64_t milliseconds) {
    auto start = std::chrono::steady_clock::now();
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < milliseconds) {
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(WatchTest, Watch) {
    genie::util::Watch watch;
    active_sleep(500);
    watch.pause();
    EXPECT_GT(watch.check(), 0.49);
    EXPECT_LT(watch.check(), 0.51);
    active_sleep(500);
    EXPECT_GT(watch.check(), 0.49);
    EXPECT_LT(watch.check(), 0.51);
    watch.resume();
    active_sleep(500);
    watch.pause();
    EXPECT_GT(watch.check(), 0.99);
    EXPECT_LT(watch.check(), 1.01);
    watch.reset();
    watch.pause();
    EXPECT_LT(watch.check() ,0.01);
}

// ---------------------------------------------------------------------------------------------------------------------