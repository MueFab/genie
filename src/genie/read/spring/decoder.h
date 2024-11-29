/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 * @brief Header file for the Decoder class in the Spring module of Genie
 *
 * This file defines the Decoder class, which is responsible for managing the
 * decoding process for read sequences in the Spring module. It handles the
 * extraction, matching, and processing of the records from the encoded streams
 * and supports paired-end reads and combined pairs decoding.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_DECODER_H_
#define SRC_GENIE_READ_SPRING_DECODER_H_

// -----------------------------------------------------------------------------

#include <filesystem>  // NOLINT
#include <string>
#include <vector>

#include "genie/core/read_decoder.h"
#include "genie/read/spring/util.h"
#include "genie/util/ordered_section.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Structure to store individual records.
 *
 * Each record contains its name, sequence, and quality values. This structure
 * is used to represent both matched and unmatched records during the decoding
 * process.
 */
struct Record {
  std::string name;  //!< @brief Name of the record.
  std::string seq;   //!< @brief Sequence data of the record.
  std::string qv;    //!< @brief Quality values of the record.
};

/**
 * @brief Decodes streams from an AccessUnit into structured records.
 *
 * This function takes an AccessUnit and decodes its streams into structured
 * records. It handles paired-end reads, manages the matching of records, and
 * extracts names and quality values.
 *
 * @param au The AccessUnit containing the encoded streams.
 * @param paired_end Flag indicating if the reads are paired-end.
 * @param combine_pairs Flag indicating if pairs should be combined.
 * @param matched_records Container for matched records.
 * @param unmatched_records Container for unmatched records.
 * @param names Container for read names.
 * @param qvs Container for quality values.
 */
void DecodeStreams(core::AccessUnit& au, bool paired_end, bool combine_pairs,
                   std::array<std::vector<Record>, 2>& matched_records,
                   std::array<std::vector<Record>, 2>& unmatched_records,
                   std::vector<std::string>& names,
                   std::vector<std::string>& qvs);

/**
 * @brief Decoder class for the Spring module.
 *
 * This class inherits from `genie::core::ReadDecoder` and is designed to
 * manage the process of decoding read sequences from access units. It supports
 * both single-end and paired-end reads, and handles the synchronization of
 * records using the `util::OrderedLock` mechanism.
 */
class Decoder final : public core::ReadDecoder {
  CompressionParams cp_{};  //!< @brief Compression parameters.
  bool combine_pairs_;      //!< @brief Flag for combined pairs decoding.
  util::OrderedLock lock_;  //!< @brief Lock for multithreaded decoding.
  std::ofstream file_out_unmatched1_;  //!< @brief Output stream for unmatched
                                       //!< reads (1st file).
  std::ofstream file_out_unmatched2_;  //!< @brief Output stream for unmatched
                                       //!< reads (2nd file).
  std::ofstream file_out_unmatched_read_names_1_;  //!< @brief Output stream for
  //!< unmatched read names (1st file).
  std::ofstream file_out_unmatched_read_names_2_;  //!< @brief Output stream for
  //!< unmatched read names (2nd file).

  std::string
      file_unmatched_fastq1_;  //!< @brief Path to unmatched FASTQ (1st file).
  std::string
      file_unmatched_fastq2_;  //!< @brief Path to unmatched FASTQ (2nd file).
  std::string file_unmatched_read_names_1_;  //!< @brief Path to unmatched
                                             //!< read names (1st file).
  std::string file_unmatched_read_names_2_;  //!< @brief Path to unmatched
                                             //!< read names (2nd file).
  uint32_t unmatched_record_index_[2];       //!< @brief Index counters for
                                             //!< unmatched records.
  std::string basedir_;  //!< @brief Base directory for output files.

 public:
  /**
   * @brief Constructor for the Spring Decoder.
   *
   * Initializes the Decoder class with the specified working directory and
   * configuration flags.
   *
   * @param working_dir The working directory for storing temporary files.
   * @param comb_p Flag for combined pairs decoding.
   * @param paired_end Flag indicating if the reads are paired-end.
   */
  explicit Decoder(const std::string& working_dir, bool comb_p,
                   bool paired_end);

  /**
   * @brief Processes an incoming AccessUnit.
   *
   * This function is called to feed an AccessUnit into the decoding pipeline.
   * It handles the extraction of records and manages synchronization using
   * the `util::Section` parameter.
   *
   * @param t The AccessUnit to be processed.
   * @param id The section ID for multithreading synchronization.
   */
  void FlowIn(core::AccessUnit&& t, const util::Section& id) override;

  /**
   * @brief Reads a record from a file stream.
   *
   * This function reads a `Record` object from the specified file stream and
   * populates the fields of the record structure.
   *
   * @param i Input file stream.
   * @param r Record structure to be populated.
   */
  static void ReadRec(std::ifstream& i, Record& r);

  /**
   * @brief Adds a record to the specified chunk.
   *
   * This function inserts a given record into the provided chunk and updates
   * the position counter.
   *
   * @param chunk Chunk to which the record will be added.
   * @param r Record to be added.
   * @param pos Position counter to be updated.
   */
  void Add(core::record::Chunk& chunk, core::record::Record&& r, uint64_t& pos);

  /**
   * @brief Flushes the input to ensure all records are processed.
   *
   * This function is called to ensure that all pending records are processed
   * and no data is left. It finalizes the decoding process for the
   * given position.
   *
   * @param pos The position to Flush.
   */
  void FlushIn(uint64_t& pos) override;

  /**
   * @brief Skips processing for a given section ID.
   *
   * This function is used when certain sections of input data should be
   * ignored or skipped without affecting the overall decoding state.
   *
   * @param id The section ID to be skipped.
   */
  void SkipIn(const util::Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
