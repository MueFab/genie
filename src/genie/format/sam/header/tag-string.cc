/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "tag-string.h"
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

TagString::TagString(std::string _name, std::string _data) : TagBase(std::move(_name)), data(std::move(_data)) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string TagString::toString() const { return data; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<TagBase> TagString::fromString(std::string name, const std::string& string) {
    return util::make_unique<TagString>(std::move(name), string);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------