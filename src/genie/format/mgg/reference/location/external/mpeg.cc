/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location/external/mpeg.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

// -----------------------------------------------------------------------------
Mpeg::Mpeg(const uint8_t reserved, std::string uri, ChecksumAlgorithm algo,
           const uint8_t group_id, const uint16_t dataset_id,
           std::string ref_checksum, const core::MpegMinorVersion version)
    : External(reserved, std::move(uri), algo, RefType::MPEGG_REF),
      version_(version),
      external_dataset_group_id_(group_id),
      external_dataset_id_(dataset_id),
      ref_checksum_(std::move(ref_checksum)) {
  UTILS_DIE_IF(
      version_ == genie::core::MpegMinorVersion::kV1900 &&
          ref_checksum_.size() != checksum_sizes_[static_cast<uint8_t>(algo)],
      "Invalid reference checksum");
}

// -----------------------------------------------------------------------------
Mpeg::Mpeg(util::BitReader& reader, const size_t seq_count,
           const core::MpegMinorVersion version)
    : External(reader),
      version_(version),
      ref_checksum_(
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())], '\0') {
  external_dataset_group_id_ = reader.ReadAlignedInt<uint8_t>();
  external_dataset_id_ = reader.ReadAlignedInt<uint16_t>();
  if (version_ == core::MpegMinorVersion::kV1900) {
    reader.ReadAlignedBytes(ref_checksum_.data(), ref_checksum_.size());
  } else {
    ref_checksum_.clear();
    for (size_t i = 0; i < seq_count; ++i) {
      seq_checksums_.emplace_back(
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())], '\0');
      reader.ReadAlignedBytes(seq_checksums_.back().data(),
                              seq_checksums_.back().size());
    }
  }
}

// -----------------------------------------------------------------------------
Mpeg::Mpeg(util::BitReader& reader, const uint8_t reserved, std::string uri,
           const ChecksumAlgorithm algo, const size_t seq_count,
           const core::MpegMinorVersion version)
    : External(reserved, std::move(uri), algo, RefType::MPEGG_REF),
      version_(version),
      ref_checksum_(
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())], '\0') {
  external_dataset_group_id_ = reader.ReadAlignedInt<uint8_t>();
  external_dataset_id_ = reader.ReadAlignedInt<uint16_t>();
  if (version_ == core::MpegMinorVersion::kV1900) {
    reader.ReadAlignedBytes(ref_checksum_.data(), ref_checksum_.length());
  } else {
    ref_checksum_.clear();
    for (size_t i = 0; i < seq_count; ++i) {
      seq_checksums_.emplace_back(
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())], '\0');
      reader.ReadAlignedBytes(seq_checksums_.back().data(),
                              seq_checksums_.back().length());
    }
  }
}

// -----------------------------------------------------------------------------
uint8_t Mpeg::GetExternalDatasetGroupId() const {
  return external_dataset_group_id_;
}

// -----------------------------------------------------------------------------
uint16_t Mpeg::GetExternalDatasetId() const { return external_dataset_id_; }

// -----------------------------------------------------------------------------
const std::string& Mpeg::GetRefChecksum() const { return ref_checksum_; }

// -----------------------------------------------------------------------------
const std::vector<std::string>& Mpeg::GetSeqChecksums() const {
  return seq_checksums_;
}

// -----------------------------------------------------------------------------
void Mpeg::AddSeqChecksum(std::string checksum) {
  UTILS_DIE_IF(
      checksum.size() !=
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())],
      "Invalid checksum length");
  seq_checksums_.emplace_back(std::move(checksum));
}

// -----------------------------------------------------------------------------
void Mpeg::Write(util::BitWriter& writer) {
  External::Write(writer);
  writer.WriteAlignedInt(external_dataset_group_id_);
  writer.WriteAlignedInt(external_dataset_id_);
  if (version_ == core::MpegMinorVersion::kV1900) {
    writer.WriteAlignedBytes(ref_checksum_.data(), ref_checksum_.length());
  } else {
    for (const auto& s : seq_checksums_) {
      writer.WriteAlignedBytes(s.data(), s.length());
    }
  }
}

// -----------------------------------------------------------------------------
void Mpeg::AddChecksum(std::string checksum) {
  UTILS_DIE_IF(
      checksum.size() !=
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())],
      "Invalid checksum length");
  if (version_ != core::MpegMinorVersion::kV1900) {
    seq_checksums_.emplace_back(std::move(checksum));
  }
}

// -----------------------------------------------------------------------------
std::unique_ptr<core::meta::RefBase> Mpeg::decapsulate() {
  auto ret = std::make_unique<core::meta::external_ref::Mpeg>(
      std::move(GetUri()),
      static_cast<core::meta::ExternalRef::ChecksumAlgorithm>(
          GetChecksumAlgorithm()),
      external_dataset_group_id_, external_dataset_id_,
      std::move(ref_checksum_));
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
