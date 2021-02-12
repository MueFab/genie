/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <utility>
#include <genie/util/runtime-exception.h>

#include "master_index_table.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MasterIndexTable::MasterIndexTable()
    : ac_info(),
      u_ac_info(),
      datasetHeader()
{}


MasterIndexTable::MasterIndexTable(util::BitReader& reader, size_t length)
    : ac_info(),
      u_ac_info(),
      datasetHeader()
{
/*
    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "mitb", "MasterIndexTable is not Found");
*/
    size_t start_pos = reader.getPos();

    // Class MITAccessUnitInfo()
    for (auto seq = 0; seq < datasetHeader->getSeqInfo().getSeqCount(); seq++) {
        ac_info.emplace_back(MITAccessUnitInfo(reader));
    }

    // Class MITUAccessUnitInfo()
    for (auto uau_id=0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        u_ac_info.emplace_back(MITUAccessUnitInfo(reader));
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid MasterIndexTable length!");
}

uint64_t MasterIndexTable::getLength() const {

    //length is first calculated in bits then converted in bytes
    /// Key c(4) Length u(64)
    uint64_t bitlen = (4 * sizeof(char) + 8) * 8 ;   // gen_info

    // data encapsulated in class MITUAccessUnitInfo()
    /// U_ref
    /// U_ref_sequence_id, U_ref_start_position, U_ref_end_position
    /// multiple_signature_base, U_signature_size
    /// U_cluster_signature, num_signatures
    for (auto& u_ac_i : u_ac_info) {
        bitlen += u_ac_i.getBitLength();
    }

    // data encapsulated in class MITAccessUnitInfo()
    /// AU_byte_offset, AU_start_position, AU_end_position
    /// ref_sequence_id, ref_start_position, ref_end_position
    /// extended_AU_start_position, extended_AU_end_position
    /// block_byte_offset
    for (auto& ac_i : ac_info) {
        bitlen += ac_i.getBitLength();
    }

    return bitlen / 8 ;
}

void MasterIndexTable::write(util::BitWriter& bit_writer) const {
    // TODO (Yeremia): Implement this!
    // KLV (Key Length Value) format

    // Key of KLV format
    bit_writer.write("mitb");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    for (auto seq=0; seq < datasetHeader->getSeqInfo().getSeqCount(); seq++) {
        for (auto ci=0; ci < datasetHeader->getBlockHeader().getNumClasses(); ci++){
            if (datasetHeader->getBlockHeader().getClassInfos()[ci].getClid() != core::record::ClassType::CLASS_U) {
                for (auto au_id=0; au_id < datasetHeader->getSeqInfo().getSeqBlocks()[seq]; au_id++) {

                    // MITAccessUnitInfo
                    /// AU_byte_offset, AU_start_position, AU_end_position
                    /// ref_sequence_id, ref_start_position, ref_end_position
                    /// extended_AU_start_position, extended_AU_end_position
                    /// block_byte_offset
                    for (auto &ac_i : ac_info) {
                        ac_i.write(bit_writer);
                    }
                }
            }
        }
    }

    // MITUAccessUnitInfo
    /// U_ref
    /// U_ref_sequence_id, U_ref_start_position, U_ref_end_position
    /// multiple_signature_base, U_signature_size
    /// U_cluster_signature, num_signatures
    for (auto  &u_ac_i : u_ac_info) {
        u_ac_i.write(bit_writer);
    }

}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie