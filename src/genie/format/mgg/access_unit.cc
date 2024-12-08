/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/access_unit.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool AccessUnit::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const AccessUnit&>(info);
  return header_ == other.header_ && au_information_ == other.au_information_ &&
         au_protection_ == other.au_protection_ && blocks_ == other.blocks_;
}

// -----------------------------------------------------------------------------
AccessUnit::AccessUnit(AccessUnitHeader h, const core::MpegMinorVersion version)
    : header_(std::move(h)), version_(version) {}

// -----------------------------------------------------------------------------
AccessUnit::AccessUnit(
    util::BitReader& reader,
    const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
    const bool mit, const bool block_header,
    const core::MpegMinorVersion version)
    : version_(version) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  std::string tmp(4, '\0');
  reader.ReadAlignedBytes(tmp.data(), tmp.length());
  UTILS_DIE_IF(tmp != "auhd", "Access unit without header");
  header_ = AccessUnitHeader(reader, parameter_sets, mit);
  do {
    const auto tmp_pos = reader.GetStreamPosition();
    std::string tmp_str(4, '\0');
    reader.ReadAlignedBytes(tmp_str.data(), tmp_str.length());
    if (tmp_str == "auin") {
      UTILS_DIE_IF(au_information_ != std::nullopt, "AU-Inf already present");
      UTILS_DIE_IF(au_protection_ != std::nullopt,
                   "AU-Inf must be before AU-PR");
      au_information_ = AuInformation(reader, version_);
    } else if (tmp_str == "aupr") {
      UTILS_DIE_IF(au_protection_ != std::nullopt, "AU-Pr already present");
      au_protection_ = AuProtection(reader, version_);
    } else {
      reader.SetStreamPosition(tmp_pos);
      break;
    }
  } while (true);
  if (block_header) {
    for (size_t i = 0; i < header_.GetHeader().GetNumBlocks(); ++i) {
      blocks_.emplace_back(reader);
    }
  }
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------
void AccessUnit::AddBlock(Block b) { blocks_.emplace_back(std::move(b)); }

// -----------------------------------------------------------------------------
const std::vector<Block>& AccessUnit::GetBlocks() const { return blocks_; }

// -----------------------------------------------------------------------------
const AccessUnitHeader& AccessUnit::GetHeader() const { return header_; }

// -----------------------------------------------------------------------------
bool AccessUnit::HasInformation() const {
  return au_information_ != std::nullopt;
}

// -----------------------------------------------------------------------------
bool AccessUnit::HasProtection() const {
  return au_protection_ != std::nullopt;
}

// -----------------------------------------------------------------------------
const AuInformation& AccessUnit::GetInformation() const {
  return *au_information_;
}

// -----------------------------------------------------------------------------
const AuProtection& AccessUnit::GetProtection() const {
  return *au_protection_;
}

// -----------------------------------------------------------------------------
void AccessUnit::SetInformation(AuInformation au) {
  au_information_ = std::move(au);
}

// -----------------------------------------------------------------------------
void AccessUnit::SetProtection(AuProtection au) {
  au_protection_ = std::move(au);
}

// -----------------------------------------------------------------------------
void AccessUnit::BoxWrite(util::BitWriter& bit_writer) const {
  header_.Write(bit_writer);
  if (au_information_ != std::nullopt) {
    au_information_->Write(bit_writer);
  }
  if (au_protection_ != std::nullopt) {
    au_protection_->Write(bit_writer);
  }
  for (const auto& b : blocks_) {
    b.Write(bit_writer);
  }
}

// -----------------------------------------------------------------------------
const std::string& AccessUnit::GetKey() const {
  static const std::string key = "aucn";
  return key;
}

// -----------------------------------------------------------------------------
void AccessUnit::PrintDebug(std::ostream& output, const uint8_t depth,
                             const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Access Unit");
  header_.PrintDebug(output, depth + 1, max_depth);
  if (au_information_) {
    au_information_->PrintDebug(output, depth + 1, max_depth);
  }
  if (au_protection_) {
    au_protection_->PrintDebug(output, depth + 1, max_depth);
  }
  for (const auto& r : blocks_) {
    r.PrintDebug(output, depth + 1, max_depth);
  }
}

// -----------------------------------------------------------------------------
AuInformation& AccessUnit::GetInformation() { return *au_information_; }

// -----------------------------------------------------------------------------
AuProtection& AccessUnit::GetProtection() { return *au_protection_; }

// -----------------------------------------------------------------------------
mgb::AccessUnit AccessUnit::Decapsulate() {
  std::vector<mgb::Block> new_blocks;
  new_blocks.reserve(blocks_.size());
  for (auto& b : blocks_) {
    new_blocks.emplace_back(b.decapsulate());
  }
  return {std::move(header_.GetHeader()), std::move(new_blocks)};
}

// -----------------------------------------------------------------------------
AccessUnit::AccessUnit(mgb::AccessUnit au, const bool mit,
                       const core::MpegMinorVersion version)
    : header_(std::move(au.GetHeader()), mit), version_(version) {
  for (auto& b : au.GetBlocks()) {
    blocks_.emplace_back(std::move(b));
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
