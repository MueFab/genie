/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/access_unit_info.h"
#include <utility>
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

MITAccessUnitInfo::MITAccessUnitInfo()
    : AU_byte_offset(0),
      AU_start_position(0),
      AU_end_position(0),
      block_byte_offset(),
      datasetHeader(),
      byte_offset_size_flag(DatasetHeader::ByteOffsetSizeFlag::OFF),
      pos_40_size_flag(DatasetHeader::Pos40SizeFlag::OFF) {}

// ---------------------------------------------------------------------------------------------------------------------

MITAccessUnitInfo::MITAccessUnitInfo(uint64_t _AU_byte_offset, uint64_t _AU_start_position, uint64_t _AU_end_position,
                                     std::vector<uint64_t>&& _block_byte_offset, DatasetHeader* _datasetHeader,
                                     DatasetHeader::ByteOffsetSizeFlag _byte_offset_size_flag,
                                     DatasetHeader::Pos40SizeFlag _pos_40_size_flag)
    : AU_byte_offset(_AU_byte_offset),
      AU_start_position(_AU_start_position),
      AU_end_position(_AU_end_position),
      block_byte_offset(std::move(_block_byte_offset)),
      datasetHeader(std::move(_datasetHeader)),
      byte_offset_size_flag(_byte_offset_size_flag),
      pos_40_size_flag(_pos_40_size_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

MITAccessUnitInfo::MITAccessUnitInfo(util::BitReader& reader) {
    /// AU_byte_offset[seq][ci]][au_id] u(byteOffsetSize)
    AU_byte_offset = reader.read<uint64_t>((uint8_t)byte_offset_size_flag);

    /// AU_start_position[seq][ci]][au_id] u(posSize)
    /// AU_end_position[seq][ci]][au_id] u(posSize)
    AU_start_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);
    AU_end_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);

    if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
        // MITReferenceSequenceInfo
        ref_seq_info = util::make_unique<MITReferenceSequenceInfo>(reader);
    }

    if (datasetHeader->getMultipleAlignmentFlag()) {
        // MITExtendedAUInfo
        extended_au_info = util::make_unique<MITExtendedAUInfo>(reader);
    }

    if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
        for (auto di = 0; di < datasetHeader->getBlockHeader().getClassInfos()[ci].getNumDescriptors(); di++) {
            /// block_byte_offset u(byteOffsetSize)
            block_byte_offset.emplace_back(reader.read<uint64_t>((uint8_t)byte_offset_size_flag));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MITAccessUnitInfo::getAUbyteOffset() const { return AU_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& MITAccessUnitInfo::getBlockByteOffset() const { return block_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

void MITAccessUnitInfo::write(util::BitWriter& bit_writer) const {
    // AU_byte_offset u(byteOffsetSize)
    bit_writer.write(AU_byte_offset, (uint8_t)byte_offset_size_flag);

    // AU_start_position u(posSize)
    bit_writer.write(AU_start_position, (uint8_t)pos_40_size_flag);

    // AU_end_position u(posSize)
    bit_writer.write(AU_end_position, (uint8_t)pos_40_size_flag);

    // ref_*
    if (ref_seq_info != nullptr) {
        ref_seq_info->write(bit_writer);
    }

    // extended_AU_*
    if (extended_au_info != nullptr) {
        extended_au_info->write(bit_writer);
    }

    // block_byte_offset u(byteOffsetSize)
    for (auto offset : block_byte_offset) {
        bit_writer.write(offset, (uint8_t)byte_offset_size_flag);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
