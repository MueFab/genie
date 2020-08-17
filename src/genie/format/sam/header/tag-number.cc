/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "tag-number.h"
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

TagNumber::TagNumber(std::string _name, uint64_t _data) : TagBase(std::move(_name)), data(_data) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string TagNumber::toString() const { return std::to_string(data); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<TagBase> TagNumber::fromString(std::string name, const std::string& string) {
    return util::make_unique<TagNumber>(std::move(name), std::stoi(string));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------