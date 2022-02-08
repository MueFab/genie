/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_DATASET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/meta/label.h"
#include "genie/format/mgg/label_region.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class LabelDataset {
 private:
    uint16_t dataset_ID;  //!< @brief

    std::vector<LabelRegion> dataset_regions;  //!< @brief

 public:
    /**
     * @brief
     * @param dataset
     * @return
     */
    std::vector<genie::core::meta::Region> decapsulate(uint16_t dataset);

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const LabelDataset& other) const;

    /**
     * @brief
     * @param _ds_ID
     */
    explicit LabelDataset(uint16_t _ds_ID);
    /**
     * @brief
     * @param reader
     */
    explicit LabelDataset(util::BitReader& reader);

    /**
     * @brief
     * @param _dataset_ID
     * @param labels
     */
    LabelDataset(uint16_t _dataset_ID, genie::core::meta::Label& labels);

    /**
     * @brief
     * @param _ds_region
     */
    void addDatasetRegion(LabelRegion _ds_region);

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @return
     */
    uint64_t getBitLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
