/**
 * Copyright 2018-2024 The Genie Authors.
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @file thread_manager.cc
 * @brief Implementation of the ThreadManager class for managing and executing
 * threads.
 *
 * This file contains the implementation of the ThreadManager class, which
 * provides functionalities to manage multiple threads, execute actions in
 * parallel, and handle exceptions that may occur during thread execution.
 *
 * @details The ThreadManager class allows setting a source of tasks, running
 * the tasks across multiple threads, and stopping the execution with an option
 * to abort. It ensures proper handling of exceptions and provides mechanisms to
 * stop and join threads.
 */

#include "genie/util/thread_manager.h"

#include <iostream>
#include <utility>

#include "genie/util/log.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "ThreadManager";

namespace genie::util {

// -----------------------------------------------------------------------------

void ThreadManager::Action(size_t) {
  try {
    for (const auto& s : source_) {
      while (!stop_flag_ && s->Pump(counter_, lock_)) {
      }
    }
  } catch (Exception& e) {
    GENIE_LOG(util::Logger::Severity::ERROR, e.Msg());
    throw;
  } catch (std::exception& e) {
    GENIE_LOG(util::Logger::Severity::ERROR, e.what());
    throw;
  }
}

// -----------------------------------------------------------------------------

ThreadManager::ThreadManager(const size_t thread_num, const size_t ctr)
    : counter_(ctr),
      threads_(thread_num),
      stop_flag_(false),
      abort_flag_(false) {}

// -----------------------------------------------------------------------------

void ThreadManager::SetSource(std::vector<OriginalSource*> src) {
  source_ = std::move(src);
}

// -----------------------------------------------------------------------------

uint64_t ThreadManager::Run() {
  size_t ctr = 0;
  for (auto& t : threads_) {
    t = std::thread(&ThreadManager::Action, this, ctr++);
  }
  for (auto& t : threads_) {
    t.join();
  }
  if (!abort_flag_) {
    source_.front()->FlushIn(counter_);
  }
  return counter_;
}

// -----------------------------------------------------------------------------

void ThreadManager::Stop(const bool abort) {
  abort_flag_ = abort;
  stop_flag_ = true;
}

// -----------------------------------------------------------------------------

ThreadManager::~ThreadManager() { Stop(true); }

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
