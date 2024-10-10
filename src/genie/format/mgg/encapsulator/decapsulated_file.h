/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
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
 * @brief
 */
class DecapsulatedFile {
    std::vector<DecapsulatedDatasetGroup> groups;  //!< @brief
    std::ifstream reader;                          //!< @brief
    genie::format::mgg::MggFile mpegg_file;        //!< @brief
 public:
    /**
     * @brief
     * @param input_file
     */
    explicit DecapsulatedFile(const std::string& input_file);

    /**
     * @brief
     * @return
     */
    std::vector<DecapsulatedDatasetGroup>& getGroups();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
