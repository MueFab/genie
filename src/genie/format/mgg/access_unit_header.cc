/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/access_unit_header.h"
#include <map>
#include <sstream>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitHeader::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    static const std::string class_lut[] = {"NONE", "P", "N", "M", "I", "HM", "U"};
    print_offset(output, depth, max_depth, "* Access Unit Header");
    print_offset(output, depth + 1, max_depth, "Access unit ID: " + std::to_string(header.getID()));
    print_offset(output, depth + 1, max_depth, "Access unit class: " + class_lut[static_cast<int>(header.getClass())]);
    print_offset(output, depth + 1, max_depth, "Access unit blocks: " + std::to_string(header.getNumBlocks()));
    print_offset(output, depth + 1, max_depth, "Access unit records: " + std::to_string(header.getReadCount()));
    print_offset(output, depth + 1, max_depth,
                 "Access unit parameter set ID: " + std::to_string(header.getParameterID()));
    if (header.getClass() != core::record::ClassType::CLASS_U) {
        print_offset(output, depth + 1, max_depth,
                     "Access unit reference ID: " + std::to_string(header.getAlignmentInfo().getRefID()));
        print_offset(output, depth + 1, max_depth,
                     "Access unit start pos:  " + std::to_string(header.getAlignmentInfo().getStartPos()));
        print_offset(output, depth + 1, max_depth,
                     "Access unit end pos:  " + std::to_string(header.getAlignmentInfo().getEndPos()));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitHeader::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const AccessUnitHeader&>(info);
    return header == other.header && mit_flag == other.mit_flag;
}

// ---------------------------------------------------------------------------------------------------------------------

const mgb::AUHeader& AccessUnitHeader::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

mgb::AUHeader& AccessUnitHeader::getHeader() { return header; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader() : AccessUnitHeader(mgb::AUHeader(), false) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader(util::BitReader& reader,
                                   const std::map<size_t, core::parameter::EncodingSet>& parameterSets, bool mit)
    : mit_flag(mit) {
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    header = mgb::AUHeader(reader, parameterSets, !mit_flag);
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader(mgb::AUHeader _header, bool _mit_flag)
    : header(std::move(_header)), mit_flag(_mit_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitHeader::box_write(util::BitWriter& bitWriter) const { header.write(bitWriter, !mit_flag); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AccessUnitHeader::getKey() const {
    static const std::string key = "auhd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
