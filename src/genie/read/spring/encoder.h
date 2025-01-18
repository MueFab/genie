/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 * @brief Header file for the Encoder class in the Spring module of Genie
 *
 * This file defines the Encoder class, which is responsible for managing
 * the encoding process for read sequences in the Spring module. It handles
 * the preprocessing, organizing of records into chunks, and managing the
 * multithreaded flow of data during the encoding process.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_ENCODER_H_
#define SRC_GENIE_READ_SPRING_ENCODER_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/read_encoder.h"
#include "genie/read/spring/preprocess.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Encoder class for the Spring module.
 *
 * This class inherits from `genie::core::ReadEncoder` and is designed to manage
 * the process of encoding read sequences into access units. The Encoder class
 * utilizes the Preprocessor class to preprocess input records and generates the
 * final encoded access units that can be stored or further processed.
 */
class Encoder final : public core::ReadEncoder {
  /// Handles preprocessing of input records and file management.
  Preprocessor preprocessor_;
  size_t preprocess_progress_printed_;

 public:
  /**
   * @brief Constructor for the Spring Encoder.
   *
   * Initializes the Encoder class with the specified parameters, sets up the
   * preprocessor, and configures the multithreading options.
   *
   * @param working_dir The working directory for storing temporary files.
   * @param num_thr The number of threads to be used for encoding.
   * @param paired_end Indicates if the reads are paired-end.
   * @param write_raw Flag to determine if raw data should be written.
   */
  explicit Encoder(const std::string& working_dir, size_t num_thr,
                   bool paired_end, bool write_raw);

  /**
   * @brief Processes an incoming chunk of records.
   *
   * This function is called to feed chunks of records into the encoding
   * pipeline. It utilizes the preprocessor to handle the records and manages
   * synchronization using the `util::Section` parameter.
   *
   * @param t A chunk of records to be processed.
   * @param id The section ID for multithreading synchronization.
   */
  void FlowIn(core::record::Chunk&& t, const util::Section& id) override;

  /**
   * @brief Flushes the input to ensure all records are processed.
   *
   * This function is called to ensure that all pending records are processed
   * and no data is left unencoded. It finalizes the encoding process for the
   * given position.
   *
   * @param pos The position to Flush.
   */
  void FlushIn(uint64_t& pos) override;

  /**
   * @brief Skips processing for a given section ID.
   *
   * This function is used when certain sections of input data should be
   * ignored or skipped without affecting the overall encoding state.
   *
   * @param id The section ID to be skipped.
   */
  void SkipIn(const util::Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
