/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_GROUP_H
#define GENIE_DATASET_GROUP_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "dataset.h"
#include "dataset_group_header.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DG_metadata {
   public:
    /**
     *
     */
    DG_metadata();

   private:
    std::vector<uint8_t> DG_metadata_value;  //!<
};

/**
 *
 */
class DG_protection {
   public:
    /**
     *
     */
    DG_protection();

   private:
    std::vector<uint8_t> DG_protection_value;  //!<
};

/**
 *
 */
class DatasetGroup {
   public:
    /**
     *
     * @param x_datasetGroupID
     */
    explicit DatasetGroup(std::vector<genie::format::mpegg_p1::Dataset>*, const uint8_t x_datasetGroupID);

    /**
     *
     * @return
     */
    const DatasetGroupHeader& getDatasetGroupHeader() const;

    /**
     *
     * @return
     */
    const std::vector<Dataset>& getDatasets() const;

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:
    mpegg_p1::DatasetGroupHeader dataset_group_header;  //!<
    // DG_metadata dg_metadata;      // optional
    // DG_protection dg_protection;  // optional
    /** reference[] and reference_metadata[] and label_list is optional and not yet implemented */
    std::vector<Dataset> datasets;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATASET_GROUP_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------