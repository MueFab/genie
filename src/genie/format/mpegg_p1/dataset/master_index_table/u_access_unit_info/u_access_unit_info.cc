    #include "u_access_unit_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MITUAccessUnitInfo::MITUAccessUnitInfo()
    : num_U_clusters(0),
      multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(false),
      U_signature_length(0),
      datasetHeader(),
      ac_info(),
      U_cluster_signature(),
      num_signatures(0)
{}

MITUAccessUnitInfo::MITUAccessUnitInfo(uint32_t _num_U_clusters)
    : num_U_clusters(_num_U_clusters),
      multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(false),
      U_signature_length(0),
      datasetHeader(),
      ac_info(),
      U_cluster_signature(),
      num_signatures(0)
{}


MITUAccessUnitInfo::MITUAccessUnitInfo(util::BitReader& reader, size_t length) {
    /*
     for (auto uau_id=0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        ///AU_byte_offset[uau_id] u(byteOffsetSize)
        AU_byte_offset = reader.read_b((uint8_t) byte_offset_size_flag);

        if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
            U_ref_sequence_id = reader.read<uint16_t>();              /// U_ref_sequence_id[uau_id]  u(16)
            U_ref_start_position = reader.read_b((uint8_t) pos_40_size_flag);    /// U_ref_start_position[uau_id] u(posSize)
            U_ref_end_position = reader.read_b((uint8_t) pos_40_size_flag);        /// U_ref_end_position[uau_id] u(posSize)datasetHeader->getDatasetType()
        }
        else {
            if (multiple_signature_base != 0) {
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
}

void MITUAccessUnitInfo::setMultipleSignature(uint32_t base, uint8_t size) {
    multiple_signature_base = base;
    U_signature_size = size;
}
void MITUAccessUnitInfo::setConstantSignature(uint8_t sign_length) {
    U_signature_constant_length = true;
    U_signature_length = sign_length;
}

uint8_t MITUAccessUnitInfo::getUSignatureSize() const { return U_signature_size;}

uint32_t MITUAccessUnitInfo::getMultipleSignatureBase() const { return multiple_signature_base; }

uint64_t MITUAccessUnitInfo::getBitLength() const {

    uint64_t bitlen;

    for (auto uau_id=0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        ///AU_byte_offset[uau_id] u(byteOffsetSize)
        bitlen += (uint64_t) byte_offset_size_flag;

        if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
            bitlen += 16;                                  /// U_ref_sequence_id[uau_id]  u(16)
            bitlen += (uint64_t) pos_40_size_flag;         /// U_ref_start_position[uau_id] u(posSize)
            bitlen += (uint64_t) pos_40_size_flag;         /// U_ref_end_position[uau_id] u(posSize)
        }
        else {
            if (multiple_signature_base != 0) {
                bitlen += (uint64_t) U_signature_size;             /// U_cluster_signature[uau_id][0] u(U_signature_size)
                if (U_cluster_signature[uau_id] != ((1 << U_signature_size) -1 )) {
                    for (auto i=0; i < multiple_signature_base; i++) {
                        bitlen += (uint64_t) U_signature_size;     /// U_cluster_signature[uau_id][i]
                    }
                }
                else {
                    bitlen += 16;                                  /// num_signatures  u(16)
                    for(auto i=0; i < num_signatures; i++) {
                        bitlen += (uint64_t) U_signature_size;     /// U_cluster_signature[uau_id][i]
                    }
                }
            }
        }

        // byte aligned
        bitlen += bitlen % 8;

    }

    return bitlen;
}

void MITUAccessUnitInfo::write(util::BitWriter& bit_writer) const {

    for (auto uau_id=0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        /// AU_byte_offset[uau_id] u(byteOffsetSize)
        bit_writer.write(ac_info.getAUbyteOffset(), (uint8_t) datasetHeader->getByteOffsetSizeFlag());

        if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
            /// U_ref_sequence_id[uau_id]  u(16)
            bit_writer.write(U_ref_sequence_id, 16);

            /// U_ref_start_position[uau_id] u(posSize), U_ref_end_position[uau_id] u(posSize)
            bit_writer.write(U_ref_start_position, (uint8_t) datasetHeader->getPos40SizeFlag());
            bit_writer.write(U_ref_end_position, (uint8_t) datasetHeader->getPos40SizeFlag());
        }
        else {
            if (multiple_signature_base != 0) {
                /// U_cluster_signature[uau_id][0] u(U_signature_size)
                for (auto& signature : U_cluster_signature){
                    bit_writer.write(signature, (uint8_t) U_signature_size);
                }

                if (U_cluster_signature[uau_id] != ((1 << U_signature_size) -1 )) {
                    for (auto i=0; i < multiple_signature_base; i++) {
                        /// U_cluster_signature[uau_id][i]
                        for (auto& signature : U_cluster_signature){
                            bit_writer.write(signature, (uint8_t) U_signature_size);
                        }
                    }
                }
                else {
                    /// num_signatures  u(16)
                    bit_writer.write(num_signatures, 16);

                    for (auto i=0; i < num_signatures; i++) {
                        /// U_cluster_signature[uau_id][i]
                        for (auto& signature : U_cluster_signature) {
                            bit_writer.write(signature, (uint8_t)U_signature_size);  // TODO(RAOUF):check
                        }
                    }
                }
            }
        }

        // byte aligned

        if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
            for (auto di=0; di < datasetHeader->getBlockHeader().getClassInfos()[datasetHeader->getBlockHeader().getNumClasses() - 1].getDescriptorIDs().size(); di++) {
                /// block_byte_offset[uau_id][di]
                for (auto& offset : ac_info.getBlockByteOffset()) {
                    bit_writer.write(offset, (uint8_t) datasetHeader->getByteOffsetSizeFlag());
                }
            }
        }
    }

}


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
