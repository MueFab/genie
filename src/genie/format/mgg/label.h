/**
 * @file
 * @brief Defines the Label class for managing label metadata within MPEG-G files.
 * @details This file provides the implementation of the `Label` class, which represents a collection of dataset-specific
 *          label information within an MPEG-G file. The Label class includes methods for adding, retrieving, and serializing
 *          dataset-related labels, as well as methods for extracting metadata labels for use in external contexts.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_LABEL_H_
#define SRC_GENIE_FORMAT_MGG_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/meta/label.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/label_dataset.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a label metadata element within an MPEG-G file.
 * @details The `Label` class stores information about a specific label, including its identifier and the list of datasets
 *          associated with it. Each dataset is represented using the `LabelDataset` class, which manages regions associated
 *          with the label for a particular dataset. This class provides functionality for serializing and deserializing
 *          label metadata and includes operations for retrieving metadata for external use.
 */
class Label : public GenInfo {
 private:
    std::string label_ID;                     //!< @brief Identifier for the label.
    std::vector<LabelDataset> dataset_infos;  //!< @brief List of datasets associated with this label.

 public:
    /**
     * @brief Extract metadata label information for a given dataset.
     * @param dataset Dataset ID for which to extract label metadata.
     * @return A `core::meta::Label` object containing the extracted metadata.
     */
    core::meta::Label decapsulate(uint16_t dataset);

    /**
     * @brief Retrieve the unique key identifier for the label.
     * @return A reference to the label ID string.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Compare this label with another `GenInfo` object for equality.
     * @param info The other `GenInfo` object to compare against.
     * @return True if both labels are equivalent, false otherwise.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Construct a new Label object with default parameters.
     */
    Label();

    /**
     * @brief Construct a new Label object with a specified label ID.
     * @param _label_ID The label ID to associate with this Label object.
     */
    explicit Label(std::string _label_ID);

    /**
     * @brief Construct a new Label object by reading from a `BitReader`.
     * @param reader The `BitReader` to extract the label data from.
     */
    explicit Label(util::BitReader& reader);

    /**
     * @brief Add a new dataset to the label.
     * @param _ds_info The `LabelDataset` object to add.
     */
    void addDataset(LabelDataset _ds_info);

    /**
     * @brief Retrieve the label ID for this label.
     * @return A reference to the label ID string.
     */
    [[nodiscard]] const std::string& getLabelID() const;

    /**
     * @brief Retrieve the list of datasets associated with this label.
     * @return A reference to the vector of `LabelDataset` objects.
     */
    [[nodiscard]] const std::vector<LabelDataset>& getDatasets() const;

    /**
     * @brief Serialize the label metadata to a `BitWriter`.
     * @param bit_writer The `BitWriter` to serialize the data to.
     */
    void box_write(genie::util::BitWriter& bit_writer) const override;

    /**
     * @brief Print debug information for the label.
     * @param output The output stream to write the debug information to.
     * @param depth Current depth level in the debug hierarchy.
     * @param max_depth Maximum depth to print in the debug output.
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
