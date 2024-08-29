/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/descriptor-stream.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(size_t _descriptor_ID, std::string _ds_protection_value)
    : descriptor_ID(_descriptor_ID), DS_protection_value(std::move(_ds_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(const nlohmann::json& obj)
    : descriptor_ID(obj["descriptor_ID"]), DS_protection_value(obj["DS_protection_value"]) {}

// ---------------------------------------------------------------------------------------------------------------------

size_t DescriptorStream::getID() const { return descriptor_ID; }

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json DescriptorStream::toJson() const {
    nlohmann::json ret;
    ret["descriptor_ID"] = descriptor_ID;
    ret["DS_protection_value"] = DS_protection_value;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DescriptorStream::getProtection() const { return DS_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

std::string& DescriptorStream::getProtection() { return DS_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
