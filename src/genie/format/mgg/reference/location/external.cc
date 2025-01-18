/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location/external.h"

#include <memory>
#include <string>
#include <utility>

#include "genie/format/mgg/reference/location/external/fasta.h"
#include "genie/format/mgg/reference/location/external/mpeg.h"
#include "genie/format/mgg/reference/location/external/raw.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference::location {

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

std::unique_ptr<Location> External::factory(util::BitReader& reader,
                                            uint8_t reserved, size_t seq_count,
                                            core::MpegMinorVersion version) {
  std::string ref_uri = reader.ReadAlignedStringTerminated();
  auto checksum_algo = reader.ReadAlignedInt<ChecksumAlgorithm>();
  switch (reader.ReadAlignedInt<RefType>()) {
    case RefType::MPEGG_REF:
      return std::make_unique<external::Mpeg>(reader, reserved,
                                              std::move(ref_uri), checksum_algo,
                                              seq_count, version);
    case RefType::RAW_REF:
      return std::make_unique<external::Raw>(
          reader, reserved, std::move(ref_uri), checksum_algo, seq_count);
    case RefType::FASTA_REF:
      return std::make_unique<external::Fasta>(
          reader, reserved, std::move(ref_uri), checksum_algo, seq_count);
    default:
      UTILS_DIE("Unknown ref type");
  }
}

// -----------------------------------------------------------------------------

External::External(const uint8_t reserved, std::string uri,
                   const ChecksumAlgorithm algo, const RefType type)
    : Location(reserved, true),
      uri_(std::move(uri)),
      checksum_algo_(algo),
      reference_type_(type) {}

// -----------------------------------------------------------------------------

External::External(util::BitReader& reader) : Location(reader) {
  uri_ = reader.ReadAlignedStringTerminated();
  checksum_algo_ = reader.ReadAlignedInt<ChecksumAlgorithm>();
  reference_type_ = reader.ReadAlignedInt<RefType>();
}

// -----------------------------------------------------------------------------

External::External(util::BitReader& reader, const uint8_t reserved)
    : Location(reserved, true) {
  uri_ = reader.ReadAlignedStringTerminated();
  checksum_algo_ = reader.ReadAlignedInt<ChecksumAlgorithm>();
  reference_type_ = reader.ReadAlignedInt<RefType>();
}

// -----------------------------------------------------------------------------

const std::string& External::GetUri() const { return uri_; }

// -----------------------------------------------------------------------------

External::ChecksumAlgorithm External::GetChecksumAlgorithm() const {
  return checksum_algo_;
}

// -----------------------------------------------------------------------------

External::RefType External::GetReferenceType() const { return reference_type_; }

// -----------------------------------------------------------------------------

void External::Write(util::BitWriter& writer) {
  Location::Write(writer);
  writer.WriteAlignedBytes(uri_.data(), uri_.length());
  writer.WriteAlignedInt('\0');
  writer.WriteAlignedInt(checksum_algo_);
  writer.WriteAlignedInt(reference_type_);
}

// -----------------------------------------------------------------------------

std::string& External::GetUri() { return uri_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
