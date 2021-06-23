/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/mpegg_file.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

MpeggFile::MpeggFile(std::vector<DatasetGroup>* x_datasetGroups) : fileHeader() {
    datasetGroups = std::move(*x_datasetGroups);
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggFile::MpeggFile(std::vector<DatasetGroup>* x_datasetGroups, std::vector<std::string>* compatible_brand)
    : fileHeader(compatible_brand) {
    datasetGroups = std::move(*x_datasetGroups);
}

// ---------------------------------------------------------------------------------------------------------------------

const FileHeader& MpeggFile::getFileHeader() const { return fileHeader; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DatasetGroup>& MpeggFile::getDatasetGroups() const { return datasetGroups; }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggFile::writeToFile(genie::util::BitWriter& bitWriter) const {
    fileHeader.writeToFile(bitWriter);
    for (auto& datasetGroup : datasetGroups) {
        datasetGroup.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
