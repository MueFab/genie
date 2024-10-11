/**
 * @file bitset-util.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for bitset utilities used in the Spring module of GENIE.
 *
 * This file contains function templates and class definitions used for
 * various bitset operations such as conversion, mask generation, and
 * dictionary construction in the Spring module of GENIE.
 *
 * The `bbhashdict` class utilizes the Boomphf library to create
 * perfect hash functions and provides utilities to manage dictionary-based
 * read indexing and compression.
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
 * @class bbhashdict
 * @brief Dictionary class for perfect hashing with BooPHF.
 *
 * This class manages a dictionary of reads using Boomphf's minimal perfect
 * hash functions. It provides utilities to find positions, remove entries,
 * and track whether specific bins in the dictionary are empty.
 */
class bbhashdict {
 public:
    boophf_t *bphf;             //!< BooPHF hash function
    int start;                  //!< Starting position for the dictionary
    int end;                    //!< Ending position for the dictionary
    uint32_t numkeys;           //!< Number of keys in the dictionary
    uint32_t dict_numreads;     //!< Number of reads in this dictionary (for variable length)
    uint32_t *startpos;         //!< Start positions for reads
    uint32_t *read_id;          //!< Read IDs for tracking
    bool *empty_bin = nullptr;  //!< Flags indicating empty bins in the dictionary

    /**
     * @brief Find the position of a given key.
     * @param dictidx Pointer to the dictionary index to fill.
     * @param startposidx Starting position index to search for.
     */
    void findpos(int64_t *dictidx, const uint64_t &startposidx) const;

    /**
     * @brief Remove an entry from the dictionary.
     * @param dictidx Pointer to the dictionary index.
     * @param startposidx Starting position index.
     * @param current Current position to remove.
     */
    void remove(int64_t *dictidx, const uint64_t &startposidx, int64_t current) const;

    /**
     * @brief Default constructor initializing pointers to nullptr.
     */
    bbhashdict() {
        bphf = nullptr;
        startpos = nullptr;
        read_id = nullptr;
    }

    /**
     * @brief Destructor to free allocated memory.
     */
    ~bbhashdict() {
        delete[] startpos;
        delete[] read_id;
        delete[] empty_bin;
        delete bphf;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Convert a string into a bitset based on character values.
 * @tparam bitset_size Size of the bitset.
 * @param s String to convert.
 * @param readlen Length of the string to convert.
 * @param b Bitset to store the converted values.
 * @param basemask Base masks to assist in conversion.
 */
template <size_t bitset_size>
void stringtobitset(const std::string &s, uint16_t readlen, std::bitset<bitset_size> &b,
                    std::bitset<bitset_size> **basemask);

/**
 * @brief Generate index masks for dictionary lookup.
 * @tparam bitset_size Size of the bitset.
 * @param mask1 Array of bitsets to store the masks.
 * @param dict Pointer to the dictionary for which masks are generated.
 * @param numdict Number of dictionaries.
 * @param bpb Bits per base (typically 2 for DNA sequences).
 */
template <size_t bitset_size>
void generateindexmasks(std::bitset<bitset_size> *mask1, bbhashdict *dict, int numdict, int bpb);

/**
 * @brief Construct a dictionary using the given reads and parameters.
 * @tparam bitset_size Size of the bitset.
 * @param read Array of reads in bitset format.
 * @param dict Pointer to the dictionary object.
 * @param read_lengths Array storing the lengths of each read.
 * @param numdict Number of dictionaries.
 * @param numreads Total number of reads.
 * @param bpb Bits per base (typically 2 for DNA sequences).
 * @param basedir Directory for storing intermediate results.
 * @param num_threads Number of threads to use for parallel processing.
 */
template <size_t bitset_size>
void constructdictionary(std::bitset<bitset_size> *read, bbhashdict *dict, const uint16_t *read_lengths, int numdict,
                         const uint32_t &numreads, int bpb, const std::string &basedir, const int &num_threads);

/**
 * @brief Generate masks for each base in the reads.
 * @tparam bitset_size Size of the bitset.
 * @param mask Pointer to an array of bitsets to store the masks.
 * @param max_readlen Maximum read length.
 * @param bpb Bits per base (typically 2 for DNA sequences).
 */
template <size_t bitset_size>
void generatemasks(std::bitset<bitset_size> **mask, int max_readlen, int bpb);

/**
 * @brief Convert a character array into a bitset based on base values.
 * @tparam bitset_size Size of the bitset.
 * @param s Character array representing the DNA sequence.
 * @param readlen Length of the sequence.
 * @param b Bitset to store the converted values.
 * @param basemask Base masks to assist in conversion.
 */
template <size_t bitset_size>
void chartobitset(const char *s, int readlen, std::bitset<bitset_size> &b, std::bitset<bitset_size> **basemask);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/read/spring/bitset-util.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_BITSET_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
