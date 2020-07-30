/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_DATASET_GROUP_HEADER_H
#define GENIE_PART1_DATASET_GROUP_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "dataset/dataset.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DatasetGroupHeader {
    /** ------------------------------------------------------------------------------------------------------------
     * ISO 23092-1 Section 6.5.1.2 table 9
     * ------------------------------------------------------------------------------------------------------------- */

   private:
    uint8_t dataset_group_ID;
    uint8_t version_number;
    std::vector<uint16_t> dataset_IDs;

   public:

    /**
     *
     * @param datasets
     * @param ver
     */
    explicit DatasetGroupHeader(std::vector<Dataset>& datasets, uint8_t ver);

    /**
     * Set dataset_group_ID
     *
     * @param _dataset_group_ID
     */
    void setDatasetGroupId(uint8_t _dataset_group_ID);

    /**
     *
     * @return
     */
    uint8_t getDatasetGroupId() const;

    /**
     *
     * @param version_number
     */
    void setVersionNumber(uint8_t _version_number);

    /**
     *
     * @return
     */
    uint8_t getVersionNumber() const;

    /**
     *
     * @return
     */
    uint64_t getNumDatasets() const;

    /**
     *
     * @return
     */
    const std::vector<uint16_t>& getDatasetId() const;

    /**
     * Get length of dataset group header in bytes.
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bit_writer
     */
    void writeToFile(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PART1_DATASET_GROUP_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------