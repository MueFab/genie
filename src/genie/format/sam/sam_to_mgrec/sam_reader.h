/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_READER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_READER_H_

// -----------------------------------------------------------------------------

#include <htslib/sam.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "genie/format/sam/sam_record.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

/**
 * @brief
 */
class SamReader {
  samFile* sam_file_;      //!< @brief
  bam_hdr_t* sam_header;   //!< @brief
  bam1_t* sam_alignment_;  //!< @brief
  kstring_t header_info;   //!< @brief

  std::optional<SamRecord> buffered_rec_;

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
  [[nodiscard]] std::vector<std::pair<std::string, size_t>> GetRefs() const;

  /**
   * @brief
   * @return
   */
  bool IsReady();

  /**
   * @brief
   * @return
   */
  bool IsValid();

  /**
   * @brief
   * @param sr
   * @return
   */
  int ReadSamQuery(std::vector<SamRecord>& sr);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_READER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
