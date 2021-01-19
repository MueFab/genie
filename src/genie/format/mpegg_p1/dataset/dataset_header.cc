/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_header.h"
#include <parameter/data_unit.h>
#include "genie/util/runtime-exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DatasetHeader::DatasetHeader():
    dataset_group_ID(0),
    dataset_ID(0),
    version(),
    byte_offset_size_flag(ByteOffsetSizeFlag::OFF),
    non_overlapping_AU_range_flag(false),
    pos_40_bits_flag(Pos40SizeFlag::OFF),
    dataset_type(core::parameter::DataUnit::DatasetType::NON_ALIGNED),
    alphabet_ID(0),
    num_U_access_units(0) {}

DatasetHeader::DatasetHeader(const uint16_t datasetID) : dataset_ID(datasetID) {}

DatasetHeader::DatasetHeader(uint8_t group_ID, uint16_t ID, ByteOffsetSizeFlag _byte_offset_size_flag,
                             bool _non_overlapping_AU_range_flag, Pos40SizeFlag _pos_40_bits_flag,
                             core::parameter::DataUnit::DatasetType _dataset_type, uint8_t _alphabet_ID,
                             uint32_t _num_U_access_units)
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

//length is first calculated in bits then converted in bytes

    /// Key c(4) Length u(64)
    uint64_t bitlen = (4 * sizeof(char) + 8) * 8 ;   // gen_info

    bitlen += (1 + 2 + 4) * 8;  /// dataset_group_ID u(8), dataset_ID u(16), version c(4)
    bitlen += 3; /// byte_offset_size_flag u(1), non_overlapping_AU_range_flag u(1) , pos_40_bits_flag u(1)

    bitlen += block_header.getBitLength();   /// block_header_flag u(1)

    // TODO (Raouf): Fix getBitLength()

    if (block_header.getBlockHeaderFlag()) {
        bitlen += block_header.getBitLength(); /// MIT_flag u(1)
        bitlen += block_header.getBitLength(); /// CC_mode_flag u(1)
    }
    else {
        bitlen += block_header.getBitLength(); /// ordered_blocks_flag u(1)
    }

    bitlen += 16;   /// seq_count u(16)
    if (seq_info.getSeqCount() > 0) {
        bitlen += 8; /// reference_ID u(8)
        bitlen += (16 + 32) * seq_info.getSeqCount() ; /// seq_ID u(16), seq_blocks u(32)
    }

    bitlen += 4;  /// dataset_type u(4)
    if (block_header.getMITFlag()) {
        bitlen += 4;  /// num_classes u(4)
        for (auto ci = 0; ci < block_header.getNumClasses(); ci++) {
            bitlen += 4 ;   /// clid[ci] u(4)
            if (!block_header.getBlockHeaderFlag()) {
                bitlen += 5 ;  /// num_descriptors[ci] u(5)
//                for (auto di = 0; di < block_header.getClassInfos()[ci].getDescriptorIds().size(); di++) {
//                    bitlen += 7 ;
//                }
                bitlen += block_header.getClassInfos()[ci].getDescriptorIDs().size() * 7; /// descriptors_ID[ci][di] u(7)
            }
        }
    }

    bitlen += 8 + 32;  /// alphabet_ID u(8), num_U_access_units u(32)
    if (num_U_access_units > 0) {//
        u_access_unit_info->getBitLength();  /// num_U_clusters u(32), multiple_signature_base u(31), U_signature_size u(6), U_signature_constant_length u(1), U_signature_length u(8)
    }

    if (seq_info.getSeqCount() > 0) {
        bitlen += 1 + 31;  /// tflag[0] f(1), thres[0] u(31)
        for (int i = 1; i < seq_info.getSeqCount(); i++) {
            bitlen += 1;  /// tflag[i] u(1)
            if (seq_info.getTFlags()[i]) {
                bitlen += 31;  /// thres[i] u(31)
            }
        }
    }

    bitlen += bitlen % 8;  /// byte_aligned() f(1)
    bitlen /= 8;  /// byte conversion

    return bitlen;
}

uint16_t DatasetHeader::getID() const { return dataset_ID; }

void DatasetHeader::setID(uint16_t ID) { dataset_ID = ID;}

uint8_t DatasetHeader::getGroupID() const {return dataset_group_ID;}

void DatasetHeader::setGroupId(uint8_t group_ID) { dataset_group_ID = group_ID;}

uint32_t DatasetHeader::getNumUAccessUnits() const { return num_U_access_units;}

bool DatasetHeader::getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

void DatasetHeader::write(util::BitWriter& bit_writer) const {

    // Key of KVL format
    bit_writer.write("dthd");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // dataset_IDs u(16)
    bit_writer.write(dataset_ID, 16);

    // version c(4)
    bit_writer.write(version);

    // byte_offset_size_flag u(1)
    if (byte_offset_size_flag == ByteOffsetSizeFlag::ON){
        bit_writer.write(1, 1);
    } else {
        bit_writer.write(0, 1);
    }

    // non_overlapping_AU_range_flag u(1)
    bit_writer.write(non_overlapping_AU_range_flag, 1);

    // pos_40_bits_flag u(1)
    if (pos_40_bits_flag == Pos40SizeFlag::ON) {
        bit_writer.write(1, 1);
    } else {
        bit_writer.write(0, 1);
    }

    // block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag
    block_header.write(bit_writer);

    // seq_count u(16)
    // reference_ID, seq_ID[seq] and seq_blocks[seq]
    seq_info.write(bit_writer);

    // dataset_type u(4)
    switch(dataset_type) {
        case core::parameter::DataUnit::DatasetType::NON_ALIGNED:
            bit_writer.write(0, 4);
            break;
        case core::parameter::DataUnit::DatasetType::ALIGNED:
            bit_writer.write(1, 4);
            break;
        case core::parameter::DataUnit::DatasetType::REFERENCE:
            bit_writer.write(2, 4);
            break;
    }

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

}// namespace mpegg_p1
}  // namespace format
}  // namespace genie