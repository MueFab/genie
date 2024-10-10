/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
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

static const char chartorevchar[128] = {
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 'T', 0, 'G', 0, 0, 0, 'C', 0, 0, 0, 0, 0, 0, 'N', 0, 0, 0, 0, 0, 'A', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //!<

/**
 * @brief
 */
struct compression_params {
    bool paired_end;               //!< @brief
    bool preserve_quality;         //!< @brief
    bool preserve_id;              //!< @brief
    bool ureads_flag;              //!< @brief
    uint32_t num_reads;            //!< @brief
    uint32_t num_reads_clean[2];   //!< @brief
    uint32_t max_readlen;          //!< @brief
    uint32_t num_reads_per_block;  //!< @brief
    int num_thr;                   //!< @brief
    uint32_t num_blocks;           //!< @brief
};

/**
 * @brief
 * @param s
 * @param s1
 * @param readlen
 */
void reverse_complement(const char *s, char *s1, int readlen);

/**
 * @brief
 * @param s
 * @param readlen
 * @return
 */
std::string reverse_complement(const std::string &s, int readlen);

/**
 * @brief
 * @param length
 * @return
 */
std::string random_string(size_t length);

/**
 * @brief
 * @param read
 * @param fout
 * @return
 */
void write_dnaN_in_bits(const std::string &read, std::ofstream &fout);

/**
 * @brief
 * @param read
 * @param fin
 * @return
 */
void read_dnaN_from_bits(std::string &read, std::ifstream &fin);

/**
 * @brief
 * @param read
 * @param fout
 * @return
 */
void write_dna_in_bits(const std::string &read, std::ofstream &fout);

/**
 * @brief
 * @param read
 * @param fin
 * @return
 */
void read_dna_from_bits(std::string &read, std::ifstream &fin);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
