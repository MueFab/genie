/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "tag-string-list.h"
#include <genie/util/make-unique.h>
#include <genie/util/tokenize.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

TagStringList::TagStringList(std::string _name, std::vector<std::string> _data, char _delimiter)
    : TagBase(std::move(_name)), data(std::move(_data)), delimiter(_delimiter) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string TagStringList::toString() const {
    std::string ret;
    for (const auto& s : data) {
        ret += s;
        ret += delimiter;
    }
    ret.pop_back();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<TagBase> TagStringList::fromString(std::string name, const std::string& string, char delimiter) {
    return util::make_unique<TagStringList>(std::move(name), util::tokenize(string, delimiter), delimiter);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------