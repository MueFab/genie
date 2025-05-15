/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_EXPORTER_H_
#define SRC_GENIE_FORMAT_SAM_EXPORTER_H_

// -----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <string>
#include <htslib/sam.h>
#include "genie/core/format_exporter.h"
#include "genie/core/record/chunk.h"
#include "genie/util/drain.h"
#include "genie/util/ordered_lock.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

/**
 * @brief Module to export MPEG-G record to sam files
 */
class Exporter final : public core::FormatExporter {
  util::OrderedLock lock_;  //!< @brief Lock to ensure in order execution
  std::string fasta_file_path_;
  std::string output_file_path_;
  std::optional<std::ofstream> output_stream_;
  std::ostream* output_file_ = &std::cout;
  bool output_set_ = false;
  std::string output_format;
  sam_hdr_t* sam_hdr_;
  samFile* sam_file_;


 public:
  /**
   * @brief
   * @param ref_file
   * @param output_file
   */
  explicit Exporter(std::string ref_file, std::string output_file, std::string format);

 ~Exporter();

  /**
   * @brief
   * @param id
   */
  void SkipIn(const util::Section& id) override;

  /**
   * @brief Process one chunk of MPEGG records
   * @param records Input records
   * @param id Block identifier (for multithreading)
   */
  void FlowIn(core::record::Chunk&& records, const util::Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_EXPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
