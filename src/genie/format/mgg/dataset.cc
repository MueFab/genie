/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset.h"

#include <genie/core/meta/block_header/enabled.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

Dataset::Dataset(util::BitReader& reader, const core::MpegMinorVersion version)
    : version_(version) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  const auto end_pos = start_pos + static_cast<int64_t>(length);
  std::string tmp(4, '\0');
  reader.ReadAlignedBytes(tmp.data(), tmp.length());
  UTILS_DIE_IF(tmp != "dthd", "Dataset without header");  // NOLINT
  header_ = DatasetHeader(reader);
  while (reader.GetStreamPosition() != end_pos) {
    UTILS_DIE_IF(reader.GetStreamPosition() > end_pos, "Read too far");
    UTILS_DIE_IF(!reader.IsStreamGood(), "Reader died");
    ReadBox(reader, false);
  }
}

// -----------------------------------------------------------------------------

const DatasetHeader& Dataset::GetHeader() const { return header_; }

// -----------------------------------------------------------------------------

void Dataset::BoxWrite(util::BitWriter& bit_writer) const {
  header_.Write(bit_writer);
  if (metadata_) {
    metadata_->Write(bit_writer);
  }
  if (protection_) {
    protection_->Write(bit_writer);
  }
  for (const auto& p : parameter_sets_) {
    p.Write(bit_writer);
  }
  if (master_index_table_) {
    master_index_table_->Write(bit_writer);
  }
  for (const auto& p : access_units_) {
    p.Write(bit_writer);
  }
  for (const auto& p : descriptor_streams_) {
    p.Write(bit_writer);
  }
}

// -----------------------------------------------------------------------------

