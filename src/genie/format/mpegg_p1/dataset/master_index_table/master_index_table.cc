    /**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/runtime-exception.h>

#include "master_index_table.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MasterIndexTable::MasterIndexTable() = default;

uint64_t MasterIndexTable::getLength() const {

    //length is first calculated in bits then converted in bytes

    // Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8) * 8 ;   // gen_info

    for (auto seq=0; seq < datasetHeader->seq_info.getSeqCount(); seq++) {
        for (auto ci=0; ci < datasetHeader->block_header.getNumClasses(); ci++){
            if (datasetHeader->block_header.getClassInfos().getClid()[ci] != core::record::ClassType::CLASS_U) {
                for (auto au_id=0; au_id < datasetHeader->seq_info.getSeqBlocks()[seq]; au_id++) {

                    /// AU_byte_offset[seq][ci]][au_id]
                    len += (uint64_t) ac_info.getByteOffsetSizeFlag();

                    /// AU_start_position[seq][ci]][au_id]
                    /// AU_end_position[seq][ci]][au_id]
                    len += (uint64_t) ac_info.getPos40SizeFlag();
                    len += (uint64_t) ac_info.getPos40SizeFlag();

                    if ( dataset_type == core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) { //namespace
                        len += 16;      /// ref_sequence_id

                        /// ref_start_position[seq][ci]][au_id]
                        /// ref_end_position[seq][ci]][au_id]
                        len += (uint64_t) ac_info.getPos40SizeFlag();
                        len += (uint64_t) ac_info.getPos40SizeFlag();
                    }

                    if (datasetHeader->getMultipleAlignmentFlag() == true) {
                        /// extended_AU_start_position[seq][ci]][au_id]
                        /// extended_AU_end_position[seq][ci]][au_id]
                        len += (uint64_t) ac_info.getPos40SizeFlag();
                        len += (uint64_t) ac_info.getPos40SizeFlag();
                    }

                    if (!datasetHeader->block_header.getBlockHeaderFlag()) {
                        for (auto& di = 0; di < datasetHeader->block_header.getClassInfos().getDescriptorIDs().size()[seq][ci]; di++) {
                            len += (uint64_t) ac_info.getByteOffsetSizeFlag();
                        }
                    }
                }
            }
        }
    }

    for (auto& uau_id=0; uau_id < dataset_header->getNumUAccessUnits(); uau_id++) {

        ///AU_byte_offset[uau_id]
        len += (uint64_t) u_ac_info.getByteOffsetSizeFlag();

        if (dataset_type == core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
            len += 16;              /// U_ref_sequence_id[uau_id]  u(16)
            len += (uint64_t) u_ac_info.getPos40SizeFlag();         /// U_ref_start_position[uau_id]
            len += (uint64_t) u_ac_info.getPos40SizeFlag();         /// U_ref_end_position[uau_id]
        }
        else {
            if (ac_info.getMultipleSignatureBase() != 0) {  //TODO(Raouf): check getFunctions
                len += (uint64_t) u_ac_info.getUSignatureSize();     /// U_cluster_signature[uau_id][0] u(U_signature_size)
                if (U_cluster_signature[][] != ((1 << U_signature_size) -1 )) {
                    for (auto i=0; i < u_ac_info.getMultipleSignatureBase(); i++) {
                        len += (uint64_t) u_ac_info.getUSignatureSize();     /// U_cluster_signature[uau_id][i]
                    }
                }
                else {
                    len += 16;      /// num_signatures  u(16)
                    for(auto i=0; i < num_signature; i++) {
                        len += (uint64_t) u_ac_info.getUSignatureSize();     /// U_cluster_signature[uau_id][i]
                    }
                }
            }
        }

        // byte aligned
        len += len % 8;

        if (!datasetHeader->block_header.getBlockHeaderFlag()) {
            for (auto di=0; di < block_header.getClassInfos().getDescriptorIDs().size()[block_header.getNumClasses() - 1]; di++) {
                len += (uint64_t) u_ac_info.getByteOffsetSizeFlag();      /// block_byte_offset[uau_id][di]
            }
        }
    }

    len /= 8;   //byte_conversion

    return len;
}

void MasterIndexTable::write(util::BitWriter& bit_writer) const {
    // TODO (Yeremia): Implement this!
    UTILS_DIE("Not yet implemented!");
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie