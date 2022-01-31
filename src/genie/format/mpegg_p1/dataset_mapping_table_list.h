/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_LIST_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DataSetMappingTableList : public GenInfo {
 private:
    uint8_t dataset_group_ID;                         //!< @brief
    std::vector<uint16_t> dataset_mapping_table_SID;  //!< @brief

 public:
    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param _ds_group_id
     */
    explicit DataSetMappingTableList(uint8_t _ds_group_id);

    /**
     * @brief
     * @param reader
     */
    explicit DataSetMappingTableList(util::BitReader& reader);

    /**
     * @brief
     * @param sid
     */
    void addDatasetMappingTableSID(uint16_t sid);

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<uint16_t>& getDatasetMappingTableSIDs() const;

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MAPPING_TABLE_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
