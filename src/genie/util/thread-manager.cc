/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "thread-manager.h"
#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

thread_local size_t ThreadManager::threadID;
thread_local size_t ThreadManager::threadNum;

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::action(size_t id) {
    ThreadManager::threadID = id;
    ThreadManager::threadNum = threads.size();
    try {
        for(const auto& s : source) {
            while (!stopFlag && s->pump(counter++)) {
            }
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw(e);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ThreadManager::ThreadManager(size_t thread_num)
    : counter(0), threads(thread_num), stopFlag(false), abortFlag(false) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ThreadManager::run() {
    size_t ctr = 0;
    for (auto& t : threads) {
        t = std::thread(&ThreadManager::action, this, ctr++);
    }
    for (auto& t : threads) {
        t.join();
    }
    if (!abortFlag) {
        source.front()->dryIn();
    }
    return counter;
}

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::stop(bool abort) {
    abortFlag = abort;
    stopFlag = true;
}

// ---------------------------------------------------------------------------------------------------------------------

ThreadManager::~ThreadManager() { stop(true); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------