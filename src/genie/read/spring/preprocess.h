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
  CompressionParams
      cp;  //!< Compression parameters for the pre-processing step.

  std::string outfile_clean[2];  //!< File paths for cleaned reads (one
                                 //!< for each paired-end read).
  std::string
      outfile_n[2];  //!< File paths for reads containing `N` bases.
  std::string outfile_order_n[2];  //!< File paths for order of reads
                                   //!< containing `N` bases.
  std::string outfile_id;          //!< File path for storing read IDs.
  std::string
      outfile_quality[2];  //!< File paths for storing quality values.
  std::string outfile_read_length[2];  //!< File paths for storing read
                                       //!< lengths.

  std::ofstream
      f_out_clean[2];  //!< Output file streams for cleaned reads.
  std::ofstream
      f_out_n[2];  //!< Output file streams for reads with `N` bases.
  std::ofstream f_out_order_n[2];  //!< Output file streams for order
                                   //!< of `N` reads.
  std::ofstream f_out_id;          //!< Output file stream for read IDs.
  std::ofstream
      f_out_quality[2];  //!< Output file streams for quality values.

  std::string temp_dir;  //!< Temporary directory for storing
                         //!< intermediate files.
  std::string
      working_dir;  //!< Working directory for the pre-processing step.

  util::OrderedLock
      lock;  //!< Synchronization lock for multithreaded processing.

  core::stats::PerfStats
      stats;  //!< Performance statistics for the pre-processing step.

  /**
   * @brief Get the performance statistics.
   * @return Reference to the performance statistics object.
   */
  core::stats::PerfStats& GetStats();

  bool used =
      false;  //!< Flag to indicate if the preprocessor has been used.

  /**
   * @brief Set up the preprocessor with the specified parameters.
   * @param working_dir_p Directory for storing temporary files.
   * @param num_thr Number of threads to use for processing.
   * @param paired_end Flag indicating if the reads are paired-end.
   */
  void Setup(const std::string& working_dir_p, size_t num_thr, bool paired_end);

  /**
   * @brief Destructor to clean up resources.
   */
  ~Preprocessor();

  /**
   * @brief Preprocess a chunk of genomic reads.
   * @param t Chunk of reads to preprocess.
   * @param id Unique section ID for synchronization.
   */
  void Preprocess(core::record::Chunk&& t, const util::Section& id);

  /**
   * @brief Skip the processing for a specific section.
   * @param id Section ID to skip.
   */
  void Skip(const util::Section& id);

  /**
   * @brief Finalize the preprocessor, closing files and cleaning up.
   * @param id Position to finalize at.
   */
  void Finish(size_t id);
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_PREPROCESS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
