/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/dataset_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::DatasetHeader():
    group_ID(0),
    ID(0),
    version("2000"),
    multiple_alignment_flag(false),
    byte_offset_size_flag(ByteOffsetSizeFlag::OFF),
    non_overlapping_AU_range_flag(0),
    pos_40_bits_flag(Pos40SizeFlag::OFF),
    block_header_flag(false),
    MIT_flag(false),
    CC_mode_flag(false),
    ordered_blocks_flag(false),
    reference_ID(0),
    seq_IDs(),
    seq_blocks(),
    dataset_type(),
    class_descs(),
    alphabet_ID(0),
    u_access_unit_info(),
    thress() {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::DatasetHeader(genie::util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length) :
    group_ID(reader.read<uint8_t>()),
    ID(reader.read<uint16_t>()),
    version(readFixedLengthChars(reader, 4)),
    multiple_alignment_flag(reader.read<bool>(1)),
    byte_offset_size_flag(reader.read<ByteOffsetSizeFlag>(1)),
    non_overlapping_AU_range_flag(reader.read<bool>(1)),
    pos_40_bits_flag(reader.read<Pos40SizeFlag>(1)),
    block_header_flag(reader.read<bool>(1)),
    MIT_flag(block_header_flag ? reader.read<bool>(1) : false),
    CC_mode_flag(block_header_flag ? reader.read<bool>(1) : false),
    ordered_blocks_flag(!block_header_flag ? reader.read<bool>(1) : false),
    reference_ID(0),
    seq_IDs(),
    seq_blocks(),
    dataset_type(),
    class_descs(),
    alphabet_ID(0),
    u_access_unit_info(),
    thress()
{
    auto seq_count = reader.read<uint16_t>();

    if (seq_count > 0){
        reference_ID = reader.read<uint8_t>();
        for (auto seq=0;seq<seq_count;seq++){
            seq_IDs.push_back(reader.read<uint16_t>());
        }
        for (auto seq=0;seq<seq_count;seq++){
            seq_blocks.push_back(reader.read<uint32_t>());
        }
    }

    dataset_type = reader.read<core::parameter::DataUnit::DatasetType>(4);

    if (MIT_flag){
        auto num_classes = reader.read<uint8_t>(4);

        for (auto ci=0;ci<num_classes;ci++){
            class_descs.emplace_back(reader, block_header_flag);
        }
    }

    parameters_update_flag = reader.read<bool>(1);
    alphabet_ID = reader.read<uint8_t>(7);

    u_access_unit_info = UAccessUnitInfo(reader, fhd);

    if (seq_count > 0){
        bool tflag = reader.read<bool>(1);
        thress.push_back(reader.read<uint32_t>(31));
        for (auto i=1;i<seq_count;i++){
            tflag = reader.read<bool>(1);
            if (tflag){
                thress.push_back(reader.read<uint32_t>(31));
            } else {
                thress.push_back(thress.back());
            }
        }
    }

    /// nesting_zero_big
    reader.flush();

    auto tlen = reader.getPos() - start_pos;
    auto elen = getLength();

    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid DatasetHeader length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetHeader::getGroupID() const { return group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::setGroupId(uint8_t _group_ID) { group_ID = group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetHeader::getID() const { return ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::setID(uint16_t _ID) { ID = _ID; }

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::ByteOffsetSizeFlag DatasetHeader::getByteOffsetSizeFlag() const { return byte_offset_size_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getNonOverlappingAURangeFlag() const{ return non_overlapping_AU_range_flag;}

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::Pos40SizeFlag DatasetHeader::getPos40SizeFlag() const { return pos_40_bits_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getBlockHeaderFlag() const {return block_header_flag;}

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getMITFlag() const {return MIT_flag;}

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getCCModeFlag() const {return CC_mode_flag;}

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getOrderedBlocksFlag() const {return ordered_blocks_flag;}

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

// ---------------------------------------------------------------------------------------------------------------------

core::parameter::DataUnit::DatasetType DatasetHeader::getDatasetType() const { return dataset_type; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetHeader::getLength() const {
    // length is first calculated in bits then converted in bytes

    /// Key c(4) Length u(64)
    uint64_t bitlen = (4 * sizeof(char) + 8) * 8;  // gen_info

    /// group_ID u(8), ID u(16), version c(4)
    bitlen += (1 + 2 + 4) * 8;

    /// multiple_alignment_flag, byte_offset_size_flag u(1), non_overlapping_AU_range_flag u(1),
    /// pos_40_bits_flag u(1), block_header_flag
    bitlen += 5;

    if (block_header_flag){
        /// MIT_flag, CC_mode_flag
        bitlen += 2;
    } else{
        /// ordered_blocks_flag
        bitlen += 1;
    }

    /// seq_count
    bitlen += 16;

    UTILS_DIE_IF(seq_IDs.size() != seq_blocks.size(), "Different number of seq_ID[] and seq_blocks[]");
    uint64_t seq_count = seq_blocks.size();

    if (seq_count > 0){
        /// reference_ID u(8)
        bitlen += 8;

        /// seq_ID[] u(16)
        bitlen += 16*seq_count;

        /// seq_blocks[] u(32)
        bitlen += 32*seq_count;
    }

    /// dataset_type u(4)
    bitlen += 4;

    if (MIT_flag){
        /// num_classes u(4)
        bitlen += 4;

        for (auto &class_desc:class_descs){
            bitlen += class_desc.getBitLength();
        }
    }

    /// parameters_update_flagu(1), alphabet_ID u(7)
    bitlen += 1+7;

    /// num_U_access_units, num_U_clusters, multiple_signature_base, U_signature_size
    /// U_signature_constant_length, U_signature_length
    u_access_unit_info.getBitLength();

    if (seq_count > 0){
        /// tflang[0] u(1), thres[0] u(31)
        bitlen += 32;

        /// tflang[i] u(1), thres[i] u(31)
        for (uint64_t i=1;i<seq_count;i++){
            bool tflag = thress[i] != thress[i-1];
            bitlen += 1;
            if (tflag){
                bitlen += 31;
            } else {
                /// thress[i] == thress[i-1];
            }
        }
    }

    bitlen += bitlen % 8;  /// byte_aligned() f(1)

    return bitlen / 8;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::write(util::BitWriter& writer) const {
    /// Key of KLV format
    writer.write("dthd");

    /// Length of KLV format
    writer.write(getLength(), 64);

    /// version c(4)
    writer.write(version);

    /// multiple_alignment_flag, byte_offset_size_flag u(1), non_overlapping_AU_range_flag u(1),
    /// pos_40_bits_flag u(1), block_header_flag
    writer.write(multiple_alignment_flag, 1);
    writer.write(byte_offset_size_flag == ByteOffsetSizeFlag::ON, 1);
    writer.write(non_overlapping_AU_range_flag, 1);
    writer.write(pos_40_bits_flag == Pos40SizeFlag::ON, 1);
    writer.write(block_header_flag, 1);

    if (block_header_flag){
        /// MIT_flag, CC_mode_flag
        writer.write(MIT_flag, 1);
        writer.write(CC_mode_flag, 1);
    } else{
        /// ordered_blocks_flag
        writer.write(ordered_blocks_flag, 1);
    }

    uint64_t seq_count = seq_blocks.size();

    /// seq_count u(16)
    writer.write(seq_count, 16);

    if (seq_count > 0){
        /// reference_ID u(8)
        writer.write(reference_ID, 8);

        for (auto seq=0;seq<seq_count;seq++){
            /// seq_ID[] u(16)
            writer.write(seq_IDs[seq], 16);
        }

        for (auto seq=0;seq<seq_count;seq++){
            /// seq_blocks[] u(32)
            writer.write(seq_blocks[seq], 32);
        }
    }

    /// dataset_type u(4)
    writer.write((uint8_t)dataset_type, 4);

    if (MIT_flag){
        /// num_classes u(4)
        writer.write(class_descs.size(), 4);

        for (auto &class_desc:class_descs){
            class_desc.write(writer);
        }
    }

    /// alphabet_ID u(8)
    writer.write(alphabet_ID, 8)
;
    /// num_U_access_units, num_U_clusters, multiple_signature_base, U_signature_size
    /// U_signature_constant_length, U_signature_length
    u_access_unit_info.write(writer);

    if (seq_count > 0){
        /// tflang[0] u(1), thres[0] u(31)
        writer.write(0, 1);
        writer.write(thress[0], 31);

        for (auto i=1;i<seq_count;i++){
            bool tflag = thress[i] != thress[i-1];
            if (tflag){
                writer.write(thress[i], 31);
            } else {
                /// thress[i] == thress[i-1];
            }
        }
    }

    /// nesting zero bit
    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
