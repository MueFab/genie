/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/raw_reference.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
RawReference::RawReference(const bool header_less)
    : DataUnit(DataUnitType::kRawReference), headerless_(header_less) {}

// -----------------------------------------------------------------------------
RawReference::RawReference(util::BitReader& reader, bool header_only,
                           const bool header_less)
    : DataUnit(DataUnitType::kRawReference), headerless_(header_less) {
  if (!header_less) {
    reader.Read<uint64_t>();
  }
  const auto count = reader.Read<uint16_t>();
  for (size_t i = 0; i < count; ++i) {
    seqs_.emplace_back(reader, header_only);
  }
}

// -----------------------------------------------------------------------------
void RawReference::AddSequence(RawReferenceSequence&& ref) {
  for (const auto& a : seqs_) {
    if (!a.IsIdUnique(ref)) {
      UTILS_THROW_RUNTIME_EXCEPTION("Reference ID is not unique");
    }
  }
  seqs_.push_back(std::move(ref));
}

// -----------------------------------------------------------------------------
void RawReference::Write(util::BitWriter& writer) const {
  if (!headerless_) {
    DataUnit::Write(writer);
    uint64_t size = 0;
    for (auto& i : seqs_) {
      size += i.GetTotalSize();
    }
    size += (8 + 64 + 16) / 8;  // data_unit_type, data_unit_size, seq_count
    writer.WriteBits(size, 64);
  }
  writer.WriteBits(seqs_.size(), 16);

  for (auto& i : seqs_) {
    i.Write(writer);
  }
}

// -----------------------------------------------------------------------------
bool RawReference::IsEmpty() const { return seqs_.empty(); }

// -----------------------------------------------------------------------------
RawReferenceSequence& RawReference::GetSequence(const size_t index) {
  return seqs_[index];
}

// -----------------------------------------------------------------------------
std::vector<RawReferenceSequence>::iterator RawReference::begin() {
  return seqs_.begin();
}

// -----------------------------------------------------------------------------
std::vector<RawReferenceSequence>::iterator RawReference::end() {
  return seqs_.end();
}

// -----------------------------------------------------------------------------
void RawReference::SetHeaderLess(const bool state) { headerless_ = state; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
