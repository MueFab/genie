#ifndef GENIE_PART1_MIT_ACCESS_UNIT_INFO_H
#define GENIE_PART1_MIT_ACCESS_UNIT_INFO_H

#include <vector>
#include <cstdint>
#include <memory>

//#include <genie/format/mpegg_p1/dataset/dataset_header.h>
#include "extended_au_info.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/ref_seq_info.h"

#include <genie/util/bitreader.h>
#include <genie/util/exception.h>
#include <genie/format/mpegg_p1/util.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class MITAccessUnitInfo {
   private:
    uint64_t AU_byte_offset;
    uint64_t AU_start_position;
    uint64_t AU_end_position;

    std::unique_ptr<MITReferenceSequenceInfo> ref_seq_info;

    std::unique_ptr<MITExtendedAUInfo> extended_au_info;

    std::vector<uint64_t> block_byte_offset;

    DatasetHeader* datasetHeader;
    /** ------------------------------------------------------------------------------------------------------------
     *  Internal
     *  ------------------------------------------------------------------------------------------------------------ */
    DatasetHeader::ByteOffsetSizeFlag byte_offset_size_flag;
    DatasetHeader::Pos40SizeFlag pos_40_size_flag;

   public:

    MITAccessUnitInfo();

    MITAccessUnitInfo(uint64_t _AU_byte_offset, uint64_t _AU_start_position, uint64_t _AU_end_position,
                      std::vector<uint64_t> _block_byte_offset, DatasetHeader&& _datasetHeader,
                      DatasetHeader::ByteOffsetSizeFlag _byte_offset_size_flag,
                      DatasetHeader::Pos40SizeFlag _pos_40_size_flag);


    MITAccessUnitInfo(util::BitReader& reader, size_t length);

    uint64_t getAUbyteOffset() const;
    const std::vector<uint64_t>& getBlockByteOffset() const;

    uint64_t getBitLength() const;
    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_MIT_ACCESS_UNIT_INFO_H
