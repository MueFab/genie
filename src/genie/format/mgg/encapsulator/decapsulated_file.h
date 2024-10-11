/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `DecapsulatedFile` class for handling MPEG-G file decapsulation.
 *
 * The `DecapsulatedFile` class is part of the MPEG-G decapsulation module and is used to read and process
 * encapsulated MPEG-G files. It provides methods to extract dataset groups and their components, making it
 * easier to manipulate or analyze the data stored within the MPEG-G format.
 *
 * This class serves as the entry point for decapsulating an MPEG-G file and managing its decapsulated contents.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_FILE_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <string>
#include <vector>
#include "genie/format/mgg/encapsulator/decapsulated_dataset_group.h"
#include "genie/format/mgg/mgg_file.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

/**
 * @class DecapsulatedFile
 * @brief Class for managing the decapsulation of MPEG-G files.
 *
 * The `DecapsulatedFile` class is used to read MPEG-G files and extract the internal dataset groups into a more
 * accessible format for further processing or manipulation. It decapsulates the file into individual
 * `DecapsulatedDatasetGroup` objects, which can then be used to access the metadata, references, and datasets
 * within the file.
 */
class DecapsulatedFile {
 private:
    std::vector<DecapsulatedDatasetGroup> groups;  //!< @brief List of decapsulated dataset groups.
    std::ifstream reader;                          //!< @brief Input file stream for reading the MPEG-G file.
    genie::format::mgg::MggFile mpegg_file;        //!< @brief MPEG-G file handler for parsing the encapsulated format.

 public:
    /**
     * @brief Constructs a `DecapsulatedFile` from an input MPEG-G file.
     *
     * This constructor reads the provided MPEG-G file and decapsulates its contents into a list of
     * `DecapsulatedDatasetGroup` objects. These objects provide access to the individual dataset groups
     * and their internal structure.
     *
     * @param input_file Path to the input MPEG-G file.
     * @throws `std::runtime_error` if the file cannot be opened or parsed.
     */
    explicit DecapsulatedFile(const std::string& input_file);

    /**
     * @brief Retrieves the list of decapsulated dataset groups.
     *
     * This method returns a reference to the internal list of `DecapsulatedDatasetGroup` objects, which
     * represent the decapsulated contents of the MPEG-G file.
     *
     * @return Reference to the vector of `DecapsulatedDatasetGroup` objects.
     */
    std::vector<DecapsulatedDatasetGroup>& getGroups();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
