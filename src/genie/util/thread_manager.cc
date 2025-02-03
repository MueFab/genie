/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/thread_manager.h"
#include <iostream>
#include <utility>
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

thread_local size_t ThreadManager::threadID;
thread_local size_t ThreadManager::threadNum;

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::Action(size_t id) {
    ThreadManager::threadID = id;
    ThreadManager::threadNum = threads.size();
    try {
        for (const auto& s : source) {
            while (!stopFlag && s->Pump(counter, lock)) {
            }
        }
    } catch (genie::util::Exception& e) {
        std::cerr << e.Msg() << std::endl;
        throw(e);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw(e);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ThreadManager::ThreadManager(size_t thread_num, size_t ctr)
    : counter(ctr), threads(thread_num), stopFlag(false), abortFlag(false) {}

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::SetSource(std::vector<OriginalSource*> src) { source = std::move(src); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ThreadManager::Run() {
    size_t ctr = 0;
    for (auto& t : threads) {
        t = std::thread(&ThreadManager::Action, this, ctr++);
    }
    for (auto& t : threads) {
        t.join();
    }
    if (!abortFlag) {
        source.front()->FlushIn(counter);
    }
    return counter;
}

// ---------------------------------------------------------------------------------------------------------------------

void ThreadManager::Stop(bool abort) {
    abortFlag = abort;
    stopFlag = true;
}

// ---------------------------------------------------------------------------------------------------------------------

ThreadManager::~ThreadManager() { Stop(true); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
