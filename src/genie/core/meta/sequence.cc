/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/sequence.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(std::string _name, uint64_t _length, uint16_t _id)
    : name(std::move(_name)), length(_length), id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(const nlohmann::json& json) : name(json["name"]), length(json["length"]), id(json["id"]) {}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Sequence::toJson() const {
    nlohmann::json ret;
    ret["name"] = name;
    ret["length"] = length;
    ret["id"] = id;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Sequence::getName() const { return name; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Sequence::getLength() const { return length; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Sequence::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------