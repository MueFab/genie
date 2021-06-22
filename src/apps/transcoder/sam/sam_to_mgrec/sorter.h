/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SORTER_H_
#define SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

/**
 * @brief
 */
class SubfileReader {
 private:
    uint64_t curr_mgrec_pos;           //!< @brief
    std::ifstream reader;              //!< @brief
    genie::util::BitReader bitreader;  //!< @brief
    genie::core::record::Record rec;   //!< @brief

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
    bool readRecord();

    /**
     * @brief
     * @return
     */
    genie::core::record::Record&& moveRecord();

    /**
     * @brief
     * @return
     */
    const genie::core::record::Record& getRecord() const;

    /**
     * @brief
     * @param bitwriter
     */
    void writeRecord(genie::util::BitWriter& bitwriter);

    /**
     * @brief
     * @return
     */
    uint64_t getPos() const;

    /**
     * @brief
     * @return
     */
    bool good();

    /**
     * @brief
     */
    void close();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
