/**
 * Copyright 2018-2024 The Genie Authors.
 * @file preprocess.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the preprocessor of genomic reads in the Spring
 * module.
 *
 * This file defines the `Preprocessor` struct, which is responsible for the
 * initial pre-processing of genomic reads before further encoding and
 * compression steps. The preprocessor handles tasks such as cleaning reads,
 * separating reads with `N` bases, and managing file outputs for different
 * categories of read data. It interacts with the Spring compression module to
 * prepare the data for efficient storage and analysis.
 *
 * This file is part of the Spring module within the GENIE project.
 */

#ifndef SRC_GENIE_READ_SPRING_PREPROCESS_H_
#define SRC_GENIE_READ_SPRING_PREPROCESS_H_

// -----------------------------------------------------------------------------

#include <filesystem>  // NOLINT
#include <optional>
#include <string>

#include "genie/core/record/chunk.h"
#include "genie/read/spring/util.h"
#include "genie/util/drain.h"
#include "genie/util/ordered_lock.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Preprocessor structure for handling read pre-processing steps.
 *
 * The `Preprocessor` structure is used for handling various pre-processing
 * steps required for genomic reads before they are fed into the compression
 * pipeline. This includes managing temporary files for cleaned reads, reads
 * containing `N` bases, quality values, and read lengths, as well as
 * maintaining order and synchronization across multiple threads.
 */
struct Preprocessor {
 private:
  void preprocess_segment(const core::record::Segment& seg,
                                        size_t seg_index, size_t rec_index);

  // ---------------------------------------------------------------------------

  void preprocess_record(const core::record::Record& rec,
  size_t record_index);

 public:
  CompressionParams cp;

  std::array<std::string, 2> outfile_clean;
  std::array<std::string, 2> outfile_n;
  std::array<std::string, 2> outfile_order_n;
  std::string outfile_id;
  std::array<std::string, 2> outfile_quality;

  std::array<std::ofstream, 2> f_out_clean;
  std::array<std::ofstream, 2> f_out_n;
  std::array<std::ofstream, 2> f_out_order_n;
  std::ofstream f_out_id;
  std::array<std::ofstream, 2> f_out_quality;

  std::string temp_dir;
  std::string working_dir;

  util::OrderedLock lock;
  core::stats::PerfStats stats;

  size_t preprocess_progress_printed;

  bool used = false;

  core::stats::PerfStats& GetStats() const;
  void Setup(const std::string& working_dir_p, size_t num_thr, bool paired_end);
  void Preprocess(core::record::Chunk&& t, const util::Section& id);
  void Skip(const util::Section& id);
  void Finish(size_t id);
  ~Preprocessor();
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_PREPROCESS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