const std::string& Dataset::GetKey() const {
  static const std::string key = "dtcn";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------

bool Dataset::HasMetadata() const { return metadata_ != std::nullopt; }

// -----------------------------------------------------------------------------

bool Dataset::HasProtection() const { return metadata_ != std::nullopt; }

// -----------------------------------------------------------------------------

DatasetMetadata& Dataset::GetMetadata() { return *metadata_; }

// -----------------------------------------------------------------------------

DatasetProtection& Dataset::GetProtection() { return *protection_; }

// -----------------------------------------------------------------------------

std::vector<AccessUnit>& Dataset::GetAccessUnits() { return access_units_; }

// -----------------------------------------------------------------------------

std::vector<DescriptorStream>& Dataset::GetDescriptorStreams() {
  return descriptor_streams_;
}

// -----------------------------------------------------------------------------

std::vector<DatasetParameterSet>& Dataset::GetParameterSets() {
  return parameter_sets_;
}

// -----------------------------------------------------------------------------

Dataset::Dataset(mgb::MgbFile& file, core::meta::Dataset& meta,
                 const core::MpegMinorVersion version,
                 const std::vector<uint8_t>& param_ids)
    : version_(version) {
  bool mit_flag = false;
  bool cc_mode = false;
  bool ordered_blocks = false;
  const bool header_on = meta.GetHeader().GetType() ==
                         core::meta::BlockHeader::HeaderType::kEnabled;
  if (header_on) {
    cc_mode =
        dynamic_cast<const core::meta::block_header::Enabled&>(meta.GetHeader())
            .GetCcFlag();
    mit_flag =
        dynamic_cast<const core::meta::block_header::Enabled&>(meta.GetHeader())
            .GetMitFlag();
  } else {
    ordered_blocks = dynamic_cast<const core::meta::block_header::Disabled&>(
                         meta.GetHeader())
                         .GetOrderedFlag();
  }

  if (!header_on) {
    if (ordered_blocks) {
      file.sort_by_position();
    }
    for (size_t c = 0; c < static_cast<size_t>(core::record::ClassType::kCount);
         ++c) {
      for (size_t d = 0; d < static_cast<size_t>(core::GenDesc::kCount); ++d) {
        auto blocks =
            file.ExtractDescriptor(static_cast<core::record::ClassType>(c),
                                   static_cast<core::GenDesc>(d), param_ids);
        auto desc = DescriptorStream(static_cast<core::GenDesc>(d),
                                     static_cast<core::record::ClassType>(c),
                                     std::move(blocks));
        if (!desc.IsEmpty()) {
          descriptor_streams_.emplace_back(std::move(desc));
          for (auto& b : meta.GetDSs()) {
            if (static_cast<core::GenDesc>(b.GetId()) ==
                descriptor_streams_.back().GetHeader().GetDescriptorId()) {
              descriptor_streams_.back().SetProtection(
                  DescriptorStreamProtection(std::move(b.GetProtection())));
            }
          }
        }
      }
    }
    file.ClearAuBlocks(param_ids);
  } else {
    if (cc_mode) {
      file.sort_by_class();
    } else {
      file.sort_by_position();
    }
  }

  auto params_p2 = file.ExtractParameters(param_ids);
  for (const auto& p : params_p2) {
    parameter_sets_.emplace_back(
        static_cast<uint8_t>(0), static_cast<uint16_t>(0), p->GetId(),
        p->GetParentId(), std::move(p->GetEncodingSet()), version_);
  }

  auto access_units_p2 = file.ExtractAUs(param_ids);

  for (auto& a : access_units_p2) {
    access_units_.emplace_back(std::move(*a), mit_flag, version_);
    for (auto& b : meta.GetAUs()) {
      if (b.GetId() == access_units_.back().GetHeader().GetHeader().GetId()) {
        access_units_.back().SetInformation(
            AuInformation(0, 0, std::move(b.GetInformation()), version));
        access_units_.back().SetProtection(
            AuProtection(0, 0, std::move(b.GetProtection()), version));
      }
    }
  }

  header_ = DatasetHeader(
      0, 0, version_,
      parameter_sets_.front().GetEncodingSet().HasMultipleAlignments(), true,
      false, parameter_sets_.front().GetEncodingSet().GetPosSize() == 40,
      parameter_sets_.front().GetEncodingSet().GetDatasetType(), false,
      parameter_sets_.front().GetEncodingSet().GetAlphabetId());

  if (!meta.GetInformation().empty()) {
    metadata_ =
        DatasetMetadata(0, 0, std::move(meta.GetInformation()), version);
  }

  if (!meta.GetProtection().empty()) {
    protection_ =
        DatasetProtection(0, 0, std::move(meta.GetProtection()), version);
  }
}

// -----------------------------------------------------------------------------

void Dataset::PatchId(const uint8_t group_id, const uint16_t set_id) {
  header_.PatchId(group_id, set_id);
  if (metadata_ != std::nullopt) {
    metadata_->PatchId(group_id, set_id);
  }
  if (protection_ != std::nullopt) {
    protection_->PatchId(group_id, set_id);
  }
  for (auto& ps : parameter_sets_) {
    ps.PatchId(group_id, set_id);
  }
}

// -----------------------------------------------------------------------------

void Dataset::PatchRefId(const uint8_t old, const uint8_t _new) {
  header_.PatchRefId(old, _new);
}

// -----------------------------------------------------------------------------

DatasetHeader& Dataset::GetHeader() { return header_; }

// -----------------------------------------------------------------------------

void Dataset::ReadBox(util::BitReader& reader,  // NOLINT
                      const bool in_offset) {
  std::string tmp_str(4, '\0');
  reader.ReadAlignedBytes(tmp_str.data(), tmp_str.length());
  if (tmp_str == "dtmd") {  // NOLINT
    UTILS_DIE_IF(metadata_ != std::nullopt, "Metadata already present");
    UTILS_DIE_IF(protection_ != std::nullopt,
                 "Metadata must be before protection");
    UTILS_DIE_IF(!parameter_sets_.empty(),
                 "Metadata must be before parameter sets");
    UTILS_DIE_IF(master_index_table_ != std::nullopt,
                 "Metadata must be before MIT");
    UTILS_DIE_IF(!access_units_.empty(),
                 "Metadata must be before Access Units");
    UTILS_DIE_IF(!descriptor_streams_.empty(),
                 "Metadata must be before Descriptor streams");
    metadata_ = DatasetMetadata(reader, version_);
  } else if (tmp_str == "dtpr") {  // NOLINT
    UTILS_DIE_IF(protection_ != std::nullopt, "Protection already present");
    UTILS_DIE_IF(!parameter_sets_.empty(),
                 "Metadata must be before parameter sets");
    UTILS_DIE_IF(master_index_table_ != std::nullopt,
                 "Metadata must be before MIT");
    UTILS_DIE_IF(!access_units_.empty(),
                 "Metadata must be before Access Units");
    UTILS_DIE_IF(!descriptor_streams_.empty(),
                 "Metadata must be before Descriptor streams");
    protection_ = DatasetProtection(reader, version_);
  } else if (tmp_str == "pars") {
    UTILS_DIE_IF(in_offset, "Offset not permitted");
    UTILS_DIE_IF(master_index_table_ != std::nullopt,
                 "Metadata must be before MIT");
    UTILS_DIE_IF(!access_units_.empty(),
                 "Metadata must be before Access Units");
    UTILS_DIE_IF(!descriptor_streams_.empty(),
                 "Metadata must be before Descriptor streams");
    parameter_sets_.emplace_back(reader, version_,
                                 header_.GetParameterUpdateFlag());
    encoding_sets_.emplace(
        static_cast<size_t>(parameter_sets_.back().GetParameterSetId()),
        parameter_sets_.back().GetEncodingSet());
  } else if (tmp_str == "mitb") {  // NOLINT
    UTILS_DIE_IF(in_offset, "Offset not permitted");
    UTILS_DIE_IF(master_index_table_ != std::nullopt, "MIT already present");
    UTILS_DIE_IF(!access_units_.empty(),
                 "Metadata must be before Access Units");
    UTILS_DIE_IF(!descriptor_streams_.empty(),
                 "Metadata must be before Descriptor streams");
    master_index_table_ = MasterIndexTable(reader, header_);
  } else if (tmp_str == "aucn") {  // NOLINT
    UTILS_DIE_IF(in_offset, "Offset not permitted");
    UTILS_DIE_IF(!descriptor_streams_.empty(),
                 "Metadata must be before Descriptor streams");
    access_units_.emplace_back(reader, encoding_sets_, header_.IsMitEnabled(),
                               header_.IsBlockHeaderEnabled(), version_);
  } else if (tmp_str == "dscn") {  // NOLINT
    UTILS_DIE_IF(in_offset, "Offset not permitted");
    UTILS_DIE_IF(master_index_table_ == std::nullopt,
                 "descriptor streams without MIT not allowed");
    UTILS_DIE_IF(header_.IsBlockHeaderEnabled(),
                 "descriptor streams only allowed without block headers");
    descriptor_streams_.emplace_back(reader, *master_index_table_,
                                     header_.GetMitConfigs());
  } else if (tmp_str == "offs") {
    UTILS_DIE_IF(in_offset, "Recursive offset not permitted");
    reader.ReadAlignedBytes(tmp_str.data(), tmp_str.length());
    const auto offset = reader.ReadAlignedInt<uint64_t>();
    if (offset == ~static_cast<uint64_t>(0)) {
      return;
    }
    const auto pos_save = reader.GetStreamPosition();
    reader.SetStreamPosition(static_cast<int64_t>(offset));
    ReadBox(reader, true);
    reader.SetStreamPosition(pos_save);
  } else {
    UTILS_DIE("Unknown box");
  }
}

// -----------------------------------------------------------------------------

void Dataset::PrintDebug(std::ostream& output, uint8_t depth,
                          uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Dataset");
  header_.PrintDebug(output, depth + 1, max_depth);
  if (metadata_) {
    metadata_->PrintDebug(output, depth + 1, max_depth);
  }
  if (protection_) {
    metadata_->PrintDebug(output, depth + 1, max_depth);
  }
  for (const auto& r : parameter_sets_) {
    r.PrintDebug(output, depth + 1, max_depth);
  }
  if (master_index_table_) {
    master_index_table_->PrintDebug(output, depth + 1, max_depth);
  }
#ifdef Genie_DEBUG_PRINT_NODETAIL
  if (!access_units_.empty()) {
    print_offset(output, depth + 1, max_depth,
                 "* " + std::to_string(access_units_.size()) + " access units");
  }
  if (!descriptor_streams_.empty()) {
    print_offset(output, depth + 1, max_depth,
                 "* " + std::to_string(descriptor_streams_.size()) +
                     " descriptor streams");
  }
#else
  for (const auto& r : access_units) {
    r.print_debug(output, depth + 1, max_depth);
  }
  for (const auto& r : descriptor_streams) {
    r.print_debug(output, depth + 1, max_depth);
  }
#endif
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
