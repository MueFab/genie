/**
 * Copyright 2018-2024 The Genie Authors.
 * @file reorder.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the reordering of reads in paired-end genomic data.
 *
 * This file defines the `reorder_global` structure and associated functions
 * used to reorder reads based on the reference genome and sequence alignment
 * data. The reordering process is essential for efficient encoding and
 * compression of genomic data. This file utilizes various bitset operations and
 * data structures to optimize the process.
 *
 * The file is part of the Spring module within the GENIE project.
 */

#ifndef SRC_GENIE_READ_SPRING_REORDER_H_
#define SRC_GENIE_READ_SPRING_REORDER_H_

// -----------------------------------------------------------------------------

#include <bitset>
#include <string>
#include <vector>

#include "genie/read/spring/bitset_util.h"
#include "genie/read/spring/params.h"
#include "genie/read/spring/util.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Global settings and data for the reordering process.
 *
 * This structure contains the global settings, file paths, and other
 * configurations used for the reordering of reads. The `reorder_global`
 * structure is parameterized with the `bitset_size` template parameter, which
 * determines the Size of the bitsets used for read representation.
 *
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 */
template <size_t BitsetSize>
struct ReorderGlobal {
  /// Number of reads.
  uint32_t num_reads{};

  /// Number of reads per file.
  uint32_t num_reads_array[2]{};

  /// Maximum shift
  int max_shift{};

  /// Number of threads
  int num_thr{};

  /// Maximum read length.
  int max_read_len{};

  /// Number of dictionaries for reordering.
  const int num_dict = kNumDictReorder;

  /// Base directory for input/output files.
  std::string basedir;

  /// Input files for paired-end reads.
  std::string infile[2];

  /// Output file for reordered reads.
  std::string outfile;

  /// Output file for read orientations.
  std::string outfile_rc;

  /// Output file for flags.
  std::string outfile_flag;

  /// Output file for positions.
  std::string outfile_pos;

  /// Output file for read order.
  std::string outfile_order;

  /// Output file for read lengths.
  std::string outfile_read_length;

  /// Indicates if the reads are paired-end.
  bool paired_end{};

  /// Bitset masks for base representation.
  std::vector<std::vector<std::bitset<BitsetSize>>> base_mask;

  /// Bitset mask with 64 bits set to 1.
  std::bitset<BitsetSize> mask64;

  /**
   * @brief Constructor for the reorder_global structure.
   * @param max_read_len_param Maximum read length.
   */
  explicit ReorderGlobal(int max_read_len_param);
};

/**
 * @brief Converts a bitset to a string representation.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param b Bitset to be converted.
 * @param s Output string.
 * @param read_len Length of the read.
 * @param rg Global reorder settings.
 */
template <size_t BitsetSize>
void BitsetToString(std::bitset<BitsetSize> b, char* s, uint16_t read_len,
                    const ReorderGlobal<BitsetSize>& rg);

/**
 * @brief Sets up global arrays for the reordering process.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param rg Global reorder settings.
 */
template <size_t BitsetSize>
void SetGlobalArrays(ReorderGlobal<BitsetSize>& rg);

/**
 * @brief Updates the reference count based on the current read.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param cur Current read bitset.
 * @param ref Reference bitset.
 * @param rev_ref Reverse reference bitset.
 * @param count Array for storing counts.
 * @param reset_count Flag to indicate if the count should be Reset.
 * @param rev Flag to indicate if the read is reverse complemented.
 * @param shift Amount to shift.
 * @param cur_read_len Current read length.
 * @param ref_len Reference length.
 * @param rg Global reorder settings.
 */
template <size_t BitsetSize>
void UpdateRefCount(std::bitset<BitsetSize>& cur, std::bitset<BitsetSize>& ref,
                    std::bitset<BitsetSize>& rev_ref,
                    std::array<std::vector<int>, 4>& count, bool reset_count,
                    bool rev, int shift, uint16_t cur_read_len, int& ref_len,
                    const ReorderGlobal<BitsetSize>& rg);

/**
 * @brief Reads DNA sequences from a file and stores them in bitset format.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param read Array of bitsets to store the reads.
 * @param read_lengths Array of read lengths.
 * @param rg Global reorder settings.
 */
template <size_t BitsetSize>
void ReadDnaFile(std::vector<std::bitset<BitsetSize>>& read,
                 std::vector<uint16_t>& read_lengths,
                 const ReorderGlobal<BitsetSize>& rg);

/**
 * @brief Searches for a match between the current read and the reference.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param ref Reference bitset.
 * @param mask1 Mask for the read.
 * @param dict_lock Lock for the dictionary.
 * @param read_lock Lock for the read.
 * @param mask Array of masks.
 * @param read_lengths Array of read lengths.
 * @param remaining_reads Array of flags indicating remaining reads.
 * @param read Current read bitset.
 * @param dict Dictionary of reads.
 * @param k Current read index.
 * @param rev Flag to indicate if the read is reverse complemented.
 * @param shift Amount to shift.
 * @param ref_len Reference length.
 * @param rg Global reorder settings.
 * @param num_reads_remaining
 * @param last_progress
 * @param local_reads_used
 * @return True if a match is found, false otherwise.
 */
template <size_t BitsetSize>
bool SearchMatch(
    const std::bitset<BitsetSize>& ref,
    const std::vector<std::bitset<BitsetSize>>& mask1,
    std::vector<std::mutex>& dict_lock, std::vector<std::mutex>& read_lock,
    std::vector<std::vector<std::bitset<BitsetSize>>>& mask,
    std::vector<uint16_t>& read_lengths, std::vector<uint8_t>& remaining_reads,
    std::vector<std::bitset<BitsetSize>>& read, std::vector<BbHashDict>& dict,
    uint32_t& k, bool rev, int shift, const int& ref_len,
    const ReorderGlobal<BitsetSize>& rg, uint32_t& num_reads_remaining,
    float& last_progress, uint32_t& local_reads_used);

/**
 * @brief Reorders the reads based on the reference sequence.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param read Array of read bitsets.
 * @param dict Dictionary of reads.
 * @param read_lengths Array of read lengths.
 * @param rg Global reorder settings.
 */
template <size_t BitsetSize>
void Reorder(std::vector<std::bitset<BitsetSize>>& read,
             std::vector<BbHashDict>& dict, std::vector<uint16_t>& read_lengths,
             const ReorderGlobal<BitsetSize>& rg);

/**
 * @brief Converts bitsets to string format and writes them to files.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param read Array of read bitsets.
 * @param read_lengths Array of read lengths.
 * @param rg Global reorder settings.
 */
template <size_t BitsetSize>
void WriteToFile(std::vector<std::bitset<BitsetSize>>& read,
                 std::vector<uint16_t>& read_lengths,
                 ReorderGlobal<BitsetSize>& rg);

/**
 * @brief Main function for the reordering process.
 * @tparam BitsetSize Template parameter specifying the bitset Size.
 * @param temp_dir Temporary directory for intermediate files.
 * @param cp Compression parameters.
 */
template <size_t BitsetSize>
void ReorderMain(const std::string& temp_dir, const CompressionParams& cp);

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#include "genie/read/spring/reorder.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_REORDER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
