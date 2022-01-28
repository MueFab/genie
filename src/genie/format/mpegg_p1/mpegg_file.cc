/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#if 0
#include "genie/format/mpegg_p1/mpegg_file.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

MpeggFile::MpeggFile(std::vector<DatasetGroup>* x_datasetGroups) : fileHeader() {
    ds_groups = std::move(*x_datasetGroups);
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggFile::MpeggFile(std::vector<DatasetGroup>* x_datasetGroups, std::vector<std::string>* compatible_brand)
    : fileHeader(compatible_brand) {
    ds_groups = std::move(*x_datasetGroups);
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggFile::MpeggFile(genie::util::BitReader& reader) : fileHeader(reader), ds_groups() {
    while (true) {
        /// Read K,L of KLV
        size_t start_pos = reader.getPos();
        std::string key = readKey(reader);
        auto val_length = reader.read<uint64_t>();

        UTILS_DIE_IF(key != "dgcn", "Dataset group is not Found");
        ds_groups.emplace_back(reader, fileHeader, start_pos, val_length);
        break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const FileHeader& MpeggFile::getFileHeader() const { return fileHeader; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DatasetGroup>& MpeggFile::getDatasetGroups() const { return ds_groups; }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggFile::writeToFile(genie::util::BitWriter& bitWriter) const {
    fileHeader.writeToFile(bitWriter);
    for (auto& ds_group : ds_groups) {
        ds_group.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
#endif