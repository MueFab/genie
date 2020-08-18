/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/thread-manager.h>
#include <gtest/gtest.h>
#include <future>

// ---------------------------------------------------------------------------------------------------------------------

class ThreadManager_DummySource : public genie::util::OriginalSource {
   private:
    size_t id;
    size_t counter;
    int64_t flushpos;
   public:
    virtual bool pump(uint64_t& t_id, std::mutex& lock) override {
        std::lock_guard<std::mutex> guard(lock);

        // Simulate an access and different data sizes
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if(id == 1 && counter == 10) {
            return false;
        }
        if(id == 2 && counter == 5) {
            return false;
        }
        t_id++;
        counter++;
        return true;
    }
    virtual void flushIn(uint64_t& pos) override {
        flushpos = pos;
    }

    explicit ThreadManager_DummySource(size_t _id) : id(_id), counter(0), flushpos(-1) {
    }

    size_t getCounter() const {
        return counter;
    }

    int64_t getFlushpos() const {
        return flushpos;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

TEST(ThreadManagerTest, run) {
    genie::util::ThreadManager manager(4);
    ThreadManager_DummySource source1(1);
    ThreadManager_DummySource source2(2);
    std::vector<genie::util::OriginalSource*> srcvec = {&source1, &source2};
    manager.setSource(srcvec);
    EXPECT_EQ(manager.run(), 15);
    EXPECT_EQ(source1.getCounter(), 10);
    EXPECT_EQ(source2.getCounter(), 5);
    EXPECT_EQ(source1.getFlushpos(), 15);
    EXPECT_EQ(source2.getFlushpos(), -1);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ThreadManagerTest, stop) {
    genie::util::ThreadManager manager(4);
    ThreadManager_DummySource source1(3);
    std::vector<genie::util::OriginalSource*> srcvec = {&source1};
    manager.setSource(srcvec);
    std::future<uint64_t> f2 = std::async(std::launch::async, [&]{ return manager.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    manager.stop(false);
    f2.wait();
    EXPECT_GT(f2.get(), 0);
    EXPECT_GT(source1.getCounter(), 0);
    EXPECT_GT(source1.getFlushpos(), -1);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ThreadManagerTest, abort) {
    genie::util::ThreadManager manager(4);
    ThreadManager_DummySource source1(3);
    std::vector<genie::util::OriginalSource*> srcvec = {&source1};
    manager.setSource(srcvec);
    std::future<uint64_t> f2 = std::async(std::launch::async, [&]{ return manager.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    manager.stop(true);
    f2.wait();
    EXPECT_GT(f2.get(), 0);
    EXPECT_GT(source1.getCounter(), 0);
    EXPECT_EQ(source1.getFlushpos(), -1);
}

// ---------------------------------------------------------------------------------------------------------------------