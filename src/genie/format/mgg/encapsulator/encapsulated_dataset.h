/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `EncapsulatedDataset` structure for handling the encapsulation of MPEG-G datasets.
 *
 * The `EncapsulatedDataset` class is part of the MPEG-G encapsulation module and is used to manage MPEG-G dataset
 * encapsulation. It provides methods to read and parse datasets from input files, and encapsulates them into the MPEG-G
 * format. This structure is primarily intended to facilitate the creation and handling of MPEG-G formatted dataset
 * components.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_DATASET_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <string>
#include <vector>
#include "genie/core/meta/dataset.h"
#include "genie/format/mgb/mgb_file.h"
#include "genie/format/mgg/dataset.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

/**
 * @struct EncapsulatedDataset
 * @brief Class for managing MPEG-G dataset encapsulation.
 *
 * The `EncapsulatedDataset` structure encapsulates MPEG-G datasets, metadata, and corresponding MGB files. It is used
 * to store the dataset's internal structure and facilitates reading and writing operations for MPEG-G formatted
 * datasets.
 */
struct EncapsulatedDataset {
    std::ifstream reader;                  //!< @brief Input file stream used to read the dataset from an input file.
    genie::format::mgb::MgbFile mgb_file;  //!< @brief The MGB file structure representing the encapsulated dataset.
    genie::core::meta::Dataset
        meta;  //!< @brief Metadata associated with the dataset, such as description, type, and related information.

    std::vector<genie::format::mgg::Dataset>
        datasets;  //!< @brief A list of encapsulated `Dataset` objects representing the dataset components.

    /**
     * @brief Constructs an `EncapsulatedDataset` from an input file.
     *
     * This constructor reads the specified input file and initializes the internal MGB and dataset structures.
     *
     * @param input_file Path to the input dataset file.
     * @param version MPEG-G minor version used for the encapsulated file.
     * @throws `std::runtime_error` if the file cannot be opened or parsed.
     */
    explicit EncapsulatedDataset(const std::string& input_file, genie::core::MPEGMinorVersion version);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
