/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SORTER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SORTER_H_

// -----------------------------------------------------------------------------

#include <fstream>
#include <optional>
#include <string>

#include "genie/core/record/record.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

/**
 * @brief
 */
class SubfileReader {
  std::ifstream reader_;                     //!< @brief
  util::BitReader bitreader_;                //!< @brief
  std::optional<core::record::Record> rec_;  //!< @brief
  std::string path_;                         //!< @brief

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
  core::record::Record MoveRecord();

  /**
   * @brief
   * @return
   */
  const std::optional<core::record::Record>& GetRecord() const;

  /**
   * @brief
   * @return
   */
  bool good();

  /**
   * @brief
   * @return
   */
  std::string GetPath() const { return path_; }
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
