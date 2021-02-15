/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/header/tag-version.h"
#include <string>
#include <utility>
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

TagVersion::TagVersion(std::string _name, std::array<uint8_t, 2> _data) : TagBase(std::move(_name)), data(_data) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string TagVersion::toString() const { return std::to_string(data[0]) + "." + std::to_string(data[1]); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<TagBase> TagVersion::fromString(std::string name, const std::string& string) {
    const auto pos = string.find('.');
    std::array<uint8_t, 2> _data = {static_cast<unsigned char>(std::stoi(string.substr(0, pos))),
                                    static_cast<unsigned char>(std::stoi(string.substr(pos + 1)))};
    return util::make_unique<TagVersion>(std::move(name), _data);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
