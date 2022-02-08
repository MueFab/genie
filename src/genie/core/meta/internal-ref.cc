/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/internal-ref.h"
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

// ---------------------------------------------------------------------------------------------------------------------

uint16_t InternalRef::getGroupID() const { return internal_dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t InternalRef::getID() const { return internal_dataset_ID; }

// ---------------------------------------------------------------------------------------------------------------------

InternalRef::InternalRef(const nlohmann::json& json)
    : internal_dataset_group_id(json["internal_dataset_group_id"]), internal_dataset_ID(json["internal_dataset_ID"]) {}

// ---------------------------------------------------------------------------------------------------------------------

InternalRef::InternalRef(uint16_t group_id, uint16_t id)
    : internal_dataset_group_id(group_id), internal_dataset_ID(id) {}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json InternalRef::toJson() const {
    nlohmann::json ret;
    ret["internal_dataset_group_id"] = internal_dataset_group_id;
    ret["internal_dataset_ID"] = internal_dataset_ID;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& InternalRef::getKeyName() const {
    static const std::string ret = "internal_ref";
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<RefBase> InternalRef::clone() const {
    auto ret = genie::util::make_unique<InternalRef>(internal_dataset_group_id, internal_dataset_ID);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
