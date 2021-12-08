/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/access-unit.h"
#include <string>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(size_t id, std::string information, std::string protection)
    : access_unit_ID(id), AU_information_value(std::move(information)), AU_protection_value(std::move(protection)) {
    UTILS_DIE_IF(AU_information_value.empty() && AU_protection_value.empty(), "Empty AU metadata");
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(const nlohmann::json& obj) : access_unit_ID(obj["access_unit_ID"]) {
    AU_information_value = obj.at("AU_information_value");
    AU_protection_value = obj.at("AU_protection_value");
    UTILS_DIE_IF(AU_information_value.empty() && AU_protection_value.empty(), "Empty AU metadata");
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json AccessUnit::toJson() const {
    nlohmann::json ret;
    ret["access_unit_ID"] = access_unit_ID;
    ret["AU_information_value"] = AU_information_value;
    ret["AU_protection_value"] = AU_protection_value;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::getID() const { return access_unit_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AccessUnit::getInformation() const { return AU_information_value; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AccessUnit::getProtection() const { return AU_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
