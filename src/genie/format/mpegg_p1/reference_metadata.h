/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class ReferenceMetadata : public GenInfo {
 private:
    uint8_t dataset_group_id;              //!< @brief
    uint8_t reference_id;                  //!< @brief
    std::string reference_metadata_value;  //!< @brief

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
     * @return
     */
    uint64_t getSize() const override;

    /**
     * @brief
     * @param _dataset_group_id
     * @param _reference_id
     * @param _reference_metadata_value
     */
    ReferenceMetadata(uint8_t _dataset_group_id, uint8_t _reference_id, std::string _reference_metadata_value);

    /**
     * @brief
     * @param bitreader
     */
    explicit ReferenceMetadata(genie::util::BitReader& bitreader);

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
    uint8_t getReferenceID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getReferenceMetadataValue() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
