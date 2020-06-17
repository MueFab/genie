/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MPEGG_FILE_H
#define GENIE_MPEGG_FILE_H

#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset_group.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {
/**
 *
 */
class MpeggFile {
   public:
    /**
     *
     */
    explicit MpeggFile(std::vector<DatasetGroup>*);

    /**
     *
     */
    explicit MpeggFile(std::vector<DatasetGroup>*, std::vector<std::string>*);

    /**
     *
     */
    const FileHeader& getFileHeader() const { return fileHeader; }

    /*
     *
     */
    const std::vector<DatasetGroup>& getDatasetGroups() const { return datasetGroups; }

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:
    FileHeader fileHeader;                    //!<
    std::vector<DatasetGroup> datasetGroups;  //!<
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
#endif  // GENIE_MPEGG_FILE_H
