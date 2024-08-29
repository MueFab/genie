/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mgg/data_stream.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class DataSetMappingTable : public GenInfo {
 public:
    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getDatasetID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<DataStream>& getDataStreams() const;

    /**
     * @brief
     * @param d
     */
    void addDataStream(DataStream d);

    /**
     * @brief
     * @param _dataset_id
     */
    explicit DataSetMappingTable(uint16_t _dataset_id);

    /**
     * @brief
     * @param reader
     */
    explicit DataSetMappingTable(util::BitReader& reader);

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

 private:
    uint16_t dataset_id;              //!< @brief
    std::vector<DataStream> streams;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
