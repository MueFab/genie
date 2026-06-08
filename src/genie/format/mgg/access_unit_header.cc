/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/access_unit_header.h"

#include <map>
#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

void AccessUnitHeader::PrintDebug(std::ostream& output, const uint8_t depth,
                                   const uint8_t max_depth) const {
  static const std::string class_lut[] = {"NONE", "P",  "N", "M",
                                          "I",    "HM", "U"};
  print_offset(output, depth, max_depth, "* Access Unit Header");
  print_offset(output, depth + 1, max_depth,
               "Access unit ID: " + std::to_string(header_.GetId()));
  print_offset(
      output, depth + 1, max_depth,
      "Access unit class: " + class_lut[static_cast<int>(header_.GetClass())]);
  print_offset(output, depth + 1, max_depth,
               "Access unit blocks: " + std::to_string(header_.GetNumBlocks()));
  print_offset(
      output, depth + 1, max_depth,
      "Access unit records: " + std::to_string(header_.GetReadCount()));
  print_offset(output, depth + 1, max_depth,
               "Access unit parameter set ID: " +
                   std::to_string(header_.GetParameterId()));
  if (header_.GetClass() != core::record::ClassType::kClassU) {
    print_offset(output, depth + 1, max_depth,
                 "Access unit reference ID: " +
                     std::to_string(header_.GetAlignmentInfo().GetRefId()));
    print_offset(output, depth + 1, max_depth,
                 "Access unit start pos:  " +
                     std::to_string(header_.GetAlignmentInfo().GetStartPos()));
    print_offset(output, depth + 1, max_depth,
                 "Access unit end pos:  " +
                     std::to_string(header_.GetAlignmentInfo().GetEndPos()));
  }
}

// -----------------------------------------------------------------------------

bool AccessUnitHeader::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const AccessUnitHeader&>(info);
  return header_ == other.header_ && mit_flag_ == other.mit_flag_;
}

// -----------------------------------------------------------------------------

const mgb::AuHeader& AccessUnitHeader::GetHeader() const { return header_; }

// -----------------------------------------------------------------------------

mgb::AuHeader& AccessUnitHeader::GetHeader() { return header_; }

// -----------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader()
    : AccessUnitHeader(mgb::AuHeader(), false) {}

// -----------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader(
    util::BitReader& reader,
    const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
    const bool mit)
    : mit_flag_(mit) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  header_ = mgb::AuHeader(reader, parameter_sets, !mit_flag_);
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader(mgb::AuHeader header, const bool mit_flag)
    : header_(std::move(header)), mit_flag_(mit_flag) {}

// -----------------------------------------------------------------------------

void AccessUnitHeader::BoxWrite(util::BitWriter& bit_writer) const {
  header_.Write(bit_writer, !mit_flag_);
}

// -----------------------------------------------------------------------------

const std::string& AccessUnitHeader::GetKey() const {
  static const std::string key = "auhd";
  return key;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
