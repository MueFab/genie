/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright
 * This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 *
 * @brief Defines the class for importing FASTQ files and converting them into
 * MPEG-G records.
 *
 * The `Importer` class reads FASTQ-formatted files, either as single-end or
 * paired-end read files, and converts the reads into MPEG-G format. It handles
 * typical FASTQ file structures with 4 lines per record and performs basic
 * sanity checks on the data before conversion.
 */

#ifndef SRC_GENIE_FORMAT_FASTQ_IMPORTER_H_
#define SRC_GENIE_FORMAT_FASTQ_IMPORTER_H_

// -----------------------------------------------------------------------------

#include <array>
#include <string>
#include <vector>

#include "genie/core/format_importer.h"
#include "genie/core/record/record.h"
#include "genie/util/ordered_lock.h"

// -----------------------------------------------------------------------------

namespace genie::format::fastq {
static constexpr size_t kLinesPerRecord = 4;
/**
 * @brief Module to read FASTQ files and convert them into MPEG-G records.
 *
 * This class converts data from standard FASTQ files (either single-end or
 * paired-end reads) into the MPEG-G genomic record format. The class uses a
 * multi-stream approach to handle paired-end reads and performs checks to
 * ensure that the FASTQ records are valid.
 */
class Importer final : public core::FormatImporter {
  //!< @brief Defines how many lines in a FASTQ file make up one record.
  size_t
      block_size_;  //!< @brief Number of records to read in one `pump()` run.
  std::vector<std::istream*>
      file_list_;           //!< @brief Input streams, supports paired files.
  util::OrderedLock lock_;  //!< @brief Lock to ensure ordered processing in
                            //!< multithreaded contexts.

  /**
   * @brief Enumerations for the different lines in a FASTQ record.
   */
  enum Lines {
    ID = 0,
    SEQUENCE = 1,
    RESERVED = 2,
    QUALITY = 3
  };  //!< @brief FASTQ format lines.

  /**
   * @brief Enumerations for input files in paired-end mode.
   */
  enum Files { FIRST = 0, SECOND = 1 };  //!< @brief Input file shortcuts.

  /**
   * @brief Reads one chunk of FASTQ data from the input file(s).
   *
   * This function reads one chunk of FASTQ data from the provided input
   * streams. If paired-end mode is enabled, it reads from both streams
   * simultaneously, ensuring that the records are synchronized. Each read is
   * split into its 4 constituent lines and stored for further processing.
   *
   * @param file_list Input file streams for the FASTQ files (2 streams for
   * paired-end mode).
   * @return A vector of arrays representing the FASTQ records.
   */
  static std::vector<std::array<std::string, kLinesPerRecord>> ReadData(
      const std::vector<std::istream*>& file_list);

  /**
   * @brief Validates the structure of a FASTQ record.
   *
   * This function performs a basic sanity check to ensure that the provided
   * FASTQ data adheres to the correct structure:
   * - The first line should start with '@'.
   * - The third line should start with '+'.
   * If these conditions are not met, the function throws an exception.
   *
   * @param data One FASTQ record (an array of 4 lines).
   */
  static void SanityCheck(const std::array<std::string, kLinesPerRecord>& data);

  /**
   * @brief Converts the raw FASTQ data into MPEG-G formatted records.
   *
   * This function takes the raw FASTQ data (as read from the input files),
   * processes it, and converts it into MPEG-G formatted records, which can be
   * used for downstream MPEG-G processing and storage.
   *
   * @param data Raw FASTQ data read from the input streams.
   * @return A fully constructed MPEG-G `Record`.
   */
  static core::record::Record BuildRecord(
      std::vector<std::array<std::string, kLinesPerRecord>> data);

 public:
  /**
   * @brief Constructor for unpaired FASTQ import.
   *
   * Initializes the importer for single-end FASTQ files, reading data from a
   * single input stream.
   *
   * @param block_size Number of records to read per `pump()` call.
   * @param file_1 Input stream for the single-end FASTQ file.
   */
  Importer(size_t block_size, std::istream& file_1);

  /**
   * @brief Constructor for paired-end FASTQ import.
   *
   * Initializes the importer for paired-end FASTQ files, reading data from two
   * input streams simultaneously. The records from both streams must be
   * synchronized.
   *
   * @param block_size Number of records to read per `pump()` call.
   * @param file_1 Input stream for the first paired-end FASTQ file (`R1`).
   * @param file_2 Input stream for the second paired-end FASTQ file (`R2`).
   */
  Importer(size_t block_size, std::istream& file_1, std::istream& file_2);

  /**
   * @brief Reads and processes a chunk of FASTQ data, converting it to MPEG-G
   * records.
   *
   * This method reads a chunk of data from the input streams and converts it to
   * MPEG-G format. It ensures that the output data is compatible with the
   * MPEG-G format and structure, and passes the resulting records to the
   * appropriate downstream classifiers.
   *
   * @param classifier The classifier responsible for receiving the converted
   * MPEG-G records.
   * @return True if records were successfully processed and output, false
   * otherwise.
   */
  bool PumpRetrieve(core::Classifier* classifier) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::fastq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTQ_IMPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
