#ifndef GENIE_PART1_MIT_EXTENDED_AU_INFO_H
#define GENIE_PART1_MIT_EXTENDED_AU_INFO_H

#include <cstdint>

#include "genie/util/bitwriter.h"
#include <genie/format/mpegg_p1/dataset/dataset_header.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class MITExtendedAUInfo {
   private:
    uint64_t extend_AU_start_position;
    uint64_t extend_AU_end_position;

    /** ------------------------------------------------------------------------------------------------------------
     *  Internal
     *  ------------------------------------------------------------------------------------------------------------ */
    DatasetHeader::Pos40SizeFlag pos_40_flag;
   public:

    MITExtendedAUInfo(uint64_t start_pos, uint64_t end_pos, DatasetHeader::Pos40SizeFlag flag);

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie


#endif  // GENIE_PART1_MIT_EXTENDED_AU_INFO_H
