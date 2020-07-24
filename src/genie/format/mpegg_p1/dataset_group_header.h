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
   public:
    /**
     *
     * @param datasetId
     * @param x_datasetGroupID
     */
    explicit DatasetGroupHeader(const std::vector<genie::format::mpegg_p1::Dataset>* datasetId,
                                uint8_t x_datasetGroupID);

    /**
     *
     * @return
     */
    uint8_t getDatasetGroupId() const;

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
     *
     * @param bit_writer
     */
    void writeToFile(genie::util::BitWriter& bit_writer) const;

   private:
    uint8_t dataset_group_ID;
    uint8_t version_number;
    std::vector<uint16_t> dataset_IDs;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PART1_DATASET_GROUP_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------