/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/external-ref/fasta.h"
#include <utility>
#include "genie/util/runtime-exception.h"
#include "genie/util/string-helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {
namespace external_ref {

// ---------------------------------------------------------------------------------------------------------------------

Fasta::Fasta(std::string _ref_uri, ChecksumAlgorithm check)
    : ExternalRef(std::move(_ref_uri), check, ReferenceType::FASTA_REF) {}

// ---------------------------------------------------------------------------------------------------------------------

Fasta::Fasta(const nlohmann::json& json) : ExternalRef(json) {
    for (const auto& s : json["ref_type_other_checksums"]) {
        ref_type_other_checksums.push_back(util::fromHex(s));
    }
    UTILS_DIE_IF(ref_type_other_checksums.empty(), "No checksums supplied.");
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Fasta::getChecksums() const { return ref_type_other_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void Fasta::addChecksum(std::string checksum) { ref_type_other_checksums.emplace_back(std::move(checksum)); }

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Fasta::toJson() const {
    auto ret = ExternalRef::toJson();
    std::vector<std::string> hexed = ref_type_other_checksums;
    for (auto& s : hexed) {
        s = util::toHex(s);
    }
    ret["ref_type_other_checksums"] = hexed;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string>& Fasta::getChecksums() { return ref_type_other_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<RefBase> Fasta::clone() const {
    auto ret = genie::util::make_unique<Fasta>(getURI(), getChecksumAlgo());
    for (auto& c : ref_type_other_checksums) {
        ret->addChecksum(c);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace external_ref
}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
