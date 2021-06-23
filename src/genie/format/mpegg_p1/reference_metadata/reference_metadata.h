/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_REFERENCE_METADATA_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class ReferenceMetadata {
 private:
    uint8_t dataset_group_ID; //!< @brief
    uint8_t reference_ID; //!< @brief

 public:
    /**
     * @brief Default
     */
    ReferenceMetadata();
    /**
     * @brief
     * @param _ds_group_ID
     * @param _ref_ID
     */
    explicit ReferenceMetadata(uint8_t _ds_group_ID, uint8_t _ref_ID);

    /**
     * @brief
     * @param _dataset_group_ID
     */
    void setDatasetGroupId(uint8_t _dataset_group_ID);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
