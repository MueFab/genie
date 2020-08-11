/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/watch.h>
#include <gtest/gtest.h>
#include <thread>

// ---------------------------------------------------------------------------------------------------------------------

TEST(WatchTest, Watch) {
    genie::util::Watch watch;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    watch.pause();
    EXPECT_TRUE(watch.check() > 0.49 && watch.check() < 0.51);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(watch.check() > 0.49 && watch.check() < 0.51);
    watch.resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    watch.pause();
    EXPECT_TRUE(watch.check() > 0.99 && watch.check() < 1.01);
    watch.reset();
    watch.pause();
    EXPECT_TRUE(watch.check() < 0.01);
}

// ---------------------------------------------------------------------------------------------------------------------