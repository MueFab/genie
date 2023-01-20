/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SORTER_H_
#define SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <string>
#include "boost/optional/optional.hpp"
#include "genie/core/record/alignment/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

/**
 * @brief
 */
class SubfileReader {
 private:
    std::ifstream reader;                              //!< @brief
    genie::util::BitReader bitreader;                  //!< @brief
    boost::optional<genie::core::record::Record> rec;  //!< @brief
    std::string path;                                  //!< @brief

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
    const boost::optional<genie::core::record::Record>& getRecord() const;

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

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_SORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
