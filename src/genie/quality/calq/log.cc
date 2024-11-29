/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/log.h"

#include <iostream>
#include <memory>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

static std::unique_ptr<LogConfiguration> current_config;

// -----------------------------------------------------------------------------

void SetLogging(const LogConfiguration& c) {
  current_config = std::make_unique<LogConfiguration>(c);
}

// -----------------------------------------------------------------------------

LogConfiguration GetLogging() {
  if (!current_config) {
    SetLogging(logging_presets::GetStandard());
  }
  return *current_config;
}

// -----------------------------------------------------------------------------

namespace logging_presets {

// -----------------------------------------------------------------------------

LogConfiguration GetSilent() {
  return {[](const std::string&) {}, [](const std::string&) {}};
}

// -----------------------------------------------------------------------------

LogConfiguration GetStandard() {
  return {[](const std::string& msg) { std::cout << msg << std::endl; },
          [](const std::string& msg) { std::cerr << msg << std::endl; }};
}

// -----------------------------------------------------------------------------

}  // namespace logging_presets

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
