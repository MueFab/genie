/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_GROUP_HEADER_H
#define GENIE_DATASET_GROUP_HEADER_H

#include <cstdint>
#include <vector>
#include "dataset.h"

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
                                const uint8_t x_datasetGroupID);

    /**
     *
     * @return
     */
    uint8_t getDatasetGroupId() const { return dataset_group_ID; }

    /**
     *
     * @return
     */
    uint8_t getVersionNumber() const { return version_number; }

    /**
     *
     * @return
     */
    uint64_t getNumDatasets() const { return dataset_ID.size(); };

    /**
     *
     * @return
     */
    const std::vector<uint16_t>& getDatasetId() const { return dataset_ID; }

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:
    /**
     * ISO 23092-1 Section 6.5.1.2 table 19
     *
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID : 8;      //!<
    uint8_t version_number : 8;        //!<
    std::vector<uint16_t> dataset_ID;  //!<
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
#endif  // GENIE_DATASET_GROUP_HEADER_H
