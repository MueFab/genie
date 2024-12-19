/**
 * Copyright 2018-2024 The Genie Authors.
 * @file bitset_util.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for bitset utilities used in the Spring module of Genie
 *
 * This file contains function templates and class definitions used for
 * various bitset operations such as conversion, mask generation, and
 * dictionary construction in the Spring module of Genie
 *
 * The `BbHashDict` class utilizes the Boomphf library to create
 * perfect hash functions and provides utilities to manage dictionary-based
 * read indexing and compression.
 */

#ifndef SRC_GENIE_READ_SPRING_BITSET_UTIL_H_
#define SRC_GENIE_READ_SPRING_BITSET_UTIL_H_

// -----------------------------------------------------------------------------

#include <bbhash/BooPHF.h>

#include <bitset>
#include <memory>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------

using HashObj = boomphf::SingleHashFunctor<uint64_t>;
using BooHashFunType = boomphf::mphf<uint64_t, HashObj>;

/**
 * @class BbHashDict
 * @brief Dictionary class for perfect hashing with BooPHF.
 *
 * This class manages a dictionary of reads using Boomphf minimal perfect
 * hash functions. It provides utilities to find positions, remove entries,
 * and track whether specific bins in the dictionary are empty.
 */
class BbHashDict {
 public:
  /// BooPHF hash function
  std::unique_ptr<BooHashFunType> boo_hash_fun_;

  /// Starting position for the dictionary
  int start_;

  /// Ending position for the dictionary
  int end_;
  /// Number of keys in the dictionary
  uint32_t num_keys_;

  /// Number of reads in this dictionary (for variable length)
  uint32_t dict_num_reads_;

  /// Start positions for reads
  std::vector<uint32_t> start_pos_;

  /// Read IDs for tracking
  std::vector<uint32_t> read_id_;

  /// Flags indicating empty bins in the dictionary
  std::vector<uint8_t> empty_bin_;

  /**
   * @brief Find the position of a given key.
   * @param dict_idx Pointer to the dictionary index to fill.
   * @param start_pos_idx Starting position index to search for.
   */
  void FindPos(int64_t* dict_idx, const uint64_t& start_pos_idx) const;

  /**
   * @brief Remove an entry from the dictionary.
   * @param dict_idx Pointer to the dictionary index.
   * @param start_pos_idx Starting position index.
   * @param current Current position to remove.
   */
  void Remove(const int64_t* dict_idx, const uint64_t& start_pos_idx,
              int64_t current);
};

// -----------------------------------------------------------------------------
/**
 * @brief Convert a string into a bitset based on character values.
 * @tparam BitsetSize Size of the bitset.
 * @param s String to convert.
 * @param read_length Length of the string to convert.
 * @param b Bitset to store the converted values.
 * @param base_mask Base masks to assist in conversion.
 */
template <size_t BitsetSize>
void StringToBitset(
    const std::string& s, uint16_t read_length, std::bitset<BitsetSize>& b,
    const std::vector<std::vector<std::bitset<BitsetSize>>>& base_mask);

/**
 * @brief Generate index masks for dictionary lookup.
 * @tparam BitsetSize Size of the bitset.
 * @param mask1 Array of bitsets to store the masks.
 * @param dict Pointer to the dictionary for which masks are generated.
 * @param num_dict Number of dictionaries.
 * @param bpb Bits per base (typically 2 for DNA sequences).
 */
template <size_t BitsetSize>
void GenerateIndexMasks(std::vector<std::bitset<BitsetSize>>& mask1,
                        BbHashDict* dict, int num_dict, int bpb);

struct SizeRange {
  uint32_t start;
  uint32_t end;
};

using DictSizes = std::array<SizeRange, 2>;

/**
 * @brief Construct a dictionary using the given reads and parameters.
 * @tparam BitsetSize Size of the bitset.
 * @param read Array of reads in bitset format.
 * @param dict_sizes Dictionary sizes for the first two dictionaries.
 * @param read_lengths Array storing the lengths of each read.
 * @param num_dict Number of dictionaries.
 * @param num_reads Total number of reads.
 * @param bpb Bits per base (typically 2 for DNA sequences).
 * @param basedir Directory for storing intermediate results.
 * @param num_threads Number of threads to use for parallel processing.
 */
template <size_t BitsetSize>
std::vector<BbHashDict> ConstructDictionary(
    const std::vector<std::bitset<BitsetSize>>& read,
    const std::vector<uint16_t>& read_lengths, int num_dict,
    const uint32_t& num_reads, int bpb, const std::string& basedir,
    const int& num_threads, const DictSizes& dict_sizes);

/**
 * @brief Generate masks for each base in the reads.
 * @tparam BitsetSize Size of the bitset.
 * @param mask Pointer to an array of bitsets to store the masks.
 * @param max_read_len Maximum read length.
 * @param bpb Bits per base (typically 2 for DNA sequences).
 */
template <size_t BitsetSize>
std::vector<std::vector<std::bitset<BitsetSize>>> GenerateMasks(
    uint32_t max_read_len, uint8_t bpb);

/**
 * @brief Convert a character array into a bitset based on base values.
 * @tparam BitsetSize Size of the bitset.
 * @param s Character array representing the DNA sequence.
 * @param read_len Length of the sequence.
 * @param b Bitset to store the converted values.
 * @param base_mask Base masks to assist in conversion.
 */
template <size_t BitsetSize>
std::bitset<BitsetSize> CharToBitset(
    const std::string& s, size_t read_len,
    const std::vector<std::vector<std::bitset<BitsetSize>>>& base_mask);

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#include "genie/read/spring/bitset_util.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_BITSET_UTIL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
