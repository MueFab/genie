/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_ENCODING_H_
#define SPRING_ENCODING_H_

#ifdef GENIE_USE_OPENMP

#include <omp.h>

#endif

#include <algorithm>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "bitset-util.h"
#include "params.h"
#include "util.h"

namespace genie {
namespace read {
namespace spring {

template <size_t bitset_size>
struct encoder_global_b {
    std::bitset<bitset_size> **basemask;
    int max_readlen;
    // bitset for A,G,C,T,N at each position
    // used in stringtobitset, and bitsettostring
    std::bitset<bitset_size> mask63;  // bitset with 63 bits set to 1 (used in
    // bitsettostring for conversion to ullong)
    encoder_global_b(int max_readlen_param);

    ~encoder_global_b();
};

struct encoder_global {
    uint32_t numreads, numreads_s, numreads_N;
    int numdict_s = NUM_DICT_ENCODER;

    int max_readlen, num_thr;

    std::string basedir;
    std::string infile;
    std::string infile_flag;
    std::string infile_pos;
    std::string infile_seq;
    std::string infile_RC;
    std::string infile_readlength;
    std::string infile_N;
    std::string outfile_unaligned;
    std::string outfile_seq;
    std::string outfile_pos;
    std::string outfile_noise;
    std::string outfile_noisepos;
    std::string infile_order;
    std::string infile_order_N;

    char enc_noise[128][128];
};

struct contig_reads {
    std::string read;
    int64_t pos;
    char RC;
    uint32_t order;
    uint16_t read_length;
};

std::string buildcontig(std::list<contig_reads> &current_contig, const uint32_t &list_size);

void writecontig(const std::string &ref, std::list<contig_reads> &current_contig, std::ofstream &f_seq,
                 std::ofstream &f_pos, std::ofstream &f_noise, std::ofstream &f_noisepos, std::ofstream &f_order,
                 std::ofstream &f_RC, std::ofstream &f_readlength, uint64_t &abs_pos);

void getDataParams(encoder_global &eg, const compression_params &cp);

void correct_order(uint32_t *order_s, const encoder_global &eg);

template <size_t bitset_size>
std::string bitsettostring(std::bitset<bitset_size> b, const uint16_t readlen,
                           const encoder_global_b<bitset_size> &egb);

template <size_t bitset_size>
void encode(std::bitset<bitset_size> *read, bbhashdict *dict, uint32_t *order_s, uint16_t *read_lengths_s,
            const encoder_global &eg, const encoder_global_b<bitset_size> &egb);

template <size_t bitset_size>
void setglobalarrays(encoder_global &eg, encoder_global_b<bitset_size> &egb);

template <size_t bitset_size>
void readsingletons(std::bitset<bitset_size> *read, uint32_t *order_s, uint16_t *read_lengths_s,
                    const encoder_global &eg, const encoder_global_b<bitset_size> &egb);

template <size_t bitset_size>
void encoder_main(const std::string &temp_dir, const compression_params &cp);

}  // namespace spring
}  // namespace read
}  // namespace genie

#include "spring-encoding.impl.h"

#endif  // SPRING_ENCODER_H_
