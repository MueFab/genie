/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/external-ref.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

// ---------------------------------------------------------------------------------------------------------------------

ExternalRef::ExternalRef(std::string _ref_uri, ChecksumAlgorithm check, ReferenceType ref)
    : ref_uri(std::move(_ref_uri)), checksum_alg(check), reference_type(ref) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalRef::ExternalRef(const nlohmann::json& json)
    : ref_uri(json["ref_uri"]),
      checksum_alg(static_cast<ChecksumAlgorithm>(json["checksum_alg"])),
      reference_type(static_cast<ReferenceType>(json["reference_type"])) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ExternalRef::getURI() const { return ref_uri; }

// ---------------------------------------------------------------------------------------------------------------------

ExternalRef::ChecksumAlgorithm ExternalRef::getChecksumAlgo() const { return checksum_alg; }

// ---------------------------------------------------------------------------------------------------------------------

ExternalRef::ReferenceType ExternalRef::getReferenceType() const { return reference_type; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ExternalRef::getKeyName() const {
    static const std::string ret = "external_ref";
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json ExternalRef::toJson() const {
    nlohmann::json ret;
    ret["ref_uri"] = ref_uri;
    ret["checksum_alg"] = checksum_alg;
    ret["reference_type"] = reference_type;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
