/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <genie/util/exception.h>
#include "genie/util/runtime-exception.h"
#include <genie/format/mpegg_p1/util.h>

#include "dataset_header.h"

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
    multiple_alignment_flag(false),
    dataset_type(core::parameter::DataUnit::DatasetType::NON_ALIGNED),
    alphabet_ID(0),
    num_U_access_units(0) {}

DatasetHeader::DatasetHeader(uint16_t datasetID) : dataset_ID(datasetID) {}

DatasetHeader::DatasetHeader(uint8_t group_ID, uint16_t ID, ByteOffsetSizeFlag _byte_offset_size_flag,
                             bool _non_overlapping_AU_range_flag, Pos40SizeFlag _pos_40_bits_flag,
                             bool _multiple_alignment_flag, core::parameter::DataUnit::DatasetType _dataset_type,
                             uint8_t _alphabet_ID, uint32_t _num_U_access_units)
    : dataset_group_ID(group_ID),
      dataset_ID(ID),
      version("XXXX"), // TODO (Yeremia): Version
      byte_offset_size_flag(_byte_offset_size_flag),
      non_overlapping_AU_range_flag(_non_overlapping_AU_range_flag),
      pos_40_bits_flag(_pos_40_bits_flag),
      multiple_alignment_flag(_multiple_alignment_flag),
      dataset_type(_dataset_type),
      alphabet_ID(_alphabet_ID),
      num_U_access_units(_num_U_access_units){

    UTILS_DIE_IF(4 != version.size(), "Version string must consists of 4 characters");
}

DatasetHeader::DatasetHeader(util::BitReader& bit_reader, size_t length)
    : dataset_group_ID(bit_reader.read<uint8_t>()),
      dataset_ID(bit_reader.read<uint16_t>()),
      version(readNullTerminatedStr(bit_reader, "XXXX")),
      byte_offset_size_flag(bit_reader.read<ByteOffsetSizeFlag>(1)),
      non_overlapping_AU_range_flag(bit_reader.read<bool>(1)),
      pos_40_bits_flag(bit_reader.read<Pos40SizeFlag>(1)),
      multiple_alignment_flag(bit_reader.read<bool>(1)),
      dataset_type(bit_reader.read<core::parameter::DataUnit::DatasetType>(4)),
      alphabet_ID(bit_reader.read<uint8_t>()),
      num_U_access_units(bit_reader.read<uint32_t>()) {

    std::string key = readKey(bit_reader);
    UTILS_DIE_IF(key != "dthd", "DatasetHeader is not Found");

    size_t start_pos = bit_reader.getPos();

    /// Class BlockConfig including ClassInfo
    block_header.ReadBlockConfig(bit_reader);

    /// Class UAccessUnitInfo
    if (num_U_access_units > 0) {
        if (u_access_unit_info != nullptr) {
            u_access_unit_info->readUAccessUnitInfo(bit_reader);
        }
    }

    /// Class SequenceConfig
    seq_info.ReadSequenceConfig(bit_reader);

    UTILS_DIE_IF(bit_reader.getPos()-start_pos != length, "Invalid DatasetHeader length!");
}

uint16_t DatasetHeader::getID() const { return dataset_ID; }

void DatasetHeader::setID(uint16_t ID) { dataset_ID = ID;}

uint8_t DatasetHeader::getGroupID() const {return dataset_group_ID;}

void DatasetHeader::setGroupId(uint8_t group_ID) { dataset_group_ID = group_ID;}


DatasetHeader::ByteOffsetSizeFlag DatasetHeader::getByteOffsetSizeFlag() const { return byte_offset_size_flag; }

DatasetHeader::Pos40SizeFlag DatasetHeader::getPos40SizeFlag() const { return pos_40_bits_flag; }

const SequenceConfig& DatasetHeader::getSeqInfo() const { return seq_info; }

const BlockConfig& DatasetHeader::getBlockHeader() const { return block_header; }

uint32_t DatasetHeader::getNumUAccessUnits() const { return num_U_access_units;}

bool DatasetHeader::getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

core::parameter::DataUnit::DatasetType DatasetHeader::getDatasetType() const { return dataset_type; }


uint64_t DatasetHeader::getLength() const {

//length is first calculated in bits then converted in bytes

    /// Key c(4) Length u(64)
    uint64_t bitlen = (4 * sizeof(char) + 8) * 8 ;   // gen_info

    bitlen += (1 + 2 + 4) * 8;  // dataset_group_ID u(8), dataset_ID u(16), version c(4)
    bitlen += 3; // byte_offset_size_flag u(1), non_overlapping_AU_range_flag u(1) , pos_40_bits_flag u(1)

    // data encapsulated in Class: BlockConfig
    /// block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag
    // data encapsulated in Class: ClassInfo under BlockConfig
    /// num_classes, clid[], num_descriptors[], descriptors_ID[][]
    bitlen += block_header.getBitLength();

    // dataset_type u(4)
    bitlen += 4;

    bitlen += 8 + 32;  // alphabet_ID u(8), num_U_access_units u(32)

    if (num_U_access_units > 0) {
        // data encapsulated in Class: UAccessUnitInfo
        /// num_U_clusters, multiple_signature_base, U_signature_size,
        /// U_signature_constant_length, U_signature_length
        if ( u_access_unit_info != nullptr) {
            bitlen += u_access_unit_info->getBitLength();
        }
    }

    // data encapsulated in Class: SequenceConfig
    /// seq_count, reference_ID, seq_ID[], seq_blocks[],
    /// tflag[0], thres[0], thres[]
    bitlen += seq_info.getBitLength();

    bitlen += bitlen % 8;  /// byte_aligned() f(1)

    return bitlen / 8;
}

void DatasetHeader::write(util::BitWriter& bit_writer) const {

    /// Key of KLV format
    bit_writer.write("dthd");

    /// Length of KLV format
    bit_writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // dataset_ID u(16)
    bit_writer.write(dataset_ID, 16);

    // version c(4)
    writeNullTerminatedStr(bit_writer, version);

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

    /// block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag
    block_header.write(bit_writer);

    // seq_count u(16)
    /// reference_ID, seq_ID[seq] and seq_blocks[seq]
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

    /// num_classes, clid[ci], num_descriptors[ci], descriptor_ID[ci][di]
    block_header.writeClassInfos(bit_writer);

    // alphabet_ID u(8)
    bit_writer.write(alphabet_ID, 8);

    // num_U_access_units u(32)
    bit_writer.write(num_U_access_units, 32);

    if (num_U_access_units){
        UTILS_DIE_IF(u_access_unit_info == nullptr, "No u_access_unit_info stored!");

        ///num_U_clusters, multiple_signature_base, U_signature_size, U_signature_constant_length, U_signature_length
        u_access_unit_info->write(bit_writer);
    }

    // tflag[], thres[]
    seq_info.writeThres(bit_writer);

    /// nesting zero bit
    bit_writer.flush();

}

}// namespace mpegg_p1
}  // namespace format
}  // namespace genie