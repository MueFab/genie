/**
 * Copyright 2018-2024 The Genie Authors.
 * @file importer.h
 * @brief Header file for the MGREC format importer module in Genie.
 *
 * This file defines the `Importer` class used for reading MGREC formatted files
 * and converting them into an internal record structure for use in the Genie
 * framework. The importer handles various record validation, supports
 * performance tracking, and manages unsupported records.
 *
 * @copyright This file is part of Genie.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGREC_IMPORTER_H_
#define SRC_GENIE_FORMAT_MGREC_IMPORTER_H_

// -----------------------------------------------------------------------------

#include "genie/core/classifier.h"
#include "genie/core/format_importer.h"
#include "genie/core/record/chunk.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgrec {

/**
 * @class Importer
 * @brief MGREC format importer for reading and processing records.
 *
 * The `Importer` class is designed to handle the import of MGREC formatted
 * files, processing them into the Genie framework's internal record structure.
 * It provides functionality for validating record types, tracking performance
 * metrics, and handling unsupported records.
 */
class Importer final : public core::FormatImporter {
  size_t block_size_;       //!< Block Size for processing chunks
  util::BitReader reader_;  //!< Bit reader for the input stream
  util::BitWriter writer_;  //!< Bit writer for unsupported record output

  size_t discarded_splices_{};        //!< Number of discarded spliced records
  size_t discarded_long_distance_{};  //!< Number of discarded long distance
                                      //!< records
  size_t discarded_hm_{};             //!< Number of discarded hard mask records
  size_t discarded_missing_pair_u_{};  //!< Number of discarded unmatched pair
                                       //!< records
  size_t missing_additional_alignments_{};  //!< Number of records missing
                                            //!< additional alignments

  std::optional<core::record::Record>
      buffered_record_;  //!< Buffer to hold the current record being processed
  bool check_support_;   //!< Flag to enable/disable record support checking

  /**
   * @brief Check if a given record is supported by the importer.
   * @param rec Record to be validated.
   * @return `true` if the record is supported, `false` otherwise.
   */
  bool IsRecordSupported(const core::record::Record& rec);

 public:
  /**
   * @brief Construct an MGREC importer.
   * @param block_size Size of blocks to read from the input.
   * @param file_1 Input stream for the MGREC file.
   * @param unsupported Output stream for unsupported records.
   * @param check_support Flag to enable/disable support checking.
   */
  Importer(size_t block_size, std::istream& file_1, std::ostream& unsupported,
           bool check_support = true);

  /**
   * @brief Retrieve and classify records from the input stream.
   * @param classifier Classifier for managing records.
   * @return `true` if records were successfully retrieved, `false` otherwise.
   */
  bool PumpRetrieve(core::Classifier* classifier) override;

  /**
   * @brief Print statistics about discarded records.
   */
  void PrintStats() const;

  /**
   * @brief Flush any remaining records into the classifier.
   * @param pos Current position indicator.
   */
  void FlushIn(uint64_t& pos) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgrec

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGREC_IMPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
