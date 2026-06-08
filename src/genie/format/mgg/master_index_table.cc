/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/master_index_table.h"

#include <algorithm>
#include <string>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

bool MasterIndexTable::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const MasterIndexTable&>(info);
  return aligned_aus_ == other.aligned_aus_ &&
         unaligned_aus_ == other.unaligned_aus_;
}

// -----------------------------------------------------------------------------

const std::vector<std::vector<std::vector<master_index_table::AlignedAuIndex>>>&
MasterIndexTable::GetAlignedAUs() const {
  return aligned_aus_;
}

// -----------------------------------------------------------------------------

const std::vector<master_index_table::UnalignedAuIndex>&
MasterIndexTable::GetUnalignedAUs() const {
  return unaligned_aus_;
}

// -----------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(const uint16_t seq_count,
                                   const uint8_t num_classes) {
  aligned_aus_.resize(
      seq_count,
      std::vector(num_classes,
                  std::vector<master_index_table::AlignedAuIndex>()));
}

// -----------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(util::BitReader& reader,
                                   const DatasetHeader& hdr) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.Read<uint64_t>();
  aligned_aus_.resize(
      hdr.GetReferenceOptions().GetSeqIDs().size(),
      std::vector(hdr.GetMitConfigs().size(),
                  std::vector<master_index_table::AlignedAuIndex>()));
  for (size_t seq = 0; seq < hdr.GetReferenceOptions().GetSeqIDs().size();
       ++seq) {
    for (size_t ci = 0; ci < hdr.GetMitConfigs().size(); ++ci) {
      if (core::record::ClassType::kClassU ==
          hdr.GetMitConfigs()[ci].GetClassId()) {
        continue;
      }
      for (size_t au_id = 0;
           au_id < hdr.GetReferenceOptions().GetSeqBlocks()[seq]; ++au_id) {
        aligned_aus_[seq][ci].emplace_back(
            reader, hdr.GetByteOffsetSize(), hdr.GetPosBits(),
            hdr.GetDatasetType(), hdr.GetMultipleAlignmentFlag(),
            hdr.IsBlockHeaderEnabled(),
            hdr.GetMitConfigs()[ci].GetDescriptorIDs());
      }
    }
  }
  for (size_t uau_id = 0; uau_id < hdr.GetNumUAccessUnits(); ++uau_id) {
    unaligned_aus_.emplace_back(
        reader, hdr.GetByteOffsetSize(), hdr.GetPosBits(), hdr.GetDatasetType(),
        hdr.GetUOptions().HasSignature(),
        hdr.GetUOptions().HasSignature() &&
            hdr.GetUOptions().GetSignature().IsConstLength(),
        hdr.GetUOptions().HasSignature() &&
                hdr.GetUOptions().GetSignature().IsConstLength()
            ? hdr.GetUOptions().GetSignature().GetConstLength()
            : static_cast<uint8_t>(0),
        hdr.IsBlockHeaderEnabled(),
        hdr.GetMitConfigs().back().GetDescriptorIDs(), hdr.GetAlphabetId());
  }
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

void MasterIndexTable::BoxWrite(util::BitWriter& bit_writer) const {
  for (const auto& a : aligned_aus_) {
    for (const auto& b : a) {
      for (const auto& c : b) {
        c.Write(bit_writer);
      }
    }
  }
  for (const auto& a : unaligned_aus_) {
    a.Write(bit_writer);
  }
}

// -----------------------------------------------------------------------------

const std::string& MasterIndexTable::GetKey() const {
  static const std::string key = "mitb";
  return key;
}

// -----------------------------------------------------------------------------

std::vector<uint64_t> MasterIndexTable::GetDescriptorStreamOffsets(
    const uint8_t class_index, const uint8_t desc_index,
    const bool is_unaligned, const uint64_t total_size) const {
  std::vector<uint64_t> offsets;
  if (is_unaligned) {
    for (const auto& unaligned_au : unaligned_aus_) {
      offsets.emplace_back(unaligned_au.GetBlockOffsets().at(desc_index));
    }
  } else {
    for (const auto& seq : aligned_aus_) {
      for (const auto& aligned_au : seq.at(class_index)) {
        offsets.emplace_back(aligned_au.GetBlockOffsets().at(desc_index));
      }
    }
  }
  std::sort(offsets.begin(), offsets.end());
  uint64_t last = offsets.front();
  offsets.erase(offsets.begin());
  offsets.emplace_back(total_size + last);
  for (uint64_t& offset : offsets) {
    const uint64_t tmp = offset;
    offset = offset - last;
    last = tmp;
  }
  return offsets;
}

// -----------------------------------------------------------------------------

void MasterIndexTable::PrintDebug(std::ostream& output, const uint8_t depth,
                                   const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Master index table");
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
