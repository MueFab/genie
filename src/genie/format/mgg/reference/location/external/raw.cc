/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location/external/raw.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

// -----------------------------------------------------------------------------
std::unique_ptr<core::meta::RefBase> Raw::decapsulate() {
  auto ret = std::make_unique<core::meta::external_ref::Raw>(
      std::move(GetUri()),
      static_cast<core::meta::ExternalRef::ChecksumAlgorithm>(
          GetChecksumAlgorithm()));

  for (auto& s : seq_checksums_) {
    ret->AddChecksum(std::move(s));
  }
  return ret;
}

// -----------------------------------------------------------------------------
Raw::Raw(const uint8_t reserved, std::string uri, const ChecksumAlgorithm algo)
    : External(reserved, std::move(uri), algo, RefType::RAW_REF) {}

// -----------------------------------------------------------------------------
Raw::Raw(util::BitReader& reader, const size_t seq_count) : External(reader) {
  for (size_t i = 0; i < seq_count; ++i) {
    seq_checksums_.emplace_back(
        checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())], '\0');
    reader.ReadAlignedBytes(seq_checksums_.back().data(),
                            seq_checksums_.back().size());
  }
}

// -----------------------------------------------------------------------------
Raw::Raw(util::BitReader& reader, const uint8_t reserved, std::string uri,
         const ChecksumAlgorithm algo, const size_t seq_count)
    : External(reserved, std::move(uri), algo, RefType::RAW_REF) {
  for (size_t i = 0; i < seq_count; ++i) {
    seq_checksums_.emplace_back(
        checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())], '\0');
    reader.ReadAlignedBytes(seq_checksums_.back().data(),
                            seq_checksums_.back().length());
  }
}

// -----------------------------------------------------------------------------
const std::vector<std::string>& Raw::GetSeqChecksums() const {
  return seq_checksums_;
}

// -----------------------------------------------------------------------------
void Raw::AddSeqChecksum(std::string checksum) {
  UTILS_DIE_IF(
      checksum.size() !=
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())],
      "Invalid checksum length");
  seq_checksums_.emplace_back(std::move(checksum));
}

// -----------------------------------------------------------------------------
void Raw::Write(util::BitWriter& writer) {
  External::Write(writer);
  for (const auto& s : seq_checksums_) {
    writer.WriteAlignedBytes(s.data(), s.length());
  }
}

// -----------------------------------------------------------------------------
void Raw::AddChecksum(std::string checksum) {
  UTILS_DIE_IF(
      checksum.size() !=
          checksum_sizes_[static_cast<uint8_t>(GetChecksumAlgorithm())],
      "Invalid checksum length");
  seq_checksums_.emplace_back(std::move(checksum));
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
