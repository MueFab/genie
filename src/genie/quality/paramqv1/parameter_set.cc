/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/paramqv1/parameter_set.h"

#include <utility>
#include <vector>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::paramqv1 {

// -----------------------------------------------------------------------------
bool ParameterSet::operator==(const ParameterSet& ps) const {
  return qv_codebooks_ == ps.qv_codebooks_;
}

// -----------------------------------------------------------------------------
void ParameterSet::AddCodeBook(Codebook&& book) {
  qv_codebooks_.emplace_back(std::move(book));
}

// -----------------------------------------------------------------------------
void ParameterSet::write(util::BitWriter& writer) const {
  writer.WriteBits(qv_codebooks_.size(), 4);
  for (const auto& q : qv_codebooks_) {
    q.write(writer);
  }
}

// -----------------------------------------------------------------------------
ParameterSet::ParameterSet(util::BitReader& reader) {
  const size_t num_codebooks = reader.Read<int>(4);
  for (auto c = num_codebooks; c > 0; --c) {
    qv_codebooks_.emplace_back(reader);
  }
}

// -----------------------------------------------------------------------------
const std::vector<Codebook>& ParameterSet::GetCodebooks() const {
  return qv_codebooks_;
}

// -----------------------------------------------------------------------------
}  // namespace genie::quality::paramqv1
