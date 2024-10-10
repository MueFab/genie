/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SORTER_H_
#define SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <optional>
#include <string>
#include "genie/core/record/record.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam::sam::sam_to_mgrec {

/**
 * @brief
 */
class SubfileReader {
 private:
    std::ifstream reader;                            //!< @brief
    genie::util::BitReader bitreader;                //!< @brief
    std::optional<genie::core::record::Record> rec;  //!< @brief
    std::string path;                                //!< @brief

 public:
    /**
     * @brief
     * @param fpath
     */
    explicit SubfileReader(const std::string& fpath);

    /**
     * @brief
     */
    ~SubfileReader();

    /**
     * @brief
     * @return
     */
    genie::core::record::Record moveRecord();

    /**
     * @brief
     * @return
     */
    const std::optional<genie::core::record::Record>& getRecord() const;

    /**
     * @brief
     * @return
     */
    bool good();

    /**
     * @brief
     * @return
     */
    std::string getPath() const { return path; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_sam::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
