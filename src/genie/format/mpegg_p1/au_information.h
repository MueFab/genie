/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_AU_INFORMATION_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_AU_INFORMATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/constants.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AUInformation : public GenInfo {
 public:
    const std::string& getKey() const override;

    /**
     * @brief
     * @param bitreader
     * @param _version
     */
    explicit AUInformation(genie::util::BitReader& bitreader,
                           genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

    /**
     * @brief
     * @param _dataset_group_id
     * @param _dataset_id
     * @param _au_information_value
     * @param _version
     */
    AUInformation(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _au_information_value,
                  genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

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
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getInformation() const;

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

 private:
    genie::core::MPEGMinorVersion version;  //!< @brief
    uint8_t dataset_group_id;               //!< @brief
    uint16_t dataset_id;                    //!< @brief
    std::string au_information_value;       //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_AU_INFORMATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
