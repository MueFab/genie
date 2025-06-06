/**
 * Copyright 2018-2024 The Genie Authors.
 * @file local_reference.h
 * @brief Header file for the `LocalReference` class in the `localassembly`
 * namespace.
 *
 * This file defines the `LocalReference` class which handles local assembly
 * reference management. It provides methods to process, store, and generate
 * references using local sequence data, supporting the Local Assembly coding in
 * the GENIE framework.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_LOCAL_REFERENCE_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_LOCAL_REFERENCE_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/core/record/record.h"

// -----------------------------------------------------------------------------

namespace genie::read::localassembly {

/**
 * @brief Manages local reference sequences used for encoding.
 *
 * This class handles the local reference generation and management for the
 * Local Assembly module. It allows for the generation of reference sequences
 * based on local reads and cigar strings, and maintains an internal buffer to
 * efficiently manage sequences for local reference computation.
 */
class LocalReference {
 public:
  /// Maximum buffer Size for the local reference sequences.
  uint32_t cr_buf_max_size_;

  /// Buffer storing sequence strings.
  std::vector<std::string> sequences_;

  /// Positions corresponding to the sequences in the buffer.
  std::vector<uint64_t> sequence_positions_;

  /// Current Size of the buffer.
  uint32_t cr_buf_size_;

  /**
   * @brief Generate a reference sequence of specified length starting at a
   * given offset.
   * @param offset The starting position of the reference to generate.
   * @param len The length of the reference sequence to generate.
   * @return The generated reference sequence.
   */
  [[nodiscard]] std::string GenerateRef(uint32_t offset, uint32_t len) const;

  /**
   * @brief Perform a majority vote on a given offset to determine the most
   * common base.
   * @param offset_to_first Offset position to perform the majority vote on.
   * @return The character representing the majority base.
   */
  [[nodiscard]] char MajorityVote(uint32_t offset_to_first) const;

  /**
   * @brief Preprocess a read based on the provided cigar string.
   * @param read The sequence of the read.
   * @param cigar The cigar string corresponding to the read.
   * @return The preprocessed sequence based on the cigar operations.
   */
  static std::string preprocess(const std::string& read,
                                const std::string& cigar);

  /**
   * @brief Calculate the length of a sequence from its cigar string.
   * @param cigar The cigar string of the sequence.
   * @return The length of the sequence as described by the cigar string.
   */
  static uint32_t LengthFromCigar(const std::string& cigar);

  /**
   * @brief Constructor for the LocalReference class.
   * @param cr_buf_max_size Maximum buffer Size for the local reference
   * sequences.
   */
  explicit LocalReference(uint32_t cr_buf_max_size);

  /**
   * @brief Add a single read to the reference buffer.
   * @param record The sequence of the read.
   * @param ecigar The extended cigar string corresponding to the read.
   * @param position The position of the read.
   */
  void AddSingleRead(const std::string& record, const std::string& ecigar,
                     uint64_t position);

  /**
   * @brief Add a complete record to the reference buffer.
   * @param s The record to add.
   */
  void AddRead(const core::record::Record& s);

  /**
   * @brief Retrieve a reference sequence using the specified position and cigar
   * string.
   * @param pos_offset The starting position of the reference.
   * @param cigar The cigar string indicating the alignment operations.
   * @return The corresponding reference sequence.
   */
  [[nodiscard]] std::string GetReference(uint32_t pos_offset,
                                         const std::string& cigar) const;

  /**
   * @brief Retrieve a reference sequence of specified length starting at a
   * given position.
   * @param pos_offset The starting position of the reference.
   * @param len The length of the reference sequence to retrieve.
   * @return The corresponding reference sequence.
   */
  [[nodiscard]] std::string GetReference(uint32_t pos_offset,
                                         uint32_t len) const;

  /**
   * @brief Get the position of the window border.
   * @return The current window border position.
   */
  [[nodiscard]] uint64_t GetWindowBorder() const;

  /**
   * @brief Print the current window of the reference buffer.
   */
  void PrintWindow() const;

  /**
   * @brief Get the maximum buffer Size.
   * @return The maximum buffer Size.
   */
  [[nodiscard]] uint32_t GetMaxBufferSize() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_LOCAL_REFERENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
