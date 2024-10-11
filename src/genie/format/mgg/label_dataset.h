/**
 * @file
 * @brief Defines the LabelDataset class for managing label regions within a dataset in MPEG-G files.
 * @details This file provides the implementation of the `LabelDataset` class, which is used to store and manage
 *          label regions associated with a specific dataset. It includes methods for serializing, deserializing,
 *          and extracting metadata from these regions. The LabelDataset class supports operations for manipulating
 *          and retrieving label-related data for a given dataset.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_LABEL_DATASET_H_
#define SRC_GENIE_FORMAT_MGG_LABEL_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/meta/label.h"
#include "genie/format/mgg/label_region.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a dataset of labels within an MPEG-G file.
 * @details The LabelDataset class encapsulates a collection of label regions associated with a specific dataset ID.
 *          Each label region is defined using the `LabelRegion` class. This class provides functionality for
 *          managing label regions, serializing and deserializing label data, and extracting metadata.
 */
class LabelDataset {
 private:
    uint16_t dataset_ID;  //!< @brief Identifier for the dataset associated with this label dataset.
    std::vector<LabelRegion> dataset_regions;  //!< @brief List of label regions within the dataset.

 public:
    /**
     * @brief Extract label regions as core metadata regions.
     * @param dataset Dataset ID to associate with the metadata.
     * @return A vector of `core::meta::Region` objects representing the label regions.
     */
    std::vector<genie::core::meta::Region> decapsulate(uint16_t dataset);

    /**
     * @brief Compare this LabelDataset with another for equality.
     * @param other The other LabelDataset object to compare.
     * @return True if both datasets are equivalent, false otherwise.
     */
    bool operator==(const LabelDataset& other) const;

    /**
     * @brief Construct a new LabelDataset object with a specified dataset ID.
     * @param _ds_ID Dataset ID to associate with this LabelDataset.
     */
    explicit LabelDataset(uint16_t _ds_ID);

    /**
     * @brief Construct a new LabelDataset object from a BitReader.
     * @param reader The BitReader to extract the label dataset data from.
     */
    explicit LabelDataset(util::BitReader& reader);

    /**
     * @brief Construct a new LabelDataset object using metadata labels.
     * @param _dataset_ID Dataset ID to associate with this LabelDataset.
     * @param labels Reference to a metadata Label object containing the label data.
     */
    LabelDataset(uint16_t _dataset_ID, genie::core::meta::Label& labels);

    /**
     * @brief Add a label region to the dataset.
     * @param _ds_region The LabelRegion object to be added.
     */
    void addDatasetRegion(LabelRegion _ds_region);

    /**
     * @brief Retrieve the dataset ID associated with this LabelDataset.
     * @return Dataset ID.
     */
    [[nodiscard]] uint16_t getDatasetID() const;

    /**
     * @brief Retrieve the total bit length required to represent the dataset.
     * @return Bit length of the dataset.
     */
    [[nodiscard]] uint64_t getBitLength() const;

    /**
     * @brief Serialize the LabelDataset to a BitWriter.
     * @param bit_writer The BitWriter to serialize the data to.
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_LABEL_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
