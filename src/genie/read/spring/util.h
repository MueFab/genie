/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_UTIL_H_
#define SPRING_UTIL_H_

#include <array>
#include <fstream>
#include <string>
#include <vector>

namespace genie {
namespace read {
namespace spring {

const std::vector<std::array<uint8_t, 2>> subseq_indices = {
    {0, 0},  // pos
    {1, 0},  // rcomp
    {3, 0},  // mmpos
    {3, 1},  // mmpos
    {4, 0},  // mmtype
    {4, 1},  // mmtype
    {6, 0},  // ureads
    {7, 0},  // rlen
    {8, 0},  // pair
    {8, 1},  // pair
    {8, 2},  // pair
    {8, 3},  // pair
    {8, 4},  // pair
    {8, 5},  // pair
    {8, 6},  // pair
    {8, 7},  // pair
    {12, 0}  // rtype
};

const std::string file_subseq_prefix = "subseq";

static const char chartorevchar[128] = {
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 'T', 0, 'G', 0, 0, 0, 'C', 0, 0, 0, 0, 0, 0, 'N', 0, 0, 0, 0, 0, 'A', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   0, 0,   0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct compression_params {
    bool paired_end;
    bool preserve_quality;
    bool preserve_id;
    bool ureads_flag;
    //  bool qvz_flag;
    //  bool ill_bin_flag;
    //  bool bin_thr_flag;
    //  double qvz_ratio;
    //  unsigned int bin_thr_thr;
    //  unsigned int bin_thr_high;
    //  unsigned int bin_thr_low;
    uint32_t num_reads;
    uint32_t num_reads_clean[2];
    uint32_t max_readlen;
    uint32_t num_reads_per_block;
    int num_thr;
    uint32_t num_blocks;
};

std::vector<int64_t> read_vector_from_file(const std::string &file_name);

void reverse_complement(char *s, char *s1, const int readlen);

std::string reverse_complement(const std::string &s, const int readlen);

std::string random_string(size_t length);

}  // namespace spring
}  // namespace read
}  // namespace genie
#endif  // SPRING_UTIL_H_
