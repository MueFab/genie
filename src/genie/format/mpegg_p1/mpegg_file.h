/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_MPEGG_FILE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_MPEGG_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset_group.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class MpeggFile {
 public:
    /**
     * @brief
     */
    explicit MpeggFile(std::vector<DatasetGroup>*);

    /**
     * @brief
     */
    explicit MpeggFile(std::vector<DatasetGroup>*, std::vector<std::string>*);

    /**
     * @brief
     */
    const FileHeader& getFileHeader() const;

    /**
     * @brief
     */
    const std::vector<DatasetGroup>& getDatasetGroups() const;

    /**
     * @brief
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

 private:
    FileHeader fileHeader;                    //!< @brief
    std::vector<DatasetGroup> datasetGroups;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_MPEGG_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
