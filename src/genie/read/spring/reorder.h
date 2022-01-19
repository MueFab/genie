/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_REORDER_H_
#define SRC_GENIE_READ_SPRING_REORDER_H_

// ---------------------------------------------------------------------------------------------------------------------

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include "genie/read/spring/bitset-util.h"
#include "genie/read/spring/params.h"
#include "genie/read/spring/util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 * @brief
 * @tparam bitset_size
 */
template <size_t bitset_size>
struct reorder_global {
    uint32_t numreads;           //!< @brief
    uint32_t numreads_array[2];  //!< @brief

    int maxshift, num_thr, max_readlen;    //!< @brief
    const int numdict = NUM_DICT_REORDER;  //!< @brief

    std::string basedir;            //!< @brief
    std::string infile[2];          //!< @brief
    std::string outfile;            //!< @brief
    std::string outfileRC;          //!< @brief
    std::string outfileflag;        //!< @brief
    std::string outfilepos;         //!< @brief
    std::string outfileorder;       //!< @brief
    std::string outfilereadlength;  //!< @brief

    bool paired_end;  //!< @brief

    std::bitset<bitset_size> **basemask;  //!< @brief bitset for A,G,C,T at each position used in stringtobitset,
                                          //!< chartobitset and bitsettostring (alloc in construtctor)
    std::bitset<bitset_size>
        mask64;  //!< @brief bitset with 64 bits set to 1 (used in bitsettostring for conversion to ullong)

    /**
     * @brief
     * @param max_readlen_param
     */
    explicit reorder_global(int max_readlen_param);

    /**
     * @brief
     */
    ~reorder_global();
};

/**
 * @brief
 * @tparam bitset_size
 * @param b
 * @param s
 * @param readlen
 * @param rg
 */
template <size_t bitset_size>
void bitsettostring(std::bitset<bitset_size> b, char *s, const uint16_t readlen, const reorder_global<bitset_size> &rg);

/**
 * @brief
 * @tparam bitset_size
 * @param rg
 */
template <size_t bitset_size>
void setglobalarrays(reorder_global<bitset_size> &rg);

/**
 * @brief
 * @tparam bitset_size
 * @param cur
 * @param ref
 * @param revref
 * @param count
 * @param resetcount
 * @param rev
 * @param shift
 * @param cur_readlen
 * @param ref_len
 * @param rg
 */
template <size_t bitset_size>
void updaterefcount(std::bitset<bitset_size> &cur, std::bitset<bitset_size> &ref, std::bitset<bitset_size> &revref,
                    int **count, const bool resetcount, const bool rev, const int shift, const uint16_t cur_readlen,
                    int &ref_len, const reorder_global<bitset_size> &rg);

/**
 * @brief
 * @tparam bitset_size
 * @param read
 * @param read_lengths
 * @param rg
 */
template <size_t bitset_size>
void readDnaFile(std::bitset<bitset_size> *read, uint16_t *read_lengths, const reorder_global<bitset_size> &rg);

/**
 * @brief
 * @tparam bitset_size
 * @param ref
 * @param mask1
 * @param mask
 * @param read_lengths
 * @param remainingreads
 * @param read
 * @param dict
 * @param k
 * @param rev
 * @param shift
 * @param ref_len
 * @param rg
 * @return
 * @note  the search_match() function is invoked from within the OpenMP parallel region in reorder().
 */
template <size_t bitset_size>
bool search_match(const std::bitset<bitset_size> &ref, std::bitset<bitset_size> *mask1,
#ifdef GENIE_USE_OPENMP
                  omp_lock *dict_lock, omp_lock *read_lock,
#endif
                  std::bitset<bitset_size> **mask, uint16_t *read_lengths, bool *remainingreads,
                  std::bitset<bitset_size> *read, bbhashdict *dict, uint32_t &k, const bool rev, const int shift,
                  const int &ref_len, const reorder_global<bitset_size> &rg);

/**
 * @brief
 * @tparam bitset_size
 * @param read
 * @param dict
 * @param read_lengths
 * @param rg
 */
template <size_t bitset_size>
void reorder(std::bitset<bitset_size> *read, bbhashdict *dict, uint16_t *read_lengths,
             const reorder_global<bitset_size> &rg);

/**
 * @brief convert bitset to string for all num_thr files in parallel
 * @tparam bitset_size
 * @param read
 * @param read_lengths
 * @param rg
 */
template <size_t bitset_size>
void writetofile(std::bitset<bitset_size> *read, uint16_t *read_lengths, reorder_global<bitset_size> &rg);

/**
 * @brief
 * @tparam bitset_size
 * @param temp_dir
 * @param cp
 */
template <size_t bitset_size>
void reorder_main(const std::string &temp_dir, const compression_params &cp);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/read/spring/reorder.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_REORDER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
