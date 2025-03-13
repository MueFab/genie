/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/external_ref/fasta.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"
#include "genie/util/string_helpers.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta::external_ref {

// -----------------------------------------------------------------------------

Fasta::Fasta(std::string ref_uri, const ChecksumAlgorithm check)
    : ExternalRef(std::move(ref_uri), check, ReferenceType::kFastaRef) {}

// -----------------------------------------------------------------------------

Fasta::Fasta(const nlohmann::json& json) : ExternalRef(json) {
  for (const auto& s : json["ref_type_other_checksums"]) {
    ref_type_other_checksums_.push_back(util::FromHex(s));
  }
  UTILS_DIE_IF(ref_type_other_checksums_.empty(), "No checksums supplied.");
}

// -----------------------------------------------------------------------------

const std::vector<std::string>& Fasta::GetChecksums() const {
  return ref_type_other_checksums_;
}

// -----------------------------------------------------------------------------

void Fasta::AddChecksum(std::string checksum) {
  ref_type_other_checksums_.emplace_back(std::move(checksum));
}

// -----------------------------------------------------------------------------

nlohmann::json Fasta::ToJson() const {
  auto ret = ExternalRef::ToJson();
  std::vector<std::string> hexed = ref_type_other_checksums_;
  for (auto& s : hexed) {
    s = util::ToHex(s);
  }
  ret["ref_type_other_checksums"] = hexed;
  return ret;
}

// -----------------------------------------------------------------------------

std::vector<std::string>& Fasta::GetChecksums() {
  return ref_type_other_checksums_;
}

// -----------------------------------------------------------------------------

std::unique_ptr<RefBase> Fasta::Clone() const {
  auto ret = std::make_unique<Fasta>(GetUri(), GetChecksumAlgo());
  for (auto& c : ref_type_other_checksums_) {
    ret->AddChecksum(c);
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::external_ref

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------