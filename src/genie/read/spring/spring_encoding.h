/**
 * Copyright 2018-2024 The Genie Authors.
 * @file spring_encoding.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for Spring encoding of genomic reads using bitset
 * manipulation.
 *
 * This file defines structures and functions used for encoding DNA sequences
 * using a bitset-based representation. The encoding process involves handling
 * contig sequences, managing compression parameters, and utilizing bitset-based
 * structures to efficiently encode DNA data. It also defines various helper
 * functions for converting between string representations and bitset-based
 * formats.
 *
 * @tparam bitset_size The Size of the bitset used for encoding.
 */

#ifndef SRC_GENIE_READ_SPRING_SPRING_ENCODING_H_
#define SRC_GENIE_READ_SPRING_SPRING_ENCODING_H_

// -----------------------------------------------------------------------------

#include <bitset>
#include <list>
#include <string>
#include <vector>

#include "genie/read/spring/bitset_util.h"
#include "genie/read/spring/params.h"
#include "genie/read/spring/util.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Structure holding global variables for the Spring encoder.
 * @tparam BitsetSize The Size of the bitset used for encoding.
 */
template <size_t BitsetSize>
struct EncoderGlobalB {
  /// @brief Base mask used for bitset conversion of A, C, G, T, N bases.
  std::vector<std::vector<std::bitset<BitsetSize>>> base_mask;
  int max_read_len;  //!< @brief Maximum read length for the sequences.
  std::bitset<BitsetSize> mask63;  //!< @brief Bitset with 63 bits set to 1

  /**
   * @brief Constructor to initialize the base mask and other structures.
   * @param max_read_len_param Maximum read length.
   */
  explicit EncoderGlobalB(int max_read_len_param);
};

/**
 * @brief Global configuration and file paths for the Spring encoder.
 */
struct EncoderGlobal {
  uint32_t num_reads{};    //!< @brief Total number of reads.
  uint32_t num_reads_s{};  //!< @brief Number of singleton reads.
  uint32_t num_reads_n{};  //!< @brief Number of reads containing 'N' bases.
  int num_dict_s = kNum_Dict_Encoder;  //!< @brief Number of hash dictionaries
                                       //!< used for encoding.

  int max_read_len{},
      num_thr{};  //!< @brief Maximum read length and number of threads.

  // File paths for various inputs and outputs
  std::string basedir;      //!< @brief Base directory for input/output files.
  std::string infile;       //!< @brief Input file for sequences.
  std::string infile_flag;  //!< @brief Input file for flags.
  std::string infile_pos;   //!< @brief Input file for positions.
  std::string infile_seq;   //!< @brief Input file for sequences.
  std::string infile_rc;    //!< @brief Input file for reverse complement flags.
  std::string infile_read_length;  //!< @brief Input file for read lengths.
  std::string infile_n;            //!< @brief Input file for reads with 'N'.
  std::string outfile_unaligned;   //!< @brief Output file for unaligned reads.
  std::string outfile_seq;         //!< @brief Output file for sequences.
  std::string outfile_pos;         //!< @brief Output file for positions.
  std::string outfile_noise;       //!< @brief Output file for noisy reads.
  std::string outfile_noise_pos;   //!< @brief Output file for noisy positions.
  std::string infile_order;  //!< @brief Input file for order of sequences.
  std::string
      infile_order_n;  //!< @brief Input file for order of 'N' sequences.

  char enc_noise[128][128]{};  //!< @brief Encoded noise data.

  /**
   * @brief Default constructor to initialize all fields.
   */
  EncoderGlobal() = default;
};

/**
 * @brief Structure representing a contig read.
 */
struct ContigReads {
  std::string read;      //!< @brief Read sequence.
  int64_t pos;           //!< @brief Position of the read.
  char rc;               //!< @brief Reverse complement flag.
  uint32_t order;        //!< @brief Order of the read in the original data.
  uint16_t read_length;  //!< @brief Length of the read.
};

/**
 * @brief Build a contig from a list of contig reads.
 *
 * @param current_contig List of reads to build the contig from.
 * @param list_size Number of reads in the list.
 * @return The generated contig sequence as a string.
 */
