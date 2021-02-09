#ifndef GENIE_PART1_MIT_H
#define GENIE_PART1_MIT_H

#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/access_unit_info.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/u_access_unit_info/u_access_unit_info.h"
#include "genie/format/mpegg_p1/dataset/dataset_header.h"

#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>
#include <genie/util/exception.h>
#include <genie/format/mpegg_p1/util.h>

//#include <genie/src/core/parameter/data_unit.h>

namespace genie {
namespace format {
namespace mpegg_p1 {


class MasterIndexTable {
   private:

    /// AU_byte_offset, AU_start_position, AU_end_position, ref_sequence_id, ref_start_position, ref_end_position, extended_AU_start_position, extended_AU_end_position, block_byte_offset
    std::vector<mpegg_p1::MITAccessUnitInfo> ac_info;

    /// U_ref_sequence_id, U_ref_start_position, U_ref_end_position, U_cluster_signature, num_signatures,
    std::vector<mpegg_p1::MITUAccessUnitInfo> u_ac_info;

    DatasetHeader* datasetHeader;

   public:
    /**
     * Default constructor
     */
    MasterIndexTable();
    /**
     *
     * @param bitReader
     * @param length
     */
    MasterIndexTable(genie::util::BitReader& bitReader, size_t length);

    /**
     *
     * @return
     */
    uint64_t getLength() const;
    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_MIT_H
