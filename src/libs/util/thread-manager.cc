/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "thread-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::action() {
    while (!stopFlag && source->pump(counter++)) {
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ThreadManager::ThreadManager(size_t thread_num, OriginalSource* src)
    : counter(0), threads(thread_num), stopFlag(false), abortFlag(false), source(src) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ThreadManager::run() {
    for (auto& t : threads) {
        t = std::thread(&ThreadManager::action, this);
    }
    for (auto& t : threads) {
        t.join();
    }
    if (!abortFlag) {
        source->dryIn();
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
// ---------------------------------------------------------------------------------------------------------------------