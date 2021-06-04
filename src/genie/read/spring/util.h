/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_UTIL_H_
#define SRC_GENIE_READ_SPRING_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <fstream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

static const char chartorevchar[128] = {
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 'T', 0, 'G', 0, 0, 0, 'C', 0, 0, 0, 0, 0, 0, 'N', 0, 0, 0, 0, 0, 'A', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //!<

/**
 *
 */
struct compression_params {
    bool paired_end;               //!<
    bool preserve_quality;         //!<
    bool preserve_id;              //!<
    bool ureads_flag;              //!<
    uint32_t num_reads;            //!<
    uint32_t num_reads_clean[2];   //!<
    uint32_t max_readlen;          //!<
    uint32_t num_reads_per_block;  //!<
    int num_thr;                   //!<
    uint32_t num_blocks;           //!<
};

/**
 *
 * @param s
 * @param s1
 * @param readlen
 */
void reverse_complement(char *s, char *s1, const int readlen);

/**
 *
 * @param s
 * @param readlen
 * @return
 */
std::string reverse_complement(const std::string &s, const int readlen);

/**
 *
 * @param length
 * @return
 */
std::string random_string(size_t length);

/**
 *
 * @param read
 * @param fout
 * @return
 */
void write_dnaN_in_bits(const std::string &read, std::ofstream &fout);

/**
 *
 * @param read
 * @param fin
 * @return
 */
void read_dnaN_from_bits(std::string &read, std::ifstream &fin);

/**
 *
 * @param read
 * @param fout
 * @return
 */
void write_dna_in_bits(const std::string &read, std::ofstream &fout);

/**
 *
 * @param read
 * @param fin
 * @return
 */
void read_dna_from_bits(std::string &read, std::ifstream &fin);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
