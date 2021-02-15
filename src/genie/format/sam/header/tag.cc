/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/header/tag.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

TagBase::TagBase(std::string _name) : name(std::move(_name)) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& TagBase::getName() const { return name; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& getSortingOrderString() {
    static const std::vector<std::string> SORTING_ORDER_STRING = {"unknown", "unsorted", "queryname", "coordinate"};
    return SORTING_ORDER_STRING;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& getGroupingOrderString() {
    static const std::vector<std::string> GROUPING_ORDER_STRING = {"none", "query", "reference"};
    return GROUPING_ORDER_STRING;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& getTopologyString() {
    static const std::vector<std::string> TOPOLOGY_STRING = {"linear", "circular"};
    return TOPOLOGY_STRING;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& getPlatformString() {
    static const std::vector<std::string> TOPOLOGY_STRING = {"CAPILLARY", "LS454",      "ILLUMINA", "SOLID",
                                                             "HELICOS",   "IONTORRENT", "ONT",      "PACBIO"};
    return TOPOLOGY_STRING;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string generateEnumRegex(const std::vector<std::string>& values) {
    std::string ret("(");
    for (const auto& v : values) {
        ret += v;
        ret += '|';
    }
    ret.pop_back();
    ret += ")";
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& getSAMRegex() {
    static const std::string regex = "[0-9A-Za-z!#$%&+./:;?@^_|~-][0-9A-Za-z!#$%&*+./:;=?@^_|~-]*";
    return regex;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
