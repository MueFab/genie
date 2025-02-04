/**
 * Copyright 2018-2024 The Genie Authors.
 * @file log.cc
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Logger class as a thread-safe singleton for logging
 * messages.
 *
 * This file contains the declaration and implementation of the Logger class,
 * which provides a modern thread-safe logging mechanism. It uses the singleton
 * pattern to ensure only one instance is created, accessible globally.
 *
 * @details The Logger class supports logging messages with different severity
 * levels, thread-safe access, and output redirection to various streams such as
 * console or file. It leverages modern C++17 features to provide a robust
 * implementation for logging in multithreaded environments.
 */

#include "genie/util/log.h"
#include <string>
#include <iostream>
#include <iomanip>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

Logger& Logger::GetInstance() {
  static Logger instance;
  return instance;
}

// -----------------------------------------------------------------------------

Logger::Logger()
    : output_stream_(&std::cout),
      location_level_(LocationLevel::MODULE),
      time_setting_(TimeSetting::RUNTIME),
      severity_level_(Severity::INFO),
      start_time_(std::chrono::steady_clock::now()) {}

// -----------------------------------------------------------------------------

void Logger::LogLine(const Severity severity, const std::string& message,
                     const std::string& module_name, const std::string& file,
                     const int line) const {
  if (severity < severity_level_) return;

  // Severity
  switch (severity) {
    case Severity::DEBUG:
      *output_stream_ << "[DEBUG";
      break;
    case Severity::INFO:
      *output_stream_ << "[INFO";
      break;
    case Severity::WARNING:
      *output_stream_ << "[WARNING";
      break;
    case Severity::ERROR:
      *output_stream_ << "[ERROR";
      break;
  }

  // Time info
  if (time_setting_ != TimeSetting::NONE) {
    std::ostringstream time_stream;
    if (time_setting_ == TimeSetting::WALLCLOCK ||
        time_setting_ == TimeSetting::BOTH) {
      const auto now = std::chrono::system_clock::now();
      const auto now_ms =
          std::chrono::time_point_cast<std::chrono::milliseconds>(now);
      const auto duration = now_ms.time_since_epoch();
      const auto millis =
          std::chrono::duration_cast<std::chrono::milliseconds>(duration)
              .count() %
          1000;
      const std::time_t now_c = std::chrono::system_clock::to_time_t(now);
      time_stream << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S")
                  << "." << std::setw(3) << std::setfill('0') << millis;
    }
    if (time_setting_ == TimeSetting::RUNTIME ||
        time_setting_ == TimeSetting::BOTH) {
      const auto runtime = std::chrono::steady_clock::now() - start_time_;
      const auto runtime_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(runtime);
      const auto runtime_seconds = runtime_ms.count() / 1000;
      const auto runtime_millis = runtime_ms.count() % 1000;
      time_stream << std::setw(6) << std::setfill(' ') << runtime_seconds << "."
                  << std::setw(3) << std::setfill('0') << runtime_millis << "s";
    }
    *output_stream_ << ", " << time_stream.str();
  }

  // Location info
  if (location_level_ != LocationLevel::NONE) {
    *output_stream_ << ", ";
    if (location_level_ == LocationLevel::MODULE && !module_name.empty()) {
      *output_stream_ << module_name;
    } else if (location_level_ == LocationLevel::EXACT && !file.empty() &&
               line > 0) {
      *output_stream_ << file << ":" << line;
    }
  }
  *output_stream_ << "]: ";

  // Message
  *output_stream_ << message << std::endl;
}

void Logger::Log(const Severity severity, const std::string& message,
                 const std::string& module_name, const std::string& file,
                 const int line) {
  std::lock_guard lock(log_mutex_);

  std::istringstream stream(message);
  std::string cur_line;

  while (std::getline(stream, cur_line)) {
    LogLine(severity, cur_line, module_name, file, line);
  }
}

// -----------------------------------------------------------------------------

void Logger::SetSeverityLevel(const Severity level) {
  std::lock_guard lock(log_mutex_);
  severity_level_ = level;
}

// -----------------------------------------------------------------------------

void Logger::SetOutputStream(std::ostream* stream) {
  std::lock_guard lock(log_mutex_);
  output_stream_ = stream ? stream : &std::cerr;
}

// -----------------------------------------------------------------------------

void Logger::SetLocationLevel(const LocationLevel level) {
  std::lock_guard lock(log_mutex_);
  location_level_ = level;
}

// -----------------------------------------------------------------------------

void Logger::SetTimeSetting(const TimeSetting setting) {
  std::lock_guard lock(log_mutex_);
  time_setting_ = setting;
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
