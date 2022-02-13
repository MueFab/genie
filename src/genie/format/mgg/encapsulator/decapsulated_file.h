/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_FILE_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <string>
#include <vector>
#include "genie/format/mgg/encapsulator/decapsulated_dataset_group.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace encapsulator {

/**
 * @brief
 */
class DecapsulatedFile {
    std::vector<DecapsulatedDatasetGroup> groups;  //!< @brief
    std::ifstream reader;                          //!< @brief
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

}  // namespace encapsulator
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
