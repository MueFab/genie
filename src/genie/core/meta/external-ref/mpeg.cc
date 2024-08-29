/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/external-ref/mpeg.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta::external_ref {

// ---------------------------------------------------------------------------------------------------------------------

MPEG::MPEG(std::string _ref_uri, ChecksumAlgorithm check, uint16_t group_id, uint16_t id, std::string checksum)
    : ExternalRef(std::move(_ref_uri), check, ReferenceType::MPEGG_REF),
      external_dataset_group_id(group_id),
      external_dataset_id(id),
      ref_checksum(std::move(checksum)) {}

// ---------------------------------------------------------------------------------------------------------------------

MPEG::MPEG(const nlohmann::json& json) : ExternalRef(json) {
    external_dataset_group_id = json["ref_type_mpegg_ref"]["external_dataset_group_id"];
    external_dataset_id = json["ref_type_mpegg_ref"]["external_dataset_id"];
    ref_checksum = json["ref_type_mpegg_ref"]["ref_checksum"];
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json MPEG::toJson() const {
    auto ret = ExternalRef::toJson();
    ret["ref_type_mpegg_ref"]["external_dataset_group_id"] = external_dataset_group_id;
    ret["ref_type_mpegg_ref"]["external_dataset_id"] = external_dataset_id;
    ret["ref_type_mpegg_ref"]["ref_checksum"] = ref_checksum;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t MPEG::getGroupID() const { return external_dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t MPEG::getID() const { return external_dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& MPEG::getChecksum() const { return ref_checksum; }

// ---------------------------------------------------------------------------------------------------------------------

std::string& MPEG::getChecksum() { return ref_checksum; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<RefBase> MPEG::clone() const {
    auto ret = genie::util::make_unique<MPEG>(getURI(), getChecksumAlgo(), external_dataset_group_id,
                                              external_dataset_id, ref_checksum);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta::external_ref

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
