/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_BITSET_UTIL_H_
#define SRC_GENIE_READ_SPRING_BITSET_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------

#include <bbhash/BooPHF.h>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <string>
#include "genie/read/spring/params.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

// ---------------------------------------------------------------------------------------------------------------------

typedef boomphf::SingleHashFunctor<uint64_t> hasher_t;
typedef boomphf::mphf<uint64_t, hasher_t> boophf_t;

/**
 * @brief
 */
class bbhashdict {
 public:
    boophf_t *bphf;             //!< @brief
    int start;                  //!< @brief
    int end;                    //!< @brief
    uint32_t numkeys;           //!< @brief
    uint32_t dict_numreads;     //!< number of reads in this dict (for variable length)
    uint32_t *startpos;         //!< @brief
    uint32_t *read_id;          //!< @brief
    bool *empty_bin = nullptr;  //!< @brief

    /**
     * @brief
     * @param dictidx
     * @param startposidx
     */
    void findpos(int64_t *dictidx, const uint64_t &startposidx);

    /**
     * @brief
     * @param dictidx
     * @param startposidx
     * @param current
     */
    void remove(int64_t *dictidx, const uint64_t &startposidx, int64_t current);

    /**
     * @brief
     */
    bbhashdict() {
        bphf = nullptr;
        startpos = nullptr;
        read_id = nullptr;
    }

    /**
     * @brief
     */
    ~bbhashdict() {
        if (startpos != nullptr) delete[] startpos;
        if (read_id != nullptr) delete[] read_id;
        if (empty_bin != nullptr) delete[] empty_bin;
        if (bphf != nullptr) delete bphf;
    }
};

/**
 * @brief
 * @tparam bitset_size
 * @param s
 * @param readlen
 * @param b
 * @param basemask
 */
template <size_t bitset_size>
void stringtobitset(const std::string &s, uint16_t readlen, std::bitset<bitset_size> &b,
                    std::bitset<bitset_size> **basemask);

/**
 * @brief
 * @tparam bitset_size
 * @param mask1
 * @param dict
 * @param numdict
 * @param bpb
 */
template <size_t bitset_size>
void generateindexmasks(std::bitset<bitset_size> *mask1, bbhashdict *dict, int numdict, int bpb);

/**
 * @brief
 * @tparam bitset_size
 * @param read
 * @param dict
 * @param read_lengths
 * @param numdict
 * @param numreads
 * @param bpb
 * @param basedir
 * @param num_threads
 */
template <size_t bitset_size>
void constructdictionary(std::bitset<bitset_size> *read, bbhashdict *dict, uint16_t *read_lengths, int numdict,
                         const uint32_t &numreads, int bpb, const std::string &basedir, const int &num_threads);

/**
 * @brief
 * @tparam bitset_size
 * @param mask
 * @param max_readlen
 * @param bpb
 */
template <size_t bitset_size>
void generatemasks(std::bitset<bitset_size> **mask, int max_readlen, int bpb);

/**
 * @brief
 * @tparam bitset_size
 * @param s
 * @param readlen
 * @param b
 * @param basemask
 */
template <size_t bitset_size>
void chartobitset(char *s, int readlen, std::bitset<bitset_size> &b, std::bitset<bitset_size> **basemask);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/read/spring/bitset-util.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_BITSET_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
