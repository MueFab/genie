/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/runtime-exception.h"
#include "dataset_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// TODO (Yeremia): Fix DatasetHeader constructor
DatasetHeader::DatasetHeader(const uint16_t datasetID) : dataset_ID(datasetID) {}

DatasetHeader::DatasetHeader(uint8_t group_ID, uint16_t ID, bool _byte_offset_size_flag,
                             bool _non_overlapping_AU_range_flag, bool _pos_40_bits_flag,
                             uint8_t _dataset_type, uint8_t _alphabet_ID, uint32_t _num_U_access_units)
    : dataset_group_ID(group_ID),
      dataset_ID(ID),
      version("XXXX"), // TODO (Yeremia): Version
      byte_offset_size_flag(_byte_offset_size_flag),
      non_overlapping_AU_range_flag(_non_overlapping_AU_range_flag),
      pos_40_bits_flag(_pos_40_bits_flag),
      dataset_type(_dataset_type),
      alphabet_ID(_alphabet_ID),
      num_U_access_units(_num_U_access_units){

    UTILS_DIE_IF(4 != version.size(), "Version string must consists of 4 characters");
}

uint64_t DatasetHeader::getLength() const {
    uint64_t bitlength = 12 * 8;   // gen_info
    bitlength += (1 + 2 + 4) * 8;  // dataset_group_ID, dataset_ID, version
    bitlength += 4; // byte_offset_size_flag, non_overlapping_AU_range_flag, pos_40_bits_flag

    bitlength += block_header.getLength();

    // TODO (Yeremia): Fix getLength()


//    length += 16;
//    if (seq_count > 0) {
//        length += 8;                      // reference_ID
//        length += (16 + 32) * seq_count;  // seq_ID + seq_blocks
//    }
//    length += 4;  // dataset_type
//    if (MIT_flag == 1) {
//        length += 4;                // num_classes
//        length += 4 * num_classes;  // clid
//        if (!block_header_flag) {
//            length += 5 * num_classes;                           // num_descriptors
//            length += 7 * num_classes * num_descriptors.size();  // descriptor_ID
//        }
//    }
//    length += 8 + 32;  // alphabet_ID + num_U_access_units
//    if (num_U_access_units > 0) {
//        length += 32 + 31;  // num_U_clusters + multiple_signature_base
//        if (multiple_signature_base > 0) {
//            length += 6;  // U_signature_size
//        }
//        length += 1;  // U_signature_constant_length
//        if (U_signature_constant_length) {
//            length += 8;  // U_signature_length
//        }
//    }
//    if (seq_count > 0) {
//        length += 1 + 31;  // tflag[0] + thres[0]
//        for (int i = 1; i < seq_count; i++) {
//            length += 1;  // tflag[i]
//            if (tflag[i] == 1) {
//                length += 31;  // thres[i]
//            }
//        }
//    }
//
//    while (length % 8)  // byte_aligned
//    {
//        length++;
//    }
//
//    length /= 8;  // byte conversion
//    return length;
}

uint16_t DatasetHeader::getDatasetId() const {return dataset_ID;}

uint8_t DatasetHeader::getDatasetGroupId() const {return dataset_group_ID;}

void DatasetHeader::setDatasetGroupId(uint8_t group_ID) { dataset_group_ID = group_ID;}

void DatasetHeader::write(util::BitWriter& bit_writer) const {
    UTILS_DIE_IF(version.size() != 4, "Invalid version");

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // dataset_IDs u(16)
    bit_writer.write(dataset_ID, 16);

    // version c(4)
    bit_writer.write(version);

    // byte_offset_size_flag u(1)
    bit_writer.write(byte_offset_size_flag, 1);

    // non_overlapping_AU_range_flag u(1)
    bit_writer.write(non_overlapping_AU_range_flag, 1);

    // pos_40_bits_flag u(1)
    bit_writer.write(pos_40_bits_flag, 1);

    // block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag
    block_header.write(bit_writer);

    // seq_count u(16)
    // reference_ID, seq_ID[seq] and seq_blocks[seq]
    seq_info.write(bit_writer);

    // dataset_type u(4)
    bit_writer.write(dataset_type, 4);

    // num_classes, clid[ci], num_descriptors[ci], descriptor_ID[ci][di]
    block_header.writeClassInfos(bit_writer);

    // alphabet_ID u(8)
    bit_writer.write(alphabet_ID, 8);

    // num_U_access_units u(32)
    bit_writer.write(num_U_access_units, 32);

    if (num_U_access_units){
        UTILS_DIE_IF(u_access_unit_info == nullptr, "No u_access_unit_info stored!");

        //num_U_clusters, multiple_signature_base, U_signature_size, U_signature_constant_length, U_signature_length
        u_access_unit_info->write(bit_writer);
    }

    // tflag[], thres[]
    seq_info.writeThres(bit_writer);

    // nesting zero bit
    bit_writer.flush();

}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie