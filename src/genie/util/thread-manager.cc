/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/thread-manager.h"
#include <iostream>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::action(size_t) {
    try {
        for (const auto& s : source) {
            while (!stopFlag && s->pump(counter, lock)) {
            }
        }
    } catch (Exception& e) {
        std::cerr << e.msg() << std::endl;
        throw e;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw e;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ThreadManager::ThreadManager(const size_t thread_num, const size_t ctr)
    : counter(ctr), threads(thread_num), stopFlag(false), abortFlag(false) {}

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::setSource(std::vector<OriginalSource*> src) { source = std::move(src); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ThreadManager::run() {
    size_t ctr = 0;
    for (auto& t : threads) {
        t = std::thread(&ThreadManager::action, this, ctr++);
    }
    for (auto& t : threads) {
        t.join();
    }
    if (!abortFlag) {
        source.front()->flushIn(counter);
    }
    return counter;
}

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::stop(const bool abort) {
    abortFlag = abort;
    stopFlag = true;
}

// ---------------------------------------------------------------------------------------------------------------------

ThreadManager::~ThreadManager() { stop(true); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
