/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/master_index_table/u_access_unit_info/u_access_unit_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

MITUAccessUnitInfo::MITUAccessUnitInfo()
    : multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(false),
      U_signature_length(0),
      U_cluster_signature(),
      num_signatures(0),
      datasetHeader(),
      byte_offset_size_flag(DatasetHeader::ByteOffsetSizeFlag::OFF),
      pos_40_size_flag(DatasetHeader::Pos40SizeFlag::OFF) {}

// ---------------------------------------------------------------------------------------------------------------------

MITUAccessUnitInfo::MITUAccessUnitInfo(uint32_t)
    : multiple_signature_base(0),
      U_signature_size(0),
      U_signature_constant_length(false),
      U_signature_length(0),
      U_cluster_signature(),
      num_signatures(0),
      datasetHeader(),
      byte_offset_size_flag(DatasetHeader::ByteOffsetSizeFlag::OFF),
      pos_40_size_flag(DatasetHeader::Pos40SizeFlag::OFF) {}

// ---------------------------------------------------------------------------------------------------------------------

MITUAccessUnitInfo::MITUAccessUnitInfo(util::BitReader& reader) {
    /// AU_byte_offset[uau_id] u(byteOffsetSize)
    reader.read<uint64_t>((uint8_t)byte_offset_size_flag);

    if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
        /// U_ref_sequence_id[uau_id]  u(16)
        /// U_ref_start_position[uau_id] u(posSize)
        /// U_ref_end_position[uau_id] u(posSize)
        U_ref_sequence_id = reader.read<uint16_t>();
        U_ref_start_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);
        U_ref_end_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);

    } else {
        if (multiple_signature_base != 0) {
            /// U_cluster_signature[uau_id][0] u(U_signature_size)
            U_cluster_signature[0] = reader.read<uint64_t>((uint8_t)U_signature_size);

            if (U_cluster_signature[0] != ((1 << U_signature_size) - 1)) {
                for (uint32_t i = 0; i < multiple_signature_base; i++) {
                    /// U_cluster_signature[uau_id][i] u(U_signature_size)
                    U_cluster_signature[i] = reader.read<uint64_t>((uint8_t)U_signature_size);
                }
            } else {
                /// num_signatures  u(16)
                num_signatures = reader.read<uint16_t>();
                for (auto i = 0; i < num_signatures; i++) {
                    /// U_cluster_signature[uau_id][i] u(U_signature_size)
                    U_cluster_signature[i] = reader.read<uint64_t>((uint8_t)U_signature_size);
                }
            }
        }
    }

    // byte aligned
    reader.flush();

    std::vector<uint64_t> block_byte_offset;
    if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
        for (auto di = 0; di < datasetHeader->getBlockHeader()
                                   .getClassInfos()[datasetHeader->getBlockHeader().getNumClasses() - 1]
                                   .getNumDescriptors();
             di++) {
            /// block_byte_offset[uau_id][di] u(byteOffsetSize)
            block_byte_offset.emplace_back(reader.read<uint64_t>((uint8_t)byte_offset_size_flag));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MITUAccessUnitInfo::setMultipleSignature(uint32_t base, uint8_t size) {
    multiple_signature_base = base;
    U_signature_size = size;
}

// ---------------------------------------------------------------------------------------------------------------------

void MITUAccessUnitInfo::setConstantSignature(uint8_t sign_length) {
    U_signature_constant_length = true;
    U_signature_length = sign_length;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t MITUAccessUnitInfo::getUSignatureSize() const { return U_signature_size; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t MITUAccessUnitInfo::getMultipleSignatureBase() const { return multiple_signature_base; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
