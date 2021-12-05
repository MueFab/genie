/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/external-ref/raw.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {
namespace external_ref {

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(std::string _ref_uri, ChecksumAlgorithm check, std::string firstChecksum)
    : ExternalRef(std::move(_ref_uri), check, ReferenceType::RAW_REF),
      ref_type_other_checksums({std::move(firstChecksum)}) {}

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(const nlohmann::json& json) : ExternalRef(json) {
    for (const auto& s : json["ref_type_other_checksums"]) {
        ref_type_other_checksums.push_back(s);
    }
    UTILS_DIE_IF(ref_type_other_checksums.empty(), "No checksums supplied.");
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Raw::getChecksums() const { return ref_type_other_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void Raw::addChecksum(std::string checksum) { ref_type_other_checksums.emplace_back(std::move(checksum)); }

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Raw::toJson() const {
    auto ret = ExternalRef::toJson();
    ret["ref_type_other_checksums"] = ref_type_other_checksums;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace external_ref
}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
