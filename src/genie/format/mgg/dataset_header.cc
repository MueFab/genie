/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/meta/block_header/enabled.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

bool DatasetHeader::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetHeader&>(info);
  return group_id_ == other.group_id_ && id_ == other.id_ &&
         version_ == other.version_ &&
         multiple_alignment_flag_ == other.multiple_alignment_flag_ &&
         byte_offset_size_flag_ == other.byte_offset_size_flag_ &&
         non_overlapping_au_range_flag_ ==
             other.non_overlapping_au_range_flag_ &&
         pos_40_bits_flag_ == other.pos_40_bits_flag_ &&
         block_header_on_ == other.block_header_on_ &&
         block_header_off_ == other.block_header_off_ &&
         reference_options_ == other.reference_options_ &&
         dataset_type_ == other.dataset_type_ &&
         mit_configs_ == other.mit_configs_ &&
         parameters_update_flag_ == other.parameters_update_flag_ &&
         alphabet_id_ == other.alphabet_id_ &&
         num_u_access_units_ == other.num_u_access_units_ &&
         u_options_ == other.u_options_ && thresholds_ == other.thresholds_;
}

// -----------------------------------------------------------------------------

uint8_t DatasetHeader::GetDatasetGroupId() const { return group_id_; }

// -----------------------------------------------------------------------------

uint16_t DatasetHeader::GetDatasetId() const { return id_; }

// -----------------------------------------------------------------------------

core::MpegMinorVersion DatasetHeader::GetVersion() const { return version_; }

// -----------------------------------------------------------------------------

bool DatasetHeader::GetMultipleAlignmentFlag() const {
  return multiple_alignment_flag_;
}

// -----------------------------------------------------------------------------

uint8_t DatasetHeader::GetByteOffsetSize() const {
  return byte_offset_size_flag_ ? 64 : 32;
}

// -----------------------------------------------------------------------------

bool DatasetHeader::GetNonOverlappingAuRangeFlag() const {
  return non_overlapping_au_range_flag_;
}

// -----------------------------------------------------------------------------

uint8_t DatasetHeader::GetPosBits() const {
  return pos_40_bits_flag_ ? 40 : 32;
}

// -----------------------------------------------------------------------------

bool DatasetHeader::IsBlockHeaderEnabled() const {
  return block_header_on_ != std::nullopt;
}

// -----------------------------------------------------------------------------

bool DatasetHeader::IsMitEnabled() const {
  return block_header_off_ != std::nullopt || block_header_on_->GetMitFlag();
}

// -----------------------------------------------------------------------------

bool DatasetHeader::IsCcModeEnabled() const {
  return block_header_on_ != std::nullopt && block_header_on_->GetCcFlag();
}

// -----------------------------------------------------------------------------

bool DatasetHeader::IsOrderedBlockMode() const {
  return block_header_off_ != std::nullopt &&
         block_header_off_->GetOrderedBlocksFlag();
}

// -----------------------------------------------------------------------------

const dataset_header::ReferenceOptions& DatasetHeader::GetReferenceOptions()
    const {
  return reference_options_;
}

// -----------------------------------------------------------------------------

core::parameter::DataUnit::DatasetType DatasetHeader::GetDatasetType() const {
  return dataset_type_;
}

// -----------------------------------------------------------------------------

const std::vector<dataset_header::MitClassConfig>&
DatasetHeader::GetMitConfigs() const {
  return mit_configs_;
}

// -----------------------------------------------------------------------------

bool DatasetHeader::GetParameterUpdateFlag() const {
  return parameters_update_flag_;
}

// -----------------------------------------------------------------------------

core::AlphabetId DatasetHeader::GetAlphabetId() const { return alphabet_id_; }

// -----------------------------------------------------------------------------

uint32_t DatasetHeader::GetNumUAccessUnits() const {
  return num_u_access_units_;
}

// -----------------------------------------------------------------------------

const dataset_header::UOptions& DatasetHeader::GetUOptions() const {
  return *u_options_;
}

// -----------------------------------------------------------------------------

const std::vector<std::optional<uint32_t>>& DatasetHeader::GetRefSeqThresholds()
    const {
  return thresholds_;
}

// -----------------------------------------------------------------------------

