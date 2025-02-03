/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_PROTECTION_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_PROTECTION_H_

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
class DatasetGroupProtection : public GenInfo {
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
    explicit DatasetGroupProtection(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param _dataset_group_id
     * @param _dg_protection_value
     * @param _version
     */
    DatasetGroupProtection(uint8_t _dataset_group_id, std::string _dg_protection_value,
                           genie::core::MPEGMinorVersion _version);

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
    const std::string& getProtection() const;

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
    genie::core::MPEGMinorVersion version;  //!< @brief
    uint8_t dataset_group_id;               //!< @brief
    std::string dg_protection_value;        //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_PROTECTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
