/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder_source.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the SpringSource class in the Spring module of Genie
 *
 * The SpringSource class inherits from util::OriginalSource and
 * util::Source<core::AccessUnit> to provide encoded access units for the Spring
 * module. This class manages the process of generating and encoding read
 * sequences using the parameters defined in the Spring module. It handles
 * access unit management, performance tracking, and temporary file handling.
 */

#ifndef SRC_GENIE_READ_SPRING_ENCODER_SOURCE_H_
#define SRC_GENIE_READ_SPRING_ENCODER_SOURCE_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/core/stats/perf_stats.h"
#include "genie/read/spring/util.h"
#include "genie/util/original_source.h"
#include "genie/util/source.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Manages encoded read sequences and access units for the Spring module.
 *
 * The SpringSource class reads the temporary files generated during
 * preprocessing, organizes them into access units, and handles the encoding of
 * read sequences according to the provided parameters. It is responsible for
 * tracking performance statistics and ensuring that the encoding process
 * follows the constraints defined in the compression parameters.
 */
class SpringSource final : public util::OriginalSource,
                           public util::Source<core::AccessUnit> {
  uint32_t num_a_us_;  //!< Number of access units to be generated.
  std::string
      read_desc_prefix_;  //!< Prefix for the read description files.
  std::string id_desc_prefix_;  //!< Prefix for the ID description files.
  std::string quality_desc_prefix_;  //!< Prefix for the quality
                                     //!< description files.
  std::vector<uint32_t>
      num_records_per_au_;  //!< Number of records per access unit.
  std::vector<uint32_t>
      num_reads_per_au_;  //!< Number of reads per access unit.
  uint32_t au_id_;        //!< ID of the current access unit.
  std::vector<core::parameter::EncodingSet>&
      params_;  //!< Encoding parameters for the access units.

  core::stats::PerfStats stats_;  //!< Performance statistics for
                                  //!< tracking encoding efficiency.

 public:
  /**
   * @brief Constructor for SpringSource.
   *
   * Initializes the SpringSource class with the specified temporary
   * directory, compression parameters, encoding sets, and performance
   * statistics.
   *
   * @param temp_dir The temporary directory containing preprocessed files.
   * @param cp Compression parameters for the Spring module.
   * @param p Reference to encoding parameters.
   * @param s Performance statistics object.
   */
  SpringSource(const std::string& temp_dir, const CompressionParams& cp,
               std::vector<core::parameter::EncodingSet>& p,
               core::stats::PerfStats s);

  /**
   * @brief Generates and pumps the next access unit.
   *
   * Reads the preprocessed files and organizes them into an access unit,
   * which is then provided to the caller.
   *
   * @param id Reference to the access unit ID.
   * @param lock Mutex lock for thread safety.
   * @return True if an access unit was successfully generated, false
   * otherwise.
   */
  bool Pump(uint64_t& id, std::mutex& lock) override;

  /**
   * @brief Flushes the input at a specific position.
   *
   * Manages any pending read sequences or temporary files and ensures that
   * the access units are flushed correctly to avoid data loss.
   *
   * @param pos Position to Flush.
   */
  void FlushIn(uint64_t& pos) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_ENCODER_SOURCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
