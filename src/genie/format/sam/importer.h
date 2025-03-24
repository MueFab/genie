/**
 * Copyright 2018-2024 The Genie Authors.
 * @file importer.h
 *
 * @brief Declares the SAM Importer module for converting SAM/FASTQ files into
 * MPEG-G record format.
 *
 * This file is part of the Genie project and defines the `Importer` class that
 * facilitates the conversion of genomic sequencing data from SAM/FASTQ format
 * into the MPEG-G record (MGREC) format. The file also introduces supporting
 * structures such as `RefInfo` for reference genome management and utilities
 * for handling file merging and sorting during the import process.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_IMPORTER_H_
#define SRC_GENIE_FORMAT_SAM_IMPORTER_H_

#define PHASE1_EXT ".phase1.mgrec"
#define PHASE2_BUFFER_SIZE 1000000

// -----------------------------------------------------------------------------

#include <genie/format/fasta/manager.h>
#include <genie/format/sam/sam_parameter.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/format_importer.h"
#include "genie/format/sam/sam_record.h"
#include "genie/format/sam/sam_reader.h"
#include "genie/format/sam/sam_sorter.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

class RefInfo {
  /// Reference manager
  std::unique_ptr<core::ReferenceManager> ref_mgr_;

  /// FASTA manager
  std::unique_ptr<fasta::Manager> fasta_mgr_;

  /// FASTA file stream
  std::unique_ptr<std::istream> fasta_file_;

  /// FAI file stream
  std::unique_ptr<std::istream> fai_file_;

  /// SHA file stream
  std::unique_ptr<std::istream> sha_file_;

  /// Flag indicating if the reference is valid
  bool valid_;

 public:
  /**
   * @brief Construct reference information from FASTA file
   * @param fasta_name Path to the reference FASTA file
   */
  explicit RefInfo(const std::string& fasta_name);

  /**
   * @brief Check if the reference is valid
   * @return True if the reference is valid
   */
  [[nodiscard]] bool IsValid() const;

  /**
   * @brief Get the reference manager
   * @return Reference manager
   */
  [[nodiscard]] core::ReferenceManager* GetMgr() const;
};

class RefIdWatcher {
  std::optional<int> ref_id_;

 public:
  bool watch(int id) {
    if (ref_id_ == std::nullopt) {
      ref_id_ = id;
      return false;
    }
    if (ref_id_ != id) {
      ref_id_ = id;
      return true;
    }
    return false;
  }

  [[nodiscard]] std::optional<int> get() const { return ref_id_; }
};

/**
 * @brief Module to reads fastq files and convert them into MPEGG-Record format
 */
class Importer final : public core::FormatImporter {
  /// How many records to read in one pump() run
  size_t block_size_;

  /// Path to the input SAM file
  std::string input_sam_file_;

  /// Path to the reference FASTA file
  std::string input_ref_file_;

  /// Number of reference sequences
  int nref_;

  /// Flag indicating if phase 1 is complete
  bool phase1_complete_;

  /// Map between reference names and ids
  std::vector<std::pair<std::string, size_t>> refs_;

  /// Map between SAM header and FASTA reference ids
  std::vector<size_t> sam_hdr_to_fasta_lut_;

  /// Counter of how many records were removed due to unsupported bases
  size_t removed_unsupported_base_ = 0;

  /// Reference information
  RefInfo refinf_;

  std::pair<std::vector<SamRecord>, std::optional<int>> ReadSamChunk();

  /// Protects the sam reader and sam sorter
  std::mutex lock_;

  /// Set to true when no more reads in the input file
  bool eof_;

  /// Reader for sam records
  SamReader sam_reader_;

  /// Sorter for sam records
  SamSorter sorter_;

  /**
   * Matches all possible sam records into pairs using the sam sorter
   * @param records Unmatched records
   * @return Matched records
   */
  std::vector<SamRecordPair> MatchPairs(std::vector<SamRecord>&& records);

 public:
  /**
   * @brief Build importer from file paths
   * @param block_size How many records to read in one pump() run
   * @param input Path to the input SAM file
   * @param ref Path to the reference FASTA file
   */
  Importer(size_t block_size, std::string input, std::string ref);

  /**
   * @brief Execute phase 1 of the transcoding process which is the conversion
   * to MGREC format. (phase 2 is sorting according to position)
   * @param options Configuration parameters for the operation
   * @param chunk_id Chunk counter
   * @return List of reference names and their lengths
   */
  std::vector<std::pair<std::string, size_t>> sam_to_mgrec_phase1(
      const Config& options, int& chunk_id);

  /**
   * @brief Load the chunk of records from the phase 1 transcoding and
   prepare mergesort for phase 2
   * @param num_chunks Number of chunks to load
   */
  void setup_merge(int num_chunks);

  /**
   * @brief Load one chunk of records and pass to classifier
   * @param classifier The classifier to regroup the records
   * @return True if more records are available
   */
  bool PumpRetrieve(core::Classifier* classifier) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_IMPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
