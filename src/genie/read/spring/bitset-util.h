/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_BITSET_UTIL_H_
#define SPRING_BITSET_UTIL_H_

#ifdef GENIE_USE_OPENMP

#include <omp.h>

#endif

#include <bbhash/BooPHF.h>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <string>
#include "params.h"

namespace genie {
namespace read {
namespace spring {

typedef boomphf::SingleHashFunctor<u_int64_t> hasher_t;
typedef boomphf::mphf<u_int64_t, hasher_t> boophf_t;

class bbhashdict {
   public:
    boophf_t *bphf;
    int start;
    int end;
    uint32_t numkeys;
    uint32_t dict_numreads;  // number of reads in this dict (for variable length)
    uint32_t *startpos;
    uint32_t *read_id;
    bool *empty_bin = NULL;

    void findpos(int64_t *dictidx, const uint64_t &startposidx);

    void remove(int64_t *dictidx, const uint64_t &startposidx, const int64_t current);

    bbhashdict() {
        bphf = NULL;
        startpos = NULL;
        read_id = NULL;
    }

    ~bbhashdict() {
        if (startpos != NULL) delete[] startpos;
        if (read_id != NULL) delete[] read_id;
        if (empty_bin != NULL) delete[] empty_bin;
        if (bphf != NULL) delete bphf;
    }
};

template <size_t bitset_size>
void stringtobitset(const std::string &s, const uint16_t readlen, std::bitset<bitset_size> &b,
                    std::bitset<bitset_size> **basemask);

template <size_t bitset_size>
void generateindexmasks(std::bitset<bitset_size> *mask1, bbhashdict *dict, int numdict, int bpb);

template <size_t bitset_size>
void constructdictionary(std::bitset<bitset_size> *read, bbhashdict *dict, uint16_t *read_lengths, const int numdict,
                         const uint32_t &numreads, const int bpb, const std::string &basedir, const int &num_threads);

template <size_t bitset_size>
void generatemasks(std::bitset<bitset_size> **mask, const int max_readlen, const int bpb);

template <size_t bitset_size>
void chartobitset(char *s, const int readlen, std::bitset<bitset_size> &b, std::bitset<bitset_size> **basemask);

}  // namespace spring
}  // namespace read
}  // namespace genie

#include "bitset-util.impl.h"

#endif  // SPRING_BITSET_UTIL_H_
