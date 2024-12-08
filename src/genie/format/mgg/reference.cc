/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/format/mgg/reference/location/external.h"
#include "genie/format/mgg/reference/location/internal.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool Reference::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const Reference&>(info);
  return reference_name_ == other.reference_name_ &&
         ref_version_ == other.ref_version_ && sequences_ == other.sequences_ &&
         reference_location_ == other.reference_location_ &&
         version_ == other.version_;
}

// -----------------------------------------------------------------------------
Reference::Reference(util::BitReader& reader,
                     const core::MpegMinorVersion version)
    : ref_version_(0, 0, 0) {
  version_ = version;
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  dataset_group_id_ = reader.ReadAlignedInt<uint8_t>();
  reference_id_ = reader.ReadAlignedInt<uint8_t>();
  reference_name_ = reader.ReadAlignedStringTerminated();
  ref_version_ = reference::Version(reader);
  const auto seq_count = reader.ReadAlignedInt<uint16_t>();
  for (size_t i = 0; i < seq_count; ++i) {
    sequences_.emplace_back(reader, version_);
  }

  reference_location_ =
      reference::Location::factory(reader, seq_count, version);
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------
Reference::Reference(const uint8_t group_id, const uint8_t ref_id,
                     std::string ref_name, const reference::Version ref_version,
                     std::unique_ptr<reference::Location> location,
                     const core::MpegMinorVersion version)
    : dataset_group_id_(group_id),
      reference_id_(ref_id),
      reference_name_(std::move(ref_name)),
      ref_version_(ref_version),
      reference_location_(std::move(location)),
      version_(version) {}

// -----------------------------------------------------------------------------
uint8_t Reference::GetDatasetGroupId() const { return dataset_group_id_; }

// -----------------------------------------------------------------------------
uint8_t Reference::GetReferenceId() const { return reference_id_; }

// -----------------------------------------------------------------------------
const std::string& Reference::GetReferenceName() const {
  return reference_name_;
}

// -----------------------------------------------------------------------------
const reference::Version& Reference::GetRefVersion() const {
  return ref_version_;
}

// -----------------------------------------------------------------------------
const std::vector<reference::Sequence>& Reference::GetSequences() const {
  return sequences_;
}

// -----------------------------------------------------------------------------
const std::string& Reference::GetKey() const {
  static const std::string key = "rfgn";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------
void Reference::AddSequence(reference::Sequence seq, std::string checksum) {
  sequences_.emplace_back(std::move(seq));
  if (reference_location_->IsExternal()) {
    dynamic_cast<reference::location::External&>(*reference_location_)
        .AddChecksum(std::move(checksum));
  }
}

// -----------------------------------------------------------------------------
const reference::Location& Reference::GetLocation() const {
  return *reference_location_;
}

// -----------------------------------------------------------------------------
void Reference::BoxWrite(util::BitWriter& writer) const {
  writer.WriteAlignedInt(dataset_group_id_);
  writer.WriteAlignedInt(reference_id_);
  writer.WriteAlignedBytes(reference_name_.data(), reference_name_.length());
  writer.WriteAlignedInt<uint8_t>(0);
  ref_version_.Write(writer);
  writer.WriteAlignedInt<uint16_t>(static_cast<uint16_t>(sequences_.size()));
  for (const auto& s : sequences_) {
    s.Write(writer);
  }
  reference_location_->Write(writer);
}

// -----------------------------------------------------------------------------
void Reference::PatchId(const uint8_t group_id) {
  dataset_group_id_ = group_id;
}

// -----------------------------------------------------------------------------
void Reference::PatchRefId(const uint8_t old, const uint8_t _new) {
  if (reference_id_ == old) {
    reference_id_ = _new;
  }
}

// -----------------------------------------------------------------------------
core::meta::Reference Reference::decapsulate(std::string meta) {
  std::unique_ptr<core::meta::RefBase> location =
      reference_location_->decapsulate();
  core::meta::Reference ret(std::move(reference_name_), ref_version_.GetMajor(),
                            ref_version_.GetMinor(), ref_version_.GetPatch(),
                            std::move(location), std::move(meta));
  for (auto& s : sequences_) {
    ret.AddSequence(
        core::meta::Sequence(s.GetName(), s.GetLength(), s.GetId()));
  }
  return ret;
}

// -----------------------------------------------------------------------------
Reference::Reference(const uint8_t dataset_group_id, const uint8_t reference_id,
                     core::meta::Reference ref,
                     const core::MpegMinorVersion version)
    : dataset_group_id_(dataset_group_id),
      reference_id_(reference_id),
      reference_name_(std::move(ref.GetName())),
      ref_version_(static_cast<uint16_t>(ref.GetMajorVersion()),
                   static_cast<uint16_t>(ref.GetMinorVersion()),
                   static_cast<uint16_t>(ref.GetPatchVersion())),
      version_(version) {
  for (auto& r : ref.GetSequences()) {
    sequences_.emplace_back(std::move(r), version_);
  }
  reference_location_ =
      reference::Location::ReferenceLocationFactory(ref.MoveBase(), version);
}

// -----------------------------------------------------------------------------
void Reference::PrintDebug(std::ostream& output, const uint8_t depth,
                            const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Reference");
  print_offset(output, depth + 1, max_depth,
               "Dataset group ID: " + std::to_string(dataset_group_id_));
  print_offset(output, depth + 1, max_depth,
               "Reference ID: " + std::to_string(reference_id_));
  print_offset(output, depth + 1, max_depth,
               "Reference name: " + reference_name_);
  print_offset(
      output, depth + 1, max_depth,
      "Reference major version: " + std::to_string(ref_version_.GetMajor()));
  print_offset(
      output, depth + 1, max_depth,
      "Reference minor version: " + std::to_string(ref_version_.GetMinor()));
  print_offset(
      output, depth + 1, max_depth,
      "Reference patch version: " + std::to_string(ref_version_.GetPatch()));
  for (const auto& r : sequences_) {
    std::string s = "Reference sequence: " + r.GetName();
    if (version_ != core::MpegMinorVersion::kV1900) {
      s += " (ID: " + std::to_string(r.GetId()) +
           "; length: " + std::to_string(r.GetLength()) + ")";
    }
    print_offset(output, depth + 1, max_depth, s);
  }
  std::string location;
  if (reference_location_->IsExternal()) {
    location =
        "External at " +
        dynamic_cast<const reference::location::External&>(*reference_location_)
            .GetUri();
  } else {
    const auto& i = dynamic_cast<const reference::location::Internal&>(
        *reference_location_);
    location = "Internal at (Dataset Group " +
               std::to_string(i.GetDatasetGroupId()) + ", Dataset " +
               std::to_string(i.GetDatasetId()) + ")";
  }
  print_offset(output, depth + 1, max_depth, "Reference location: " + location);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
