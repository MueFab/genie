/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_READER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_READER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <htslib/sam.h>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "genie/format/sam/sam_to_mgrec/sam_record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

/**
 * @brief
 */
class SamReader {
 private:
    samFile* sam_file;      //!< @brief
    bam_hdr_t* sam_header;  //!< @brief
    bam1_t* sam_alignment;  //!< @brief
    kstring_t header_info;  //!< @brief

    std::optional<SamRecord> buffered_rec;

 public:
    /**
     * @brief
     * @param fpath
     */
    explicit SamReader(const std::string& fpath);

    /**
     * @brief
     */
    ~SamReader();

    /**
     * @brief
     * @return
     */
    std::vector<std::pair<std::string, size_t>> getRefs();

    /**
     * @brief
     * @return
     */
    bool isReady();

    /**
     * @brief
     * @return
     */
    bool isValid();

    /**
     * @brief
     * @param sr
     * @return
     */
    int readSamQuery(std::vector<SamRecord>& sr);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
