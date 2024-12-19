/**
 * Copyright 2018-2024 The Genie Authors.
 * @file codebook.cc
 *
 * @brief Implementation of the Codebook class for quality value (QV)
 * representation in the Genie framework.
 *
 * This file contains the implementation of the `Codebook` class within the
 * `paramqv1` namespace. The class provides functionality for managing quality
 * value reconstruction entries and their serialization.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/paramqv1/codebook.h"

#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::quality::paramqv1 {

// -----------------------------------------------------------------------------
bool Codebook::operator==(const Codebook& ps) const {
  return qv_recon_ == ps.qv_recon_;
}

// -----------------------------------------------------------------------------
Codebook::Codebook(util::BitReader& reader) {
  qv_recon_.resize(reader.Read<uint8_t>());
  for (auto& v : qv_recon_) {
    v = reader.Read<uint8_t>();
  }
}

// -----------------------------------------------------------------------------
void Codebook::AddEntry(uint8_t entry) {
  constexpr size_t book_max_size = 94;
  UTILS_DIE_IF(qv_recon_.size() == book_max_size,
               "Maximum codebook Size exceeded");
  qv_recon_.emplace_back(entry);
}

// -----------------------------------------------------------------------------
void Codebook::write(util::BitWriter& writer) const {
  writer.WriteBits(qv_recon_.size(), 8);
  for (const auto& v : qv_recon_) {
    writer.WriteBits(v, 8);
  }
}

// -----------------------------------------------------------------------------
Codebook::Codebook(const uint8_t v1, const uint8_t v2) {
  qv_recon_.push_back(v1);
  qv_recon_.push_back(v2);
}

// -----------------------------------------------------------------------------
const std::vector<uint8_t>& Codebook::GetEntries() const { return qv_recon_; }

// -----------------------------------------------------------------------------
}  // namespace genie::quality::paramqv1
