/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/reference.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/meta/external_ref/fasta.h"
#include "genie/core/meta/external_ref/mpeg.h"
#include "genie/core/meta/external_ref/raw.h"
#include "genie/core/meta/internal_ref.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------

Reference::Reference(std::string name, const uint32_t major,
                     const uint32_t minor, const uint32_t patch,
                     std::unique_ptr<RefBase> base, std::string metadata)
    : reference_name_(std::move(name)),
      reference_major_version_(major),
      reference_minor_version_(minor),
      reference_patch_version_(patch),
      ref_(std::move(base)),
      reference_metadata_value_(std::move(metadata)) {}

// -----------------------------------------------------------------------------

Reference::Reference(const nlohmann::json& json)
    : reference_name_(json["reference_name"]),
      reference_major_version_(json["reference_major_version"]),
      reference_minor_version_(json["reference_minor_version"]),
      reference_patch_version_(json["reference_patch_version"]),
      reference_metadata_value_(json["reference_metadata_value"]) {
  if (json.contains("seqs")) {
    for (const auto& s : json["seqs"]) {
      seqs_.emplace_back(s);
    }
  }
  if (json.contains("external_ref")) {
    switch (static_cast<ExternalRef::ReferenceType>(
        json["external_ref"]["reference_type"])) {
      case ExternalRef::ReferenceType::kFastaRef:
        ref_ = std::make_unique<external_ref::Fasta>(json["external_ref"]);
        break;
      case ExternalRef::ReferenceType::kMpeggRef:
        ref_ = std::make_unique<external_ref::Mpeg>(json["external_ref"]);
        break;
      case ExternalRef::ReferenceType::kRawRef:
        ref_ = std::make_unique<external_ref::Raw>(json["external_ref"]);
        break;
      default:
        UTILS_DIE("Unknown external reference type.");
    }

  } else {
    ref_ = std::make_unique<InternalRef>(json["internal_ref"]);
  }
}

// -----------------------------------------------------------------------------

nlohmann::json Reference::ToJson() const {
  nlohmann::json ret;
  ret["reference_name"] = reference_name_;
  ret["reference_major_version"] = reference_major_version_;
  ret["reference_minor_version"] = reference_minor_version_;
  ret["reference_patch_version"] = reference_patch_version_;
  ret[ref_->GetKeyName()] = ref_->ToJson();
  ret["reference_metadata_value"] = reference_metadata_value_;
  for (const auto& s : seqs_) {
    ret["seqs"].push_back(s.ToJson());
  }
  return ret;
}

// -----------------------------------------------------------------------------

const std::string& Reference::GetName() const { return reference_name_; }

// -----------------------------------------------------------------------------

uint32_t Reference::GetMajorVersion() const { return reference_major_version_; }

// -----------------------------------------------------------------------------

uint32_t Reference::GetMinorVersion() const { return reference_minor_version_; }

// -----------------------------------------------------------------------------

uint32_t Reference::GetPatchVersion() const { return reference_patch_version_; }

// -----------------------------------------------------------------------------

const std::vector<Sequence>& Reference::GetSequences() const { return seqs_; }

// -----------------------------------------------------------------------------

void Reference::AddSequence(Sequence s) { seqs_.emplace_back(std::move(s)); }

// -----------------------------------------------------------------------------

std::unique_ptr<RefBase> Reference::MoveBase() { return std::move(ref_); }

// -----------------------------------------------------------------------------

const RefBase& Reference::GetBase() const { return *ref_; }

// -----------------------------------------------------------------------------

const std::string& Reference::GetInformation() const {
  return reference_metadata_value_;
}

// -----------------------------------------------------------------------------

std::string& Reference::GetInformation() { return reference_metadata_value_; }

// -----------------------------------------------------------------------------

std::vector<Sequence>& Reference::GetSequences() { return seqs_; }

// -----------------------------------------------------------------------------

std::string& Reference::GetName() { return reference_name_; }

// -----------------------------------------------------------------------------

Reference::Reference(const Reference& ref)
    : reference_name_(ref.reference_name_),
      reference_major_version_(ref.reference_major_version_),
      reference_minor_version_(ref.reference_minor_version_),
      reference_patch_version_(ref.reference_patch_version_),
      seqs_(ref.seqs_),
      ref_(ref.ref_->Clone()),
      reference_metadata_value_(ref.reference_metadata_value_) {}

// -----------------------------------------------------------------------------

Reference::Reference(Reference&& ref) noexcept
    : reference_name_(std::move(ref.reference_name_)),
      reference_major_version_(ref.reference_major_version_),
      reference_minor_version_(ref.reference_minor_version_),
      reference_patch_version_(ref.reference_patch_version_),
      seqs_(std::move(ref.seqs_)),
      ref_(std::move(ref.ref_)),
      reference_metadata_value_(std::move(ref.reference_metadata_value_)) {}

// -----------------------------------------------------------------------------

Reference& Reference::operator=(const Reference& ref) {
  reference_name_ = ref.reference_name_;
  reference_major_version_ = ref.reference_major_version_;
  reference_minor_version_ = ref.reference_minor_version_;
  reference_patch_version_ = ref.reference_patch_version_;
  seqs_ = ref.seqs_;
  ref_ = ref.ref_->Clone();
  reference_metadata_value_ = ref.reference_metadata_value_;
  return *this;
}

// -----------------------------------------------------------------------------

Reference& Reference::operator=(Reference&& ref) noexcept {
  reference_name_ = std::move(ref.reference_name_);
  reference_major_version_ = ref.reference_major_version_;
  reference_minor_version_ = ref.reference_minor_version_;
  reference_patch_version_ = ref.reference_patch_version_;
  seqs_ = std::move(ref.seqs_);
  ref_ = std::move(ref.ref_);
  reference_metadata_value_ = std::move(ref.reference_metadata_value_);
  return *this;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
