/**
 * @file util.h
 * @brief Header file for utility functions and structures used in the Spring module for DNA sequence manipulation.
 *
 * This file defines a set of utility functions for handling DNA sequences, such as generating reverse complements,
 * encoding/decoding DNA sequences in binary format, and other helper methods used in the Spring module.
 * It also defines the `compression_params` struct, which encapsulates configuration options for compressing reads.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_UTIL_H_
#define SRC_GENIE_READ_SPRING_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Lookup table for reverse complement characters.
 *
 * This table maps each character to its reverse complement in the DNA alphabet:
 * - 'A' -> 'T'
 * - 'C' -> 'G'
 * - 'G' -> 'C'
 * - 'T' -> 'A'
 * - 'N' -> 'N'
 *
 * Other characters in the ASCII range are mapped to 0.
 */
static const char chartorevchar[128] = {
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 'T', 0, 'G', 0, 0, 0, 'C', 0, 0, 0, 0, 0, 0, 'N', 0, 0, 0, 0, 0, 'A', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
 * @brief Parameters for configuring the compression of DNA reads.
 *
 * The `compression_params` struct contains fields that specify how the Spring module should compress DNA reads,
 * including options for paired-end reads, quality preservation, number of reads per block, and threading parameters.
 */
struct compression_params {
    bool paired_end;               //!< @brief Flag indicating if the reads are paired-end.
    bool preserve_quality;         //!< @brief Flag indicating if quality values should be preserved.
    bool preserve_id;              //!< @brief Flag indicating if read IDs should be preserved.
    bool ureads_flag;              //!< @brief Flag for unaligned reads.
    uint32_t num_reads;            //!< @brief Total number of reads.
    uint32_t num_reads_clean[2];   //!< @brief Number of clean reads in each of the paired-end files.
    uint32_t max_readlen;          //!< @brief Maximum read length.
    uint32_t num_reads_per_block;  //!< @brief Number of reads per block.
    int num_thr;                   //!< @brief Number of threads for compression.
    uint32_t num_blocks;           //!< @brief Total number of blocks.
};

/**
 * @brief Generate the reverse complement of a DNA sequence.
 *
 * This function calculates the reverse complement of a given DNA sequence and stores it in the output buffer.
 *
 * @param s Pointer to the input DNA sequence.
 * @param s1 Pointer to the buffer where the reverse complement will be stored.
 * @param readlen Length of the DNA sequence.
 */
void reverse_complement(const char *s, char *s1, int readlen);

/**
 * @brief Generate the reverse complement of a DNA sequence.
 *
 * This function takes a DNA sequence as a `std::string` and returns its reverse complement.
 *
 * @param s The input DNA sequence as a `std::string`.
 * @param readlen Length of the DNA sequence.
 * @return The reverse complement of the input sequence.
 */
std::string reverse_complement(const std::string &s, int readlen);

/**
 * @brief Generate a random DNA string of the specified length.
 *
 * This function creates a random DNA sequence using the characters 'A', 'C', 'G', and 'T'.
 *
 * @param length The length of the DNA string to generate.
 * @return A randomly generated DNA sequence of the specified length.
 */
std::string random_string(size_t length);

/**
 * @brief Write a DNA sequence with 'N' bases encoded in bits to a file.
 *
 * This function encodes a DNA sequence containing 'N' bases in binary format and writes it to an output file stream.
 *
 * @param read The input DNA sequence to encode.
 * @param fout The output file stream to write the encoded data to.
 */
void write_dnaN_in_bits(const std::string &read, std::ofstream &fout);

/**
 * @brief Read a DNA sequence with 'N' bases encoded in bits from a file.
 *
 * This function reads a binary-encoded DNA sequence containing 'N' bases from an input file stream and decodes it.
 *
 * @param read The string to store the decoded DNA sequence.
 * @param fin The input file stream to read the encoded data from.
 */
void read_dnaN_from_bits(std::string &read, std::ifstream &fin);

/**
 * @brief Write a DNA sequence encoded in bits to a file.
 *
 * This function encodes a DNA sequence (excluding 'N' bases) in binary format and writes it to an output file stream.
 *
 * @param read The input DNA sequence to encode.
 * @param fout The output file stream to write the encoded data to.
 */
void write_dna_in_bits(const std::string &read, std::ofstream &fout);

/**
 * @brief Read a DNA sequence encoded in bits from a file.
 *
 * This function reads a binary-encoded DNA sequence from an input file stream and decodes it.
 *
 * @param read The string to store the decoded DNA sequence.
 * @param fin The input file stream to read the encoded data from.
 */
void read_dna_from_bits(std::string &read, std::ifstream &fin);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
