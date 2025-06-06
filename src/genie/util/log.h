/**
 * Copyright 2018-2024 The Genie Authors.
 * @file log.h
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

#ifndef SRC_GENIE_UTIL_LOG_H_
#define SRC_GENIE_UTIL_LOG_H_

// -----------------------------------------------------------------------------

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A thread-safe singleton class for logging messages.
 *
 * The Logger class provides an interface for logging messages with different
 * severity levels. It supports logging to console and files and ensures
 * thread-safe access for concurrent applications.
 */
class Logger {
 public:
  /**
   * @brief Severity levels for log messages.
   */
  enum class Severity {
    /// Debug messages for detailed information.
    DEBUG = 0,

    /// Informational messages.
    INFO = 1,

    /// Warnings that may indicate potential issues.
    WARNING = 2,

    /// Errors that indicate a failure or problem.
    ERROR = 3
  };

  /**
   * @brief Location levels for log messages.
   */
  enum class LocationLevel {
    /// No location information.
    NONE,

    /// Module-level information.
    MODULE,

    /// Exact file and line number.
    EXACT
  };

  /**
   * @brief Time settings for log messages.
   */
  enum class TimeSetting {
    /// No time information.
    NONE,

    /// Wall-clock time.
    WALLCLOCK,

    /// Runtime since application start.
    RUNTIME,

    /// Both wall-clock and runtime.
    BOTH
  };

  /**
   * @brief Gets the singleton instance of the Logger class.
   *
   * This method provides access to the single instance of the Logger class,
   * ensuring only one instance exists across the application.
   *
   * @return A reference to the Logger instance.
   */
  static Logger& GetInstance();

  /**
   * @brief Logs a message with the specified severity level.
   *
   * This method formats the message with the severity level and writes it to
   * the output stream.
   *
   * @param severity The severity level of the message.
   * @param message The message to log.
   * @param module_name The name of the module logging the message.
   * @param file The source file name (used for exact location).
   * @param line The source line number (used for exact location).
   */
  void Log(Severity severity, const std::string& message,
           const std::string& module_name = "", const std::string& file = "",
           int line = 0);

  /**
   * @brief Sets the output stream for logging.
   *
   * This method allows redirecting log messages to a custom output stream,
   * such as a file.
   *
   * @param stream A pointer to the output stream. Defaults to std::cout.
   */
  void SetOutputStream(std::ostream* stream = &std::cerr);

  /**
   * @brief Configures the logger to control location information.
   *
   * @param level The desired location level.
   */
  void SetLocationLevel(LocationLevel level);

  /**
   * @brief Configures the logger to control time information.
   *
   * @param setting The desired time setting.
   */
  void SetTimeSetting(TimeSetting setting);

  /**
   * @brief Configures the logger to control the severity level.
   *
   * @param level The desired severity level.
   */
  void SetSeverityLevel(Severity level);

  /**
   * @brief Deletes copy constructor and assignment operator.
   *
   * Ensures the singleton cannot be copied or assigned.
   */
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

 private:
  /**
   * @brief Private constructor for the Logger class.
   *
   * The constructor initializes the Logger with the default output stream
   * (std::cout).
   */
  Logger();

  /// The output stream for logging messages.
  std::ostream* output_stream_;

  /// Mutex for thread-safe logging.
  std::mutex log_mutex_;

  /// Controls the level of location info.
  LocationLevel location_level_;

  /// Controls the inclusion of time info.
  TimeSetting time_setting_;

  /// The current severity level for logging.
  Severity severity_level_;

  /// Application start time.
  std::chrono::steady_clock::time_point start_time_;

  /**
   * @brief Logs a message with detailed information.
   *
   * This method formats the message with the severity level, module name,
   * file name, and line number, and writes it to the output stream.
   *
   * @param severity The severity level of the message.
   * @param message The message to log.
   * @param module_name The name of the module logging the message.
   * @param file The source file name.
   * @param line The source line number.
   */
  void LogLine(Severity severity, const std::string& message,
               const std::string& module_name, const std::string& file,
               int line) const;
};

/**
 * @brief Macro to simplify logging with automatic location information.
 *
 * This macro automatically provides the file and line number to the log entry.
 *
 * @param severity The severity level of the message.
 * @param message The log message.
 * @param module_name The module name for context (optional).
 */
#define UTILS_LOG(severity, message)                                        \
  genie::util::Logger::GetInstance().Log(severity, message, kLogModuleName, \
                                         __FILE__, __LINE__)

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_LOG_H_

// -----------------------------------------------------------------------------