const std::string& DatasetHeader::GetKey() const {
  static const std::string key = "dthd";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------

DatasetHeader::DatasetHeader()
    : DatasetHeader(0, 0, core::MpegMinorVersion::kV2000, false, false, false,
                    false, core::parameter::DataUnit::DatasetType::kAligned,
                    false, core::AlphabetId::kAcgtn) {}

// -----------------------------------------------------------------------------

DatasetHeader::DatasetHeader(
    const uint8_t dataset_group_id, const uint16_t dataset_id,
    const core::MpegMinorVersion version, const bool multiple_alignments_flags,
    const bool byte_offset_size_flags, const bool non_overlapping_au_range_flag,
    const bool pos_40_bits_flag,
    const core::parameter::DataUnit::DatasetType dataset_type,
    const bool parameters_update_flag, const core::AlphabetId alphabet_id)
    : group_id_(dataset_group_id),
      id_(dataset_id),
      version_(version),
      multiple_alignment_flag_(multiple_alignments_flags),
      byte_offset_size_flag_(byte_offset_size_flags),
      non_overlapping_au_range_flag_(non_overlapping_au_range_flag),
      pos_40_bits_flag_(pos_40_bits_flag),
      dataset_type_(dataset_type),
      parameters_update_flag_(parameters_update_flag),
      alphabet_id_(alphabet_id) {
  block_header_on_ = dataset_header::BlockHeaderOnOptions{false, false};
  num_u_access_units_ = 0;
}

// -----------------------------------------------------------------------------

DatasetHeader::DatasetHeader(util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  group_id_ = reader.ReadAlignedInt<uint8_t>();
  id_ = reader.ReadAlignedInt<uint16_t>();
  std::string version_string(4, '\0');
  reader.ReadAlignedBytes(version_string.data(), version_string.length());
  version_ = core::GetMpegVersion(version_string);
  UTILS_DIE_IF(version_ == core::MpegMinorVersion::kUnknown,
               "Unknown MPEG version");

  multiple_alignment_flag_ = reader.Read<bool>(1);
  byte_offset_size_flag_ = reader.Read<bool>(1);
  non_overlapping_au_range_flag_ = reader.Read<bool>(1);
  pos_40_bits_flag_ = reader.Read<bool>(1);
  bool block_header_flag = reader.Read<bool>(1);
  if (block_header_flag) {
    block_header_on_ = dataset_header::BlockHeaderOnOptions(reader);
  } else {
    block_header_off_ = dataset_header::BlockHeaderOffOptions(reader);
  }
  reference_options_ = dataset_header::ReferenceOptions(reader);
  dataset_type_ = reader.Read<core::parameter::DataUnit::DatasetType>(4);
  if ((block_header_on_ != std::nullopt && block_header_on_->GetMitFlag()) ||
      block_header_on_ == std::nullopt) {
    const auto num_classes = reader.Read<uint8_t>(4);
    for (size_t i = 0; i < num_classes; ++i) {
      mit_configs_.emplace_back(reader, block_header_flag);
    }
  }
  parameters_update_flag_ = reader.Read<bool>(1);
  alphabet_id_ = reader.Read<core::AlphabetId>(7);
  num_u_access_units_ = reader.Read<uint32_t>(32);
  if (num_u_access_units_) {
    u_options_ = dataset_header::UOptions(reader);
  }
  for (size_t i = 0; i < reference_options_.GetSeqIDs().size(); ++i) {
    const bool flag = reader.Read<bool>(1);
    UTILS_DIE_IF(flag == false && i == 0, "First ref must provide threshold");
    if (flag) {
      thresholds_.emplace_back(reader.Read<uint32_t>(31));
    } else {
      thresholds_.emplace_back(std::nullopt);
    }
  }
  reader.FlushHeldBits();
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

void DatasetHeader::BoxWrite(util::BitWriter& writer) const {
  writer.WriteAlignedInt<uint8_t>(group_id_);
  writer.WriteAlignedInt<uint16_t>(id_);
  const auto& v_string = GetMpegVersionString(version_);
  writer.WriteAlignedBytes(v_string.data(), v_string.length());

  writer.WriteBits(multiple_alignment_flag_, 1);
  writer.WriteBits(byte_offset_size_flag_, 1);
  writer.WriteBits(non_overlapping_au_range_flag_, 1);
  writer.WriteBits(pos_40_bits_flag_, 1);
  writer.WriteBits(block_header_on_ != std::nullopt, 1);
  if (block_header_on_ != std::nullopt) {
    block_header_on_->Write(writer);
  } else {
    block_header_off_->Write(writer);
  }
  reference_options_.Write(writer);
  writer.WriteBits(static_cast<uint8_t>(dataset_type_), 4);
  if ((block_header_on_ != std::nullopt && block_header_on_->GetMitFlag()) ||
      block_header_on_ == std::nullopt) {
    writer.WriteBits(mit_configs_.size(), 4);
    for (const auto& c : mit_configs_) {
      c.Write(writer);
    }
  }
  writer.WriteBits(parameters_update_flag_, 1);
  writer.WriteBits(static_cast<uint8_t>(alphabet_id_), 7);
  writer.WriteBits(num_u_access_units_, 32);
  if (num_u_access_units_) {
    u_options_->Write(writer);
  }
  for (const auto& t : thresholds_) {
    writer.WriteBits(t != std::nullopt, 1);
    if (t != std::nullopt) {
      writer.WriteBits(*t, 31);
    }
  }
  writer.FlushBits();
}

// -----------------------------------------------------------------------------

void DatasetHeader::AddRefSequence(const uint8_t reference_id,
                                   const uint16_t seq_id,
                                   const uint32_t blocks_num,
                                   std::optional<uint32_t> threshold) {
  UTILS_DIE_IF(threshold == std::nullopt && thresholds_.empty(),
               "First threshold must be supplied");
  reference_options_.AddSeq(reference_id, seq_id, blocks_num);
  thresholds_.emplace_back(threshold);
}

// -----------------------------------------------------------------------------

void DatasetHeader::SetUaUs(const uint32_t num_u_access_units,
                            dataset_header::UOptions u_opts) {
  UTILS_DIE_IF(!num_u_access_units,
               "Resetting num_u_access_units not supported");
  num_u_access_units_ = num_u_access_units;
  u_options_ = u_opts;
}

// -----------------------------------------------------------------------------

void DatasetHeader::AddClassConfig(dataset_header::MitClassConfig config) {
  UTILS_DIE_IF(
      block_header_on_ != std::nullopt && !block_header_on_->GetMitFlag(),
      "Adding classes without MIT has no effect");
  UTILS_DIE_IF(
      config.GetDescriptorIDs().empty() && block_header_off_ != std::nullopt,
      "Descriptor streams not supplied (block_header_flag)");
  if (!config.GetDescriptorIDs().empty() && block_header_off_ == std::nullopt) {
    config = dataset_header::MitClassConfig(config.GetClassId());
  }
  UTILS_DIE_IF(!mit_configs_.empty() &&
                   mit_configs_.back().GetClassId() >= config.GetClassId(),
               "Class IDs must be in order.");
  mit_configs_.emplace_back(std::move(config));
}

// -----------------------------------------------------------------------------

void DatasetHeader::DisableBlockHeader(
    dataset_header::BlockHeaderOffOptions opts) {
  UTILS_DIE_IF(
      !mit_configs_.empty(),
      "Disabling block header after adding MIT information not supported.");
  block_header_on_ = std::nullopt;
  block_header_off_ = opts;
}

// -----------------------------------------------------------------------------

void DatasetHeader::DisableMit() {
  UTILS_DIE_IF(block_header_on_ == std::nullopt,
               "MIT can only be disabled when block headers are activated");
  block_header_on_ = dataset_header::BlockHeaderOnOptions(
      false, block_header_on_->GetCcFlag());
  mit_configs_.clear();
}

// -----------------------------------------------------------------------------

void DatasetHeader::PatchRefId(const uint8_t old, const uint8_t _new) {
  reference_options_.PatchRefId(old, _new);
}

// -----------------------------------------------------------------------------

void DatasetHeader::PatchId(const uint8_t group_id, const uint16_t set_id) {
  group_id_ = group_id;
  id_ = set_id;
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::meta::BlockHeader> DatasetHeader::decapsulate() const {
  if (block_header_on_ != std::nullopt) {
    return std::make_unique<core::meta::block_header::Enabled>(
        block_header_on_->GetMitFlag(), block_header_on_->GetCcFlag());
  }
  return std::make_unique<core::meta::block_header::Disabled>(
      block_header_off_->GetOrderedBlocksFlag());
}

// -----------------------------------------------------------------------------

void DatasetHeader::PrintDebug(std::ostream& output, const uint8_t depth,
                                const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Dataset Header");
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
