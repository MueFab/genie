/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_DATASET_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_DATASET_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "label_region.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class LabelDataset {
 private:
    /**
     * ISO 23092-1 Section 6.5.1.5.4 table 15 (for num_dataset)
     *
     **/
    uint16_t dataset_ID;

    std::vector<LabelRegion> dataset_regions;

 public:
    /**
     *
     * @param _ds_ID
     */
    explicit LabelDataset(uint16_t _ds_ID);
    /**
     *
     * @param reader
     */
    explicit LabelDataset(util::BitReader& reader);

    /**
     *
     * @param _ds_region
     */
    void addDatasetRegion(LabelRegion _ds_region);

    /**
     *
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     *
     * @return
     */
    uint64_t getBitLength() const;
    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_DATASET_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
