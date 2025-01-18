/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `FastaSource` class for generating and managing FASTA
 * files.
 *
 * This file contains the implementation of the `FastaSource` class, which is
 * responsible for creating FASTA files using sequence data retrieved from a
 * reference manager. The class also provides methods to stream sequences to an
 * output file in a thread-safe manner.
 *
 * This file is part of the Genie project, a software suite for processing
 * genomic data according to the MPEG-G standard. For more details, refer to the
 * LICENSE file or visit the project's repository at:
 * https://github.com/MueFab/genie.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_
#define SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_

// -----------------------------------------------------------------------------

#include <map>
#include <mutex>
#include <string>

#include "genie/core/reference_manager.h"
#include "genie/util/ordered_lock.h"
#include "genie/util/original_source.h"
#include "genie/util/source.h"

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Provides methods for creating a FASTA file from sequence data.
 *
 * The `FastaSource` class serves as an interface for generating a FASTA file
 * using data from a `ReferenceManager`. It extends the `OriginalSource` and
 * `Source` interfaces, allowing it to act as a producer of sequence strings
 * that are written to an output stream.
 *
 * The class maintains a map of accumulated sequence lengths to facilitate
 * efficient sequence lookups and writing. Thread-safety is ensured using a
 * `OrderedLock` mechanism.
 */
class FastaSource final : public util::OriginalSource,
                          public util::Source<std::string> {
  std::ostream* outfile_;  //!< @brief Output stream for the FASTA file.
  util::OrderedLock
      output_lock_;  //!< @brief Lock mechanism for synchronized writing.
  core::ReferenceManager*
      ref_mgr_;  //!< @brief Reference manager to retrieve sequences.
  std::map<std::string, size_t>
      cumulative_reference_lengths_;  //!< @brief Map to store accumulated
                                      //!< lengths for each sequence.
  size_t line_length_;  //!< @brief Line length for formatting sequences in the
                        //!< FASTA file.

 public:
  /**
   * @brief Constructs a `FastaSource` object.
   *
   * Initializes the `FastaSource` with the given output file stream and
   * reference manager. The class uses the reference manager to access and
   * organize sequence data, which is then formatted and written to the
   * specified output stream.
   *
   * @param outfile Pointer to the output file stream where the FASTA content
   * will be written.
   * @param ref_mgr Pointer to the `ReferenceManager` for managing reference
   * sequences.
   */
  FastaSource(std::ostream* outfile, core::ReferenceManager* ref_mgr);

  /**
   * @brief Pumps sequence data from the source into a string buffer.
   *
   * This method is called to retrieve a sequence string from the reference
   * manager, format it according to FASTA specifications, and store it in an
   * output buffer. The method uses a lock to ensure that data is written in a
   * thread-safe manner.
   *
   * @param id The sequence ID being processed. The ID is used to determine the
   * order of writing.
   * @param lock Mutex lock for synchronizing access to shared resources.
   * @return `true` if the pumping was successful, `false` otherwise.
   */
  bool Pump(uint64_t& id, std::mutex& lock) override;

  /**
   * @brief Flushes any remaining data to the output stream.
   *
   * This method is called when all sequence data has been processed. It ensures
   * that any remaining data in the buffer is flushed to the output file before
   * the stream is closed.
   *
   * @param id The ID associated with the current flushing operation.
   */
  void FlushIn(uint64_t& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
