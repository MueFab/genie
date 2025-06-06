/**
 * Copyright 2018-2024 The Genie Authors.
 * @file quantizer.cc
 * Implementation file for the Quantizer class.
 *
 * This file provides the implementation of the `Quantizer` class, which maps
 * genomic quality values to compact index representations using a look-up table
 * (LUT). The class provides methods to transform quality values to their
 * respective indices and reconstruction values for efficient encoding and
 * decoding of quality scores.
 *
 * @details The `Quantizer` class allows for efficient representation of quality
 * values by quantizing them into a set of indices and corresponding
 * reconstruction values. It maintains a forward and inverse LUT for encoding
 * and decoding operations. The LUT maps quality values to indices, and indices
 * to reconstruction values, facilitating compact storage and quick lookup
 * during the quantization process.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/quantizer.h"

// -----------------------------------------------------------------------------

#include <map>
#include <sstream>
#include <string>

// -----------------------------------------------------------------------------

#include "genie/util/runtime_exception.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Util";

namespace genie::util {

// -----------------------------------------------------------------------------

Quantizer::Quantizer() = default;

// -----------------------------------------------------------------------------

Quantizer::Quantizer(const std::map<std::size_t, int>& inverse_lut)
    : inverse_lut_(inverse_lut) {}

// -----------------------------------------------------------------------------

Quantizer::~Quantizer() = default;

// -----------------------------------------------------------------------------

int Quantizer::ValueToIndex(const int& value) const {
  UTILS_DIE_IF(lut_.find(value) == lut_.end(), "Value out of range");
  return lut_.at(value).first;
}

// -----------------------------------------------------------------------------

int Quantizer::IndexToReconstructionValue(const int& index) const {
  UTILS_DIE_IF(inverse_lut_.find(index) == inverse_lut_.end(),
               "Index not found");
  return inverse_lut_.at(index);
}

// -----------------------------------------------------------------------------

int Quantizer::ValueToReconstructionValue(const int& value) const {
  UTILS_DIE_IF(lut_.find(value) == lut_.end(), "Value out of range");

  return lut_.at(value).second;
}

// -----------------------------------------------------------------------------

const std::map<std::size_t, int>& Quantizer::InverseLut() const {
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

  UTILS_LOG(Logger::Severity::INFO, stream.str());
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
