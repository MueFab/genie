/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/access_unit_header.h"

#include <map>

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
bool AuHeader::operator==(const AuHeader& other) const {
  return access_unit_id_ == other.access_unit_id_ &&
         num_blocks_ == other.num_blocks_ &&
         parameter_set_id_ == other.parameter_set_id_ &&
         au_type_ == other.au_type_ && reads_count_ == other.reads_count_ &&
         mm_cfg_ == other.mm_cfg_ && ref_cfg_ == other.ref_cfg_ &&
         au_type_u_cfg_ == other.au_type_u_cfg_ &&
         signature_config_ == other.signature_config_;
}

// -----------------------------------------------------------------------------
void AuHeader::Write(util::BitWriter& writer,
                     const bool write_signatures) const {
  writer.WriteBits(access_unit_id_, 32);
  writer.WriteBits(num_blocks_, 8);
  writer.WriteBits(parameter_set_id_, 8);
  writer.WriteBits(static_cast<uint8_t>(au_type_), 4);
  writer.WriteBits(reads_count_, 32);
  if (mm_cfg_) {
    mm_cfg_->Write(writer);
  }
  if (ref_cfg_) {
    ref_cfg_->Write(writer);
  }
  if (write_signatures) {
    if (au_type_u_cfg_) {
      au_type_u_cfg_->Write(writer);
    }
    if (signature_config_) {
      signature_config_->Write(writer);
    }
  }
  writer.FlushBits();
}

// -----------------------------------------------------------------------------
uint32_t AuHeader::GetNumBlocks() const { return num_blocks_; }

// -----------------------------------------------------------------------------
void AuHeader::BlockAdded() { num_blocks_++; }

// -----------------------------------------------------------------------------
AuHeader::AuHeader(
    util::BitReader& bit_reader,
    const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
    const bool read_signatures) {
  UTILS_DIE_IF(!bit_reader.IsByteAligned(), "Bitreader not aligned");
  access_unit_id_ = bit_reader.Read<uint32_t>();
  num_blocks_ = bit_reader.Read<uint8_t>();
  parameter_set_id_ = bit_reader.Read<uint8_t>();
  au_type_ = bit_reader.Read<core::record::ClassType>(4);
  reads_count_ = bit_reader.Read<uint32_t>();
  if (au_type_ == core::record::ClassType::kClassN ||
      au_type_ == core::record::ClassType::kClassM) {
    this->mm_cfg_ = MmCfg(bit_reader);
  }

  if (parameter_sets.at(parameter_set_id_).GetDatasetType() ==
      core::parameter::ParameterSet::DatasetType::kReference) {
    this->ref_cfg_ =
        RefCfg(parameter_sets.at(parameter_set_id_).GetPosSize(), bit_reader);
  }
  if (read_signatures) {
    if (au_type_ != core::record::ClassType::kClassU) {
      this->au_type_u_cfg_ = AuTypeCfg(
          parameter_sets.at(parameter_set_id_).GetPosSize(),
          parameter_sets.at(parameter_set_id_).HasMultipleAlignments(),
          bit_reader);
    } else {
      if (parameter_sets.at(parameter_set_id_).IsSignatureActivated()) {
        uint8_t length = 0;
        if (parameter_sets.at(parameter_set_id_).IsSignatureConstLength()) {
          length =
              parameter_sets.at(parameter_set_id_).GetSignatureConstLength();
        }
        this->signature_config_ = SignatureCfg(
            bit_reader, length,
            GetAlphabetProperties(
                parameter_sets.at(parameter_set_id_).GetAlphabetId())
                .base_bits);
      }
    }
  }
  bit_reader.FlushHeldBits();
  UTILS_DIE_IF(!bit_reader.IsByteAligned(), "Bitreader not aligned");
}

// -----------------------------------------------------------------------------
AuHeader::AuHeader()
    : AuHeader(0, 0, core::record::ClassType::kNone, 0,
               core::parameter::ParameterSet::DatasetType::kNonAligned, 0,
               false, core::AlphabetId::kAcgtn) {}

// -----------------------------------------------------------------------------
AuHeader::AuHeader(
    const uint32_t access_unit_id, const uint8_t parameter_set_id,
    const core::record::ClassType au_type, const uint32_t reads_count,
    const core::parameter::ParameterSet::DatasetType dataset_type,
    const uint8_t pos_size, const bool signature_flag,
    const core::AlphabetId alphabet)
    : access_unit_id_(access_unit_id),
      num_blocks_(0),
      parameter_set_id_(parameter_set_id),
      au_type_(au_type),
      reads_count_(reads_count) {
  if (au_type_ == core::record::ClassType::kClassN ||
      au_type_ == core::record::ClassType::kClassM) {
    mm_cfg_ = MmCfg();
  }
  if (dataset_type == core::parameter::ParameterSet::DatasetType::kReference) {
    ref_cfg_ = RefCfg(pos_size);
  }
  if (au_type_ != core::record::ClassType::kClassU) {
    au_type_u_cfg_ = AuTypeCfg(pos_size);
  } else {
    if (signature_flag) {
      signature_config_ =
          SignatureCfg(GetAlphabetProperties(alphabet).base_bits);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
