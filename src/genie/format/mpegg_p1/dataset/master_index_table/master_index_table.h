/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_MASTER_INDEX_TABLE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_MASTER_INDEX_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/format/mpegg_p1/dataset/dataset_header.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/access_unit_info.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/u_access_unit_info/u_access_unit_info.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class MasterIndexTable {
 private:
    std::vector<mpegg_p1::MITAccessUnitInfo> ac_info;  //!< @brief AU_byte_offset, AU_start_position, AU_end_position,
    //!< ref_sequence_id, ref_start_position, ref_end_position, extended_AU_start_position, extended_AU_end_position,
    //!< block_byte_offset

    std::vector<mpegg_p1::MITUAccessUnitInfo> u_ac_info;  //!< @brief U_ref_sequence_id, U_ref_start_position,
                                                          //!< U_ref_end_position, U_cluster_signature, num_signatures,

    DatasetHeader* datasetHeader;  //!< @brief

 public:
    /**
     * @brief Default constructor
     */
    MasterIndexTable();
    /**
     * @brief
     * @param _ac_info
     * @param _u_ac_info
     * @param header
     */
    MasterIndexTable(std::vector<mpegg_p1::MITAccessUnitInfo>&& _ac_info,
                     std::vector<mpegg_p1::MITUAccessUnitInfo>&& _u_ac_info, DatasetHeader* header);

    /**
     * @brief
     * @param reader
     * @param length
     * @param _header
     */
    MasterIndexTable(util::BitReader& reader, size_t length, DatasetHeader* _header);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;
    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_MASTER_INDEX_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
