/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/chunk.h"

#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------
ReferenceManager::ReferenceExcerpt& Chunk::GetRef() { return reference_; }

// -----------------------------------------------------------------------------
void Chunk::AddRefToWrite(size_t start, size_t end) {
  ref_to_write_.emplace_back(start, end);
}

// -----------------------------------------------------------------------------
const std::vector<std::pair<size_t, size_t>>& Chunk::GetRefToWrite() const {
  return ref_to_write_;
}

// -----------------------------------------------------------------------------
const ReferenceManager::ReferenceExcerpt& Chunk::GetRef() const {
  return reference_;
}

// -----------------------------------------------------------------------------
void Chunk::SetRefId(const size_t id) { ref_id_ = id; }

// -----------------------------------------------------------------------------
size_t Chunk::GetRefId() const { return ref_id_; }

// -----------------------------------------------------------------------------
std::vector<Record>& Chunk::GetData() { return data_; }

// -----------------------------------------------------------------------------
const std::vector<Record>& Chunk::GetData() const { return data_; }

// -----------------------------------------------------------------------------
stats::PerfStats& Chunk::GetStats() { return stats_; }

// -----------------------------------------------------------------------------
void Chunk::SetStats(stats::PerfStats&& s) { stats_ = std::move(s); }

// -----------------------------------------------------------------------------
bool Chunk::IsReferenceOnly() const { return reference_only_; }

// -----------------------------------------------------------------------------
void Chunk::SetReferenceOnly(const bool ref) { reference_only_ = ref; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
