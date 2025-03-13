/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/external_ref.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------

ExternalRef::ExternalRef(std::string ref_uri, const ChecksumAlgorithm check,
                         const ReferenceType ref)
    : ref_uri_(std::move(ref_uri)),
      checksum_alg_(check),
      reference_type_(ref) {}

// -----------------------------------------------------------------------------

ExternalRef::ExternalRef(const nlohmann::json& json)
    : ref_uri_(json["ref_uri"]),
      checksum_alg_(static_cast<ChecksumAlgorithm>(json["checksum_alg"])),
      reference_type_(static_cast<ReferenceType>(json["reference_type"])) {}

// -----------------------------------------------------------------------------

const std::string& ExternalRef::GetUri() const { return ref_uri_; }

// -----------------------------------------------------------------------------

ExternalRef::ChecksumAlgorithm ExternalRef::GetChecksumAlgo() const {
  return checksum_alg_;
}

// -----------------------------------------------------------------------------

ExternalRef::ReferenceType ExternalRef::GetReferenceType() const {
  return reference_type_;
}

// -----------------------------------------------------------------------------

const std::string& ExternalRef::GetKeyName() const {
  static const std::string ret = "external_ref";
  return ret;
}

// -----------------------------------------------------------------------------

nlohmann::json ExternalRef::ToJson() const {
  nlohmann::json ret;
  ret["ref_uri"] = ref_uri_;
  ret["checksum_alg"] = checksum_alg_;
  ret["reference_type"] = reference_type_;
  return ret;
}

// -----------------------------------------------------------------------------

std::string& ExternalRef::GetUri() { return ref_uri_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------