std::string BuildContig(std::list<ContigReads>& current_contig,
                        const uint32_t& list_size);

/**
 * @brief Write a contig sequence and related data to output files.
 *
 * @param ref The reference sequence.
 * @param current_contig List of reads forming the contig.
 * @param f_seq Output file stream for sequence data.
 * @param f_pos Output file stream for position data.
 * @param f_noise Output file stream for noisy read data.
 * @param f_noise_pos Output file stream for noisy position data.
 * @param f_order Output file stream for order data.
 * @param f_rc Output file stream for reverse complement flags.
 * @param f_read_length Output file stream for read lengths.
 * @param abs_pos The absolute position in the reference.
 */
void WriteContig(const std::string& ref, std::list<ContigReads>& current_contig,
                 std::ofstream& f_seq, std::ofstream& f_pos,
                 std::ofstream& f_noise, std::ofstream& f_noise_pos,
                 std::ofstream& f_order, std::ofstream& f_rc,
                 std::ofstream& f_read_length, uint64_t& abs_pos);

/**
 * @brief Retrieve data parameters for the encoder.
 *
 * @param eg Reference to the encoder global configuration structure.
 * @param cp Compression parameters.
 */
void GetDataParams(EncoderGlobal& eg, const CompressionParams& cp);

/**
 * @brief Correct the order of reads based on the global encoder configuration.
 *
 * @param order_s Array of read orders.
 * @param eg Reference to the global encoder configuration.
 */
void CorrectOrder(std::vector<uint32_t>& order_s,
                  const EncoderGlobal& eg);

/**
 * @brief Convert a bitset to a string representation.
 * @tparam BitsetSize The Size of the bitset used.
 * @param b Bitset to convert.
 * @param read_len Length of the read.
 * @param egb Encoder global configuration with base masks.
 * @return The string representation of the bitset.
 */
template <size_t BitsetSize>
std::string BitsetToString(std::bitset<BitsetSize> b, uint16_t read_len,
                           const EncoderGlobalB<BitsetSize>& egb);

/**
 * @brief Encode a list of reads using the specified dictionary and
 * configurations.
 * @tparam BitsetSize The Size of the bitset used.
 * @param read Array of reads in bitset form.
 * @param dict Pointer to the hash dictionary.
 * @param order_s Array of read orders.
 * @param read_lengths_s Array of read lengths.
 * @param eg Global encoder configuration.
 * @param egb Encoder global configuration for bitset operations.
 */
template <size_t BitsetSize>
void Encode(std::bitset<BitsetSize>* read, BbHashDict* dict, uint32_t* order_s,
            uint16_t* read_lengths_s, const EncoderGlobal& eg,
            const EncoderGlobalB<BitsetSize>& egb);

/**
 * @brief Initialize global arrays for the encoder.
 * @tparam BitsetSize The Size of the bitset used.
 * @param eg Global encoder configuration.
 * @param egb Encoder global configuration for bitset operations.
 */
template <size_t BitsetSize>
void SetGlobalArrays(EncoderGlobal& eg, EncoderGlobalB<BitsetSize>& egb);

/**
 * @brief Read single reads from the input data.
 * @tparam BitsetSize The Size of the bitset used.
 * @param read Array of reads in bitset form.
 * @param order_s Array of read orders.
 * @param read_lengths_s Array of read lengths.
 * @param eg Global encoder configuration.
 * @param egb Encoder global configuration for bitset operations.
 */
template <size_t BitsetSize>
void ReadSingletons(std::bitset<BitsetSize>* read, uint32_t* order_s,
                    uint16_t* read_lengths_s, const EncoderGlobal& eg,
                    const EncoderGlobalB<BitsetSize>& egb);

/**
 * @brief Main encoder function for the Spring encoder.
 * @tparam BitsetSize The Size of the bitset used.
 * @param temp_dir Temporary directory for intermediate files.
 * @param cp Compression parameters.
 */
template <size_t BitsetSize>
void EncoderMain(const std::string& temp_dir, const CompressionParams& cp);

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#include "genie/read/spring/spring_encoding.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_SPRING_ENCODING_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
