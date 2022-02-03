/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/access_unit_header.h"
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitHeader::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const AccessUnitHeader&>(info);
    return header == other.header && mit_flag == other.mit_flag;
}

// ---------------------------------------------------------------------------------------------------------------------

const genie::format::mgb::AUHeader& AccessUnitHeader::getHeader() const { return header; }

genie::format::mgb::AUHeader& AccessUnitHeader::getHeader() { return header; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader() : AccessUnitHeader(genie::format::mgb::AUHeader(), false) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader(util::BitReader& reader,
                                   const std::map<size_t, core::parameter::EncodingSet>& parameterSets, bool mit)
    : mit_flag(mit) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.readBypassBE<uint64_t>();
    header = genie::format::mgb::AUHeader(reader, parameterSets, !mit_flag);
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitHeader::AccessUnitHeader(genie::format::mgb::AUHeader _header, bool _mit_flag)
    : header(std::move(_header)), mit_flag(_mit_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitHeader::box_write(genie::util::BitWriter& bitWriter) const {
    header.write(bitWriter, !mit_flag);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AccessUnitHeader::getKey() const {
    static const std::string key = "auhd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
