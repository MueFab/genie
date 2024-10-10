/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_FILE_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>
#include "genie/format/mgg/encapsulator/encapsulated_dataset_group.h"
#include "genie/format/mgg/mgg_file.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

/**
 * @brief
 */
struct EncapsulatedFile {
    std::vector<EncapsulatedDatasetGroup> groups;  //!< @brief

    /**
     * @brief
     * @param input_files
     * @return
     */
    [[nodiscard]] static std::map<uint8_t, std::vector<std::string>> groupInputFiles(
        const std::vector<std::string>& input_files);

    /**
     * @brief
     * @param input_files
     * @param version
     */
    EncapsulatedFile(const std::vector<std::string>& input_files, genie::core::MPEGMinorVersion version);

    /**
     * @brief
     * @param version
     * @return
     */
    genie::format::mgg::MggFile assemble(genie::core::MPEGMinorVersion version);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_ENCAPSULATED_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
