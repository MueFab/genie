/**
 * Copyright 2018-2024 The Genie Authors.
 * @file log.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of logging utility functions and structures for the CALQ
 * quality value compression library.
 *
 * This file provides the declaration of the `LogConfiguration` structure and
 * functions for configuring and controlling logging behavior within the CALQ
 * library. It enables the redirection of Log output to custom destinations
 * (such as standard output, standard error, or custom logging systems).
 *
 * @details The `LogConfiguration` structure defines function pointers for
 * logging to standard output and error streams. Using the `setLogging` and
 * `getLogging` functions, the Log configuration can be changed at runtime. The
 * `loggingPresets` namespace provides predefined configurations for typical use
 * cases, such as silent or standard logging.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_LOG_H_
#define SRC_GENIE_QUALITY_CALQ_LOG_H_

#include <functional>
#include <string>

// -----------------------------------------------------------------------------
namespace genie::quality::calq {

// -----------------------------------------------------------------------------
/**
 * @brief Configuration structure for Log output streams.
 *
 * The `LogConfiguration` structure defines two function pointers for handling
 * Log output: one for standard output (`standardOut`) and one for standard
 * error (`errorOut`). Each function takes a `std::string` as an input,
 * representing the Log message to be output. By assigning different functions
 * to these pointers, the user can customize the logging behavior, such as
 * redirecting logs to files, disabling logs, or using custom logging systems.
 */
struct LogConfiguration {
  std::function<void(const std::string&)>
      standard_out;  //!< @brief Function to handle standard output Log
                     //!< messages.
  std::function<void(const std::string&)>
      error_out;  //!< @brief Function to handle standard error Log messages.
};

/**
 * @brief Sets the current logging configuration for the CALQ library.
 *
 * This function sets the global logging configuration to the provided
 * configuration `c`. The configuration will determine how Log messages are
 * output in the library (e.g., to standard output, standard error, or custom
 * logging destinations).
 *
 * @param c The new logging configuration to apply.
 */
void SetLogging(const LogConfiguration& c);

/**
 * @brief Retrieves the current logging configuration.
 *
 * @return The current `LogConfiguration` structure, which defines the current
 * Log output settings.
 */
LogConfiguration GetLogging();

// -----------------------------------------------------------------------------
/**
 * @brief Namespace containing predefined logging configurations.
 *
 * The `loggingPresets` namespace provides a set of predefined
 * `LogConfiguration` objects for common logging use cases. Users can select
 * from these presets to quickly configure the logging behavior of the CALQ
 * library without needing to define custom configurations.
 */
namespace logging_presets {

// -----------------------------------------------------------------------------
/**
 * @brief Retrieves a `LogConfiguration` that disables all logging.
 *
 * The "silent" configuration disables all logging by setting the Log handlers
 * to empty functions. This is useful when the user wants to completely suppress
 * Log output, such as for performance reasons or when running in production
 * environments where logs are not needed.
 *
 * @return A `LogConfiguration` structure that suppresses all standard and error
 * Log output.
 */
LogConfiguration GetSilent();

/**
 * @brief Retrieves a `LogConfiguration` that logs to `std::cout` and
 * `std::cerr`.
 *
 * The "standard" configuration directs standard output Log messages to
 * `std::cout` and standard error messages to `std::cerr`. This is the default
 * logging behavior and is suitable for most applications where console output
 * is acceptable.
 *
 * @return A `LogConfiguration` structure that logs to standard output and error
 * streams.
 */
LogConfiguration GetStandard();

// -----------------------------------------------------------------------------
}  // namespace logging_presets

// -----------------------------------------------------------------------------
}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_QUALITY_CALQ_LOG_H_
