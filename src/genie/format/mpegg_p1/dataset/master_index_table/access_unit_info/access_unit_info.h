#ifndef GENIE_PART1_MIT_ACCESS_UNIT_INFO_H
#define GENIE_PART1_MIT_ACCESS_UNIT_INFO_H

#include <vector>
#include <cstdint>
#include <memory>

#include <genie/format/mpegg_p1/dataset/dataset_header.h>
#include "extended_au_info.h"
#include "genie/format/mpegg_p1/master_index_table/ref_seq_info.h"

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

    /** ------------------------------------------------------------------------------------------------------------
     *  Internal
     *  ------------------------------------------------------------------------------------------------------------ */
    DatasetHeader::ByteOffsetSizeFlag byte_offset_size_flag;
    DatasetHeader::ByteOffsetSizeFlag pos_40_size_flag;

   public:

    MITAccessUnitInfo();

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_MIT_ACCESS_UNIT_INFO_H
