/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_METADATA_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class DatasetGroupMetadata : public GenInfo {
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
     * @param bitreader
     * @param _version
     */
    explicit DatasetGroupMetadata(genie::util::BitReader& bitreader, genie::core::MpegMinorVersion _version);

    /**
     * @brief
     * @param _dataset_group_id
     * @param _dg_metatdata_value
     * @param _version
     */
    DatasetGroupMetadata(uint8_t _dataset_group_id, std::string _dg_metatdata_value,
                         genie::core::MpegMinorVersion _version);

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getMetadata() const;

    /**
     * @brief
     * @return
     */
    std::string decapsulate();

    /**
     * @brief
     * @param groupID
     */
    void patchID(uint8_t groupID);

 private:
    genie::core::MpegMinorVersion version;  //!< @brief
    uint8_t dataset_group_id;               //!< @brief
    std::string dg_metatdata_value;         //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
