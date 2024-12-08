/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location.h"

#include <memory>
#include <utility>

#include "genie/format/mgg/reference/location/external.h"
#include "genie/format/mgg/reference/location/external/fasta.h"
#include "genie/format/mgg/reference/location/external/mpeg.h"
#include "genie/format/mgg/reference/location/external/raw.h"
#include "genie/format/mgg/reference/location/internal.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference {

// -----------------------------------------------------------------------------
std::unique_ptr<Location> Location::factory(
    util::BitReader& reader, const size_t seq_count,
    const core::MpegMinorVersion version) {
  auto reserved = reader.Read<uint8_t>(7);
  if (const bool external_ref_flag = reader.Read<bool>(1); !external_ref_flag) {
    return std::make_unique<location::Internal>(reader, reserved);
  }
  return location::External::factory(reader, reserved, seq_count, version);
}

// -----------------------------------------------------------------------------
std::unique_ptr<Location> Location::ReferenceLocationFactory(
    const std::unique_ptr<core::meta::RefBase>& base,
    core::MpegMinorVersion version) {
  if (dynamic_cast<core::meta::external_ref::Mpeg*>(base.get()) != nullptr) {
    const auto ref = dynamic_cast<core::meta::external_ref::Mpeg*>(base.get());
    auto ret = std::make_unique<location::external::Mpeg>(
        static_cast<uint8_t>(0), std::move(ref->GetUri()),
        static_cast<location::External::ChecksumAlgorithm>(
            ref->GetChecksumAlgo()),
        static_cast<uint8_t>(ref->GetGroupId()), ref->GetId(),
        std::move(ref->GetChecksum()), version);
    return ret;
  }
  if (dynamic_cast<core::meta::external_ref::Fasta*>(base.get()) != nullptr) {
    const auto ref = dynamic_cast<core::meta::external_ref::Fasta*>(base.get());
    auto ret = std::make_unique<location::external::Fasta>(
        static_cast<uint8_t>(0), std::move(ref->GetUri()),
        static_cast<location::External::ChecksumAlgorithm>(
            ref->GetChecksumAlgo()));
    for (const auto& s : ref->GetChecksums()) {
      ret->AddChecksum(s);
    }
    return ret;
  }
  if (dynamic_cast<core::meta::external_ref::Raw*>(base.get()) != nullptr) {
    const auto ref = dynamic_cast<core::meta::external_ref::Raw*>(base.get());
    auto ret = std::make_unique<location::external::Raw>(
        static_cast<uint8_t>(0), std::move(ref->GetUri()),
        static_cast<location::External::ChecksumAlgorithm>(
            ref->GetChecksumAlgo()));
    for (const auto& s : ref->GetChecksums()) {
      ret->AddChecksum(s);
    }
    return ret;
  }
  if (dynamic_cast<core::meta::InternalRef*>(base.get()) != nullptr) {
    const auto ref = dynamic_cast<core::meta::InternalRef*>(base.get());
    return std::make_unique<location::Internal>(
        static_cast<uint8_t>(0), static_cast<uint8_t>(ref->GetGroupId()),
        ref->GetId());
  }
  UTILS_DIE("Unknown reference location type");
}

// -----------------------------------------------------------------------------
Location::Location(const uint8_t reserved, const bool external_ref_flag)
    : reserved_(reserved), external_ref_flag_(external_ref_flag) {}

// -----------------------------------------------------------------------------
Location::Location(util::BitReader& reader) {
  reserved_ = reader.Read<uint8_t>(7);
  external_ref_flag_ = reader.Read<bool>(1);
}

// -----------------------------------------------------------------------------
bool Location::IsExternal() const { return external_ref_flag_; }

// -----------------------------------------------------------------------------
void Location::Write(util::BitWriter& writer) {
  writer.WriteBits(reserved_, 7);
  writer.WriteBits(external_ref_flag_, 1);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
