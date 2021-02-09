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
    for (auto seq=0; seq < datasetHeader->getSeqInfo().getSeqCount(); seq++) {
        auto value_length = reader.read<size_t>();
        ac_info.emplace_back(reader, value_length);
    }

    // Class MITUAccessUnitInfo()
    for (auto uau_id=0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        auto value_length = reader.read<size_t>();
        ac_info.emplace_back(reader, value_length);
    }

/*
    for (auto seq=0; seq < datasetHeader->getSeqInfo().getSeqCount(); seq++) {
            for (auto ci = 0; ci < datasetHeader->getBlockHeader().getNumClasses(); ci++) {
                if (datasetHeader->getBlockHeader().getClassInfos()[ci].getClid() != core::record::ClassType::CLASS_U) {
                    for (auto au_id = 0; au_id < datasetHeader->getSeqInfo().getSeqBlocks()[seq]; au_id++) {
                        /// AU_byte_offset[seq][ci]][au_id] u(byteOffsetSize)
                        AU_byte_offset = reader.read<uint64_t>((uint8_t) byte_offset_size_flag);

                        /// AU_start_position[seq][ci]][au_id] u(posSize)
                        /// AU_end_position[seq][ci]][au_id] u(posSize)
                        AU_start_position = reader.read_b((uint8_t) pos_40_size_flag);
                        AU_end_position = reader.read_b((uint8_t) pos_40_size_flag);

                        if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
                            /// ref_seq_id u(16)
                            ref_sequence_id = reader.read<uint16_t>();

                            /// ref_start_position[seq][ci]][au_id] u(posSize)
                            /// ref_end_position[seq][ci]][au_id] u(posSize)
                            ref_start_position = reader.read_b((uint8_t) pos_40_size_flag);
                            ref_end_position = reader.read_b((uint8_t) pos_40_size_flag);
                        }

                        if (datasetHeader->getMultipleAlignmentFlag()) {
                            /// extended_AU_start_position[seq][ci]][au_id] u(posSize)
                            /// extended_AU_end_position[seq][ci]][au_id] u(posSize)
                            extended_AU_start_position = reader.read_b((uint8_t) pos_40_size_flag);
                            extended_AU_end_position = reader.read_b((uint8_t) pos_40_size_flag);

                            if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
                                for (auto di = 0; di < datasetHeader->getBlockHeader().getClassInfos()[ci].getDescriptorIDs().size(); di++) {
                                    /// block_byte_offset u(byteOffsetSize)
                                    for (auto& offset : block_byte_offset) {
                                        block_byte_offset.emplace_back(reader.read<uint64_t>((uint8_t) byte_offset_size_flag));

                            }
                        }


                }
            }
        }
    }

    for (auto uau_id=0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        ///AU_byte_offset[uau_id] u(byteOffsetSize)
        AU_byte_offset = reader.read_b((uint8_t) byte_offset_size_flag);

        if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
            U_ref_sequence_id = reader.read<uint16_t>();              /// U_ref_sequence_id[uau_id]  u(16)
            U_ref_start_position = reader.read_b((uint8_t) pos_40_size_flag);    /// U_ref_start_position[uau_id] u(posSize)
            U_ref_end_position = reader.read_b((uint8_t) pos_40_size_flag);        /// U_ref_end_position[uau_id] u(posSize)datasetHeader->getDatasetType()
        }
        else {
            if (multiple_signature_base != 0) {  //TODO(Raouf): check getFunctions
                U_cluster_signature = reader.read_b((uint8_t) U_signature_size);    /// U_cluster_signature[uau_id][0] u(U_signature_size)
                if (U_cluster_signature[uau_id][0] != ((1 <<multiple_signature_base) -1 )) {
                    for (auto i=0; i < (U_signature_size; i++) {
                        U_cluster_signature = reader.read_b((uint8_t) U_signature_size);     /// U_cluster_signature[uau_id][i]
                    }
                }
                else {
                    num_signatures = reader.read<uint16_t>();      /// num_signatures  u(16)
                    for(auto i=0; i < num_signatures; i++) {
                        U_cluster_signature +=  reader.read_b((uint8_t) U_signature_size);     /// U_cluster_signature[uau_id][i]
                    }
                }
            }
        }

        // byte aligned

        if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
            for (auto di=0; di < datasetHeader->getBlockHeader().getClassInfos()[datasetHeader->getBlockHeader().getNumClasses() - 1].getDescriptorIDs().size(); di++) {
                block_byte_offset.emplace_back(reader.read<uint64_t>());    /// block_byte_offset[uau_id][di]
            }
        }
    }
*/
    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DatasetGroup length!");
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


    bitlen /= 8;   //byte_conversion
    return bitlen;
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