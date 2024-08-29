/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_
#define SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void stringtobitset(const std::string &s, const uint16_t readlen, std::bitset<bitset_size> &b,
                    std::bitset<bitset_size> **basemask) {
    for (int i = 0; i < readlen; i++) b |= basemask[i][(uint8_t)s[i]];
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void generateindexmasks(std::bitset<bitset_size> *mask1, bbhashdict *dict, int numdict, int bpb) {
    for (int j = 0; j < numdict; j++) mask1[j].reset();
    for (int j = 0; j < numdict; j++)
        for (int i = bpb * dict[j].start; i < bpb * (dict[j].end + 1); i++) mask1[j][i] = 1;
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void constructdictionary(std::bitset<bitset_size> *read, bbhashdict *dict, uint16_t *read_lengths, const int numdict,
                         const uint32_t &numreads, const int bpb, const std::string &basedir, const int &num_threads) {
    auto *mask = new std::bitset<bitset_size>[numdict];
    generateindexmasks<bitset_size>(mask, dict, numdict, bpb);
    for (int j = 0; j < numdict; j++) {
        auto *ull = new uint64_t[numreads];

        //
        // It is not worth putting parallel region here.
        // This loop barely shows up on the execution profile.
        //
#if 0 && GENIE_USE_OPENMP
#pragma omp parallel num_threads(num_threads)
#endif
        {
            std::bitset<bitset_size> b;
#if 0 && GENIE_USE_OPENMP
            int tid = omp_get_thread_num();
                int num_thr = omp_get_num_threads();
#else
            int tid = 0;
            int num_thr = 1;
#endif
            uint64_t i, stop;
            i = uint64_t(tid) * numreads / num_thr;
            stop = uint64_t(tid + 1) * numreads / num_thr;
            if (tid == num_thr - 1) stop = numreads;
            // compute keys and and store in ull
            for (; i < stop; i++) {
                b = read[i] & mask[j];
                ull[i] = (b >> bpb * dict[j].start).to_ullong();
            }
        }  // parallel end

        // remove keys corresponding to reads shorter than dict_end[j]
        dict[j].dict_numreads = 0;
        for (uint32_t i = 0; i < numreads; i++) {
            if (read_lengths[i] > dict[j].end) {
                ull[dict[j].dict_numreads] = ull[i];
                dict[j].dict_numreads++;
            }
        }

        //
        // write ull to file
        //
        // This region must be executed in parallel with num_threads
        // threads, for correctness.
        //
        // FIXME - make this a parallel for loop that can easily have
        // the OpenMP directive commented out && parallelism disabled.
        //
#if GENIE_USE_OPENMP
#pragma omp parallel num_threads(num_threads)
#endif
        {
#ifdef GENIE_USE_OPENMP
            int tid = omp_get_thread_num();
            int num_thr = omp_get_num_threads();
#else
            int tid = 0;
            int num_thr = 1;
#endif
            std::ofstream foutkey(basedir + std::string("/keys.bin.") + std::to_string(tid), std::ios::binary);
            uint64_t i, stop;
            i = uint64_t(tid) * dict[j].dict_numreads / num_thr;
            stop = uint64_t(tid + 1) * dict[j].dict_numreads / num_thr;
            if (tid == num_thr - 1) stop = dict[j].dict_numreads;
            for (; i < stop; i++) foutkey.write(reinterpret_cast<char *>(&ull[i]), sizeof(uint64_t));
            foutkey.close();
        }  // parallel end

        // deduplicating ull
        std::sort(ull, ull + dict[j].dict_numreads);
        uint32_t k = 0;
        for (uint32_t i = 1; i < dict[j].dict_numreads; i++)
            if (ull[i] != ull[k]) ull[++k] = ull[i];
        dict[j].numkeys = k + 1;
        // construct mphf
        auto data_iterator =
            boomphf::range(static_cast<const uint64_t *>(ull), static_cast<const uint64_t *>(ull + dict[j].numkeys));
        double gammaFactor = 5.0;  // balance between speed and memory
        dict[j].bphf = new boomphf::mphf<uint64_t, hasher_t>(dict[j].numkeys, data_iterator, /*num_thr*/ 1, gammaFactor,
                                                             true, false);

        delete[] ull;

        //
        // compute hashes for all reads
        //
        // This region must be executed in parallel with num_threads
        // threads, for correctness.
        //
        // FIXME - make this a parallel for loop that can easily have
        // the OpenMP directive commented out && parallelism disabled.
        //
        // The bphf lookup() call shows up in the execution profile,
        // so there might be a small benefit from it being executed
        // in parallel on large problem sizes.
        //
#ifdef GENIE_USE_OPENMP
#pragma omp parallel num_threads(num_threads)
#endif
        {
#ifdef GENIE_USE_OPENMP
            int tid = omp_get_thread_num();
            int num_thr = omp_get_num_threads();
#else
            int tid = 0;
            int num_thr = 1;
#endif
            std::ifstream finkey(basedir + std::string("/keys.bin.") + std::to_string(tid), std::ios::binary);
            std::ofstream fouthash(basedir + std::string("/hash.bin.") + std::to_string(tid) + '.' + std::to_string(j),
                                   std::ios::binary);
            uint64_t currentkey, currenthash;
            uint64_t i, stop;
            i = uint64_t(tid) * dict[j].dict_numreads / num_thr;
            stop = uint64_t(tid + 1) * dict[j].dict_numreads / num_thr;
            if (tid == num_thr - 1) stop = dict[j].dict_numreads;
            for (; i < stop; i++) {
                finkey.read(reinterpret_cast<char *>(&currentkey), sizeof(uint64_t));
                //
                // the following line shows up on the execution profile
                //
                currenthash = (dict[j].bphf)->lookup(currentkey);
                fouthash.write(reinterpret_cast<char *>(&currenthash), sizeof(uint64_t));
            }
            finkey.close();
            remove((basedir + std::string("/keys.bin.") + std::to_string(tid)).c_str());
            fouthash.close();
        }  // parallel end
    }

    //
    // for rest of the function, use numdict threads to parallelize
    //
    // This loops must be executed in parallel with num_threads
    // threads, for correctness.
    //
    // When this region shows up in the execution profile, it is always
    // because of a file I/O operation.
    //
    {
#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(std::min(numdict, num_threads))
#endif
        for (int j = 0; j < numdict; j++) {
            // fill startpos by first storing numbers and then doing cumulative sum
            dict[j].startpos = new uint32_t[dict[j].numkeys + 1]();  // 1 extra to store end pos of last key
            uint64_t currenthash;
            for (int tid = 0; tid < num_threads; tid++) {
                std::ifstream finhash(
                    basedir + std::string("/hash.bin.") + std::to_string(tid) + '.' + std::to_string(j),
                    std::ios::binary);
                finhash.read(reinterpret_cast<char *>(&currenthash), sizeof(uint64_t));
                while (!finhash.eof()) {
                    dict[j].startpos[currenthash + 1]++;
                    finhash.read(reinterpret_cast<char *>(&currenthash), sizeof(uint64_t));
                }
                finhash.close();
            }

            dict[j].empty_bin = new bool[dict[j].numkeys]();
            for (uint32_t i = 1; i < dict[j].numkeys; i++)
                dict[j].startpos[i] = dict[j].startpos[i] + dict[j].startpos[i - 1];

            // insert elements in the dict array
            dict[j].read_id = new uint32_t[dict[j].dict_numreads];
            uint32_t i = 0;
            for (int tid = 0; tid < num_threads; tid++) {
                std::ifstream finhash(
                    basedir + std::string("/hash.bin.") + std::to_string(tid) + '.' + std::to_string(j),
                    std::ios::binary);
                finhash.read(reinterpret_cast<char *>(&currenthash), sizeof(uint64_t));
                while (!finhash.eof()) {
                    while (read_lengths[i] <= dict[j].end) i++;
                    dict[j].read_id[dict[j].startpos[currenthash]++] = i;
                    i++;
                    finhash.read(reinterpret_cast<char *>(&currenthash), sizeof(uint64_t));
                }
                finhash.close();
                remove((basedir + std::string("/hash.bin.") + std::to_string(tid) + '.' + std::to_string(j)).c_str());
            }

            // correcting startpos array modified during insertion
            for (int64_t keynum = dict[j].numkeys; keynum >= 1; keynum--)
                dict[j].startpos[keynum] = dict[j].startpos[keynum - 1];
            dict[j].startpos[0] = 0;
        }  // parallel for end
    }
    delete[] mask;
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void generatemasks(std::bitset<bitset_size> **mask, const int max_readlen, const int bpb) {
    // mask for zeroing the end bits (needed while reordering to compute Hamming
    // distance between shifted reads)
    for (int i = 0; i < max_readlen; i++) {
        for (int j = 0; j < max_readlen; j++) {
            mask[i][j].reset();
            for (int k = bpb * i; k < bpb * max_readlen - bpb * j; k++) mask[i][j][k] = 1;
        }
    }
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void chartobitset(char *s, const int readlen, std::bitset<bitset_size> &b, std::bitset<bitset_size> **basemask) {
    b.reset();
    for (int i = 0; i < readlen; i++) b |= basemask[i][(uint8_t)s[i]];
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
