/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/dataset-group.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetGroup::getID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroup::getVersion() const { return version_number; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroup::getInformation() const { return DG_metadata_value; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroup::getProtection() const { return DG_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(uint16_t id, uint8_t version, std::string meta, std::string protection)
    : dataset_group_ID(id),
      version_number(version),
      DG_metadata_value(std::move(meta)),
      DG_protection_value(std::move(protection)) {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(const nlohmann::json& json)
    : dataset_group_ID(json["dataset_group_ID"]),
      version_number(json["version_number"]),
      DG_metadata_value(json["DG_metadata_value"]),
      DG_protection_value(json["DG_protection_value"]) {}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json DatasetGroup::toJson() const {
    nlohmann::json ret;
    ret["dataset_group_ID"] = dataset_group_ID;
    ret["version_number"] = version_number;
    ret["DG_metadata_value"] = DG_metadata_value;
    ret["DG_protection_value"] = DG_protection_value;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
