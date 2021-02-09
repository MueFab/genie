#include "access_unit_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MITAccessUnitInfo::MITAccessUnitInfo()
    : AU_byte_offset(0),
      AU_start_position(0),
      AU_end_position(0),
      block_byte_offset(),
      datasetHeader(),
      byte_offset_size_flag(DatasetHeader::ByteOffsetSizeFlag::OFF),
      pos_40_size_flag(DatasetHeader::Pos40SizeFlag::OFF)
{}

MITAccessUnitInfo::MITAccessUnitInfo(uint64_t _AU_byte_offset, uint64_t _AU_start_position, uint64_t _AU_end_position,
                                     std::vector<uint64_t> _block_byte_offset, DatasetHeader&& _datasetHeader,
                                     DatasetHeader::ByteOffsetSizeFlag _byte_offset_size_flag,
                                     DatasetHeader::Pos40SizeFlag _pos_40_size_flag)
    : AU_byte_offset(_AU_byte_offset),
      AU_start_position(_AU_start_position),
      AU_end_position(_AU_end_position),
      block_byte_offset(std::move(_block_byte_offset)),
      datasetHeader(std::move(&_datasetHeader)),
      byte_offset_size_flag(_byte_offset_size_flag),
      pos_40_size_flag(_pos_40_size_flag)
{}

MITAccessUnitInfo::MITAccessUnitInfo(util::BitReader& reader, size_t length) {
/*
    for (auto ci = 0; ci < datasetHeader->getBlockHeader().getNumClasses(); ci++) {
        if (datasetHeader->getBlockHeader().getClassInfos()[ci].getClid() != core::record::ClassType::CLASS_U) {
            for (auto au_id = 0; au_id < datasetHeader->getSeqInfo().getSeqBlocks(); au_id++) {
                /// AU_byte_offset[seq][ci]][au_id] u(byteOffsetSize)
                AU_byte_offset = reader.read<uint64_t>((uint8_t)byte_offset_size_flag);

                /// AU_start_position[seq][ci]][au_id] u(posSize)
                /// AU_end_position[seq][ci]][au_id] u(posSize)
                AU_start_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);
                AU_end_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);

                if (datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) {
                    /// ref_seq_id u(16)
                    ref_sequence_id = reader.read<uint16_t>();

                    /// ref_start_position[seq][ci]][au_id] u(posSize)
                    /// ref_end_position[seq][ci]][au_id] u(posSize)
                    ref_start_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);
                    ref_end_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);
                }

                if (datasetHeader->getMultipleAlignmentFlag()) {
                    /// extended_AU_start_position[seq][ci]][au_id] u(posSize)
                    /// extended_AU_end_position[seq][ci]][au_id] u(posSize)
                    extended_AU_start_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);
                    extended_AU_end_position = reader.read<uint64_t>((uint8_t)pos_40_size_flag);

                    if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
                        for (auto di = 0;
                             di < datasetHeader->getBlockHeader().getClassInfos()[ci].getDescriptorIDs().size(); di++) {
                            /// block_byte_offset u(byteOffsetSize)
                            for (auto& offset : block_byte_offset) {
                                block_byte_offset.emplace_back(
                                    reader.read<uint64_t>((uint8_t)byte_offset_size_flag));
                            }
                        }
                    }
                }
            }
        }
    }
*/

}
//TODO: constructors

uint64_t MITAccessUnitInfo::getAUbyteOffset() const { return AU_byte_offset;}

const std::vector<uint64_t>& MITAccessUnitInfo::getBlockByteOffset() const { return block_byte_offset;}

uint64_t MITAccessUnitInfo::getBitLength() const {

    uint64_t bitlen;

    for (auto uau_id=0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
            for (auto di=0; di < datasetHeader->getBlockHeader().getClassInfos()[datasetHeader->getBlockHeader().getNumClasses() - 1].getDescriptorIDs().size(); di++) {
                bitlen += (uint64_t) datasetHeader->getByteOffsetSizeFlag();      /// block_byte_offset[uau_id][di]
            }
        }
    }

    for (auto seq=0; seq < datasetHeader->getSeqInfo().getSeqCount(); seq++) {
        for (auto ci=0; ci < datasetHeader->getBlockHeader().getNumClasses(); ci++){
            if (datasetHeader->getBlockHeader().getClassInfos()[ci].getClid() != core::record::ClassType::CLASS_U) {
                for (auto au_id=0; au_id < datasetHeader->getSeqInfo().getSeqBlocks()[seq]; au_id++) {

                    /// AU_byte_offset[seq][ci]][au_id] u(byteOffsetSize)
                    bitlen += (uint64_t) byte_offset_size_flag;

                    /// AU_start_position[seq][ci]][au_id] u(posSize)
                    /// AU_end_position[seq][ci]][au_id] u(posSize)
                    bitlen += (uint64_t) pos_40_size_flag;
                    bitlen += (uint64_t) pos_40_size_flag;

                    if ( datasetHeader->getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE) { //namespace
                        bitlen += 16;      /// ref_sequence_id
                        /// ref_start_position[seq][ci]][au_id] u(posSize)
                        /// ref_end_position[seq][ci]][au_id] u(posSize)
                        bitlen += (uint64_t) pos_40_size_flag;
                        bitlen += (uint64_t) pos_40_size_flag;
                    }

                    if (datasetHeader->getMultipleAlignmentFlag()) {
                        /// extended_AU_start_position[seq][ci]][au_id] u(posSize)
                        /// extended_AU_end_position[seq][ci]][au_id] u(posSize)
                        bitlen += (uint64_t) pos_40_size_flag;
                        bitlen += (uint64_t) pos_40_size_flag;
                    }

                    if (!datasetHeader->getBlockHeader().getBlockHeaderFlag()) {
                        for (auto di = 0; di < datasetHeader->getBlockHeader().getClassInfos()[ci].getDescriptorIDs().size(); di++) {
                            /// block_byte_offset u(byteOffsetSize)
                            bitlen += (uint64_t) byte_offset_size_flag;
                        }
                    }
                }
            }
        }
    }

    return bitlen;
}
void MITAccessUnitInfo::write(util::BitWriter& bit_writer) const {

    // AU_byte_offset u(byteOffsetSize)
    bit_writer.write(AU_byte_offset, (uint8_t) byte_offset_size_flag);

    // AU_start_position u(posSize)
    bit_writer.write(AU_start_position, (uint8_t) pos_40_size_flag);

    // AU_end_position u(posSize)
    bit_writer.write(AU_end_position, (uint8_t) pos_40_size_flag);

    // ref_*
    if (ref_seq_info != nullptr){
        ref_seq_info->write(bit_writer);
    }

    // extended_AU_*
    if (extended_au_info != nullptr) {
        extended_au_info->write(bit_writer);
    }

    // block_byte_offset u(byteOffsetSize)
    for (auto offset: block_byte_offset){
        bit_writer.write(offset, (uint8_t) byte_offset_size_flag);
    }

}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie