/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/extended_au_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

MITExtendedAUInfo::MITExtendedAUInfo()
    : extended_AU_start_position(0), extended_AU_end_position(0), pos_40_flag(DatasetHeader::Pos40SizeFlag::OFF) {}

// ---------------------------------------------------------------------------------------------------------------------

MITExtendedAUInfo::MITExtendedAUInfo(uint64_t start_pos, uint64_t end_pos, DatasetHeader::Pos40SizeFlag flag)
    : extended_AU_start_position(start_pos), extended_AU_end_position(end_pos), pos_40_flag(flag) {}

// ---------------------------------------------------------------------------------------------------------------------

MITExtendedAUInfo::MITExtendedAUInfo(util::BitReader& reader) : pos_40_flag(DatasetHeader::Pos40SizeFlag::OFF) {
    /// extended_AU_start_position[seq][ci]][au_id] u(posSize)
    /// extended_AU_end_position[seq][ci]][au_id] u(posSize)
    extended_AU_start_position = reader.read<uint64_t>((uint8_t)pos_40_flag);
    extended_AU_end_position = reader.read<uint64_t>((uint8_t)pos_40_flag);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MITExtendedAUInfo::getBitLength() const {
    /// extended_AU_start_position[seq][ci]][au_id] u(posSize)
    /// extended_AU_end_position[seq][ci]][au_id] u(posSize)
    uint64_t bitlen = (uint64_t)pos_40_flag;
    bitlen += (uint64_t)pos_40_flag;

    return bitlen;
}

// ---------------------------------------------------------------------------------------------------------------------

void MITExtendedAUInfo::write(util::BitWriter& bit_writer) const {
    // extended_AU_start_position u(posSize)
    bit_writer.write(extended_AU_start_position, (uint8_t)pos_40_flag);

    // extended_AU_start_position u(posSize)
    bit_writer.write(extended_AU_end_position, (uint8_t)pos_40_flag);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
