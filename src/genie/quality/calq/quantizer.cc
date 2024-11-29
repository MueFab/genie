/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/quantizer.h"

// -----------------------------------------------------------------------------

#include <map>
#include <sstream>
#include <string>

// -----------------------------------------------------------------------------

#include "genie/quality/calq/error_exception_reporter.h"
#include "genie/quality/calq/log.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

Quantizer::Quantizer() = default;

// -----------------------------------------------------------------------------

Quantizer::Quantizer(const std::map<size_t, int>& inverse_lut)
    : inverse_lut_(inverse_lut) {}

// -----------------------------------------------------------------------------

Quantizer::~Quantizer() = default;

// -----------------------------------------------------------------------------

int Quantizer::ValueToIndex(const int& value) const {
  if (lut_.find(value) == lut_.end()) {
    THROW_ERROR_EXCEPTION("Value out of range");
  }
  return lut_.at(value).first;
}

// -----------------------------------------------------------------------------

int Quantizer::IndexToReconstructionValue(const int& index) const {
  if (inverse_lut_.find(index) == inverse_lut_.end()) {
    THROW_ERROR_EXCEPTION("Index not found");
  }
  return inverse_lut_.at(index);
}

// -----------------------------------------------------------------------------

int Quantizer::ValueToReconstructionValue(const int& value) const {
  if (lut_.find(value) == lut_.end()) {
    THROW_ERROR_EXCEPTION("Value out of range");
  }

  return lut_.at(value).second;
}

// -----------------------------------------------------------------------------

const std::map<size_t, int>& Quantizer::InverseLut() const {
  return inverse_lut_;
}

// -----------------------------------------------------------------------------

void Quantizer::print() const {
  std::stringstream stream;
  stream << "LUT:" << std::endl;
  for (const auto& [fst, snd] : lut_) {
    stream << "  " << fst << ": ";
    stream << snd.first << ",";
    stream << snd.second << std::endl;
  }

  stream << "Inverse LUT:" << std::endl;
  for (const auto& [fst, snd] : inverse_lut_) {
    stream << "  " << fst << ": ";
    stream << snd << std::endl;
  }

  std::string line;
  while (std::getline(stream, line)) {
    GetLogging().standard_out(line);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
