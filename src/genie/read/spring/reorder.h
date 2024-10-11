/**
 * @file reorder.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the reordering of reads in paired-end genomic data.
 *
 * This file defines the `reorder_global` structure and associated functions used to reorder reads
 * based on the reference genome and sequence alignment data. The reordering process is essential
 * for efficient encoding and compression of genomic data. This file utilizes various bitset
 * operations and data structures to optimize the process.
 *
 * The file is part of the Spring module within the GENIE project.
 *
 * @tparam bitset_size Template parameter specifying the bitset size used for genomic data representation.
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

namespace genie::read::spring {

/**
 * @brief Global settings and data for the reordering process.
 *
 * This structure contains the global settings, file paths, and other configurations used for the
 * reordering of reads. The `reorder_global` structure is parameterized with the `bitset_size` template
 * parameter, which determines the size of the bitsets used for read representation.
 *
 * @tparam bitset_size Template parameter specifying the bitset size.
 */
template <size_t bitset_size>
struct reorder_global {
    uint32_t numreads;           //!< @brief Number of reads.
    uint32_t numreads_array[2];  //!< @brief Number of reads per file.

    int maxshift, num_thr, max_readlen;    //!< @brief Maximum shift, number of threads, and maximum read length.
    const int numdict = NUM_DICT_REORDER;  //!< @brief Number of dictionaries for reordering.

    std::string basedir;            //!< @brief Base directory for input/output files.
    std::string infile[2];          //!< @brief Input files for paired-end reads.
    std::string outfile;            //!< @brief Output file for reordered reads.
    std::string outfileRC;          //!< @brief Output file for read orientations.
    std::string outfileflag;        //!< @brief Output file for flags.
    std::string outfilepos;         //!< @brief Output file for positions.
    std::string outfileorder;       //!< @brief Output file for read order.
    std::string outfilereadlength;  //!< @brief Output file for read lengths.

    bool paired_end;  //!< @brief Indicates if the reads are paired-end.

    std::bitset<bitset_size> **basemask;  //!< @brief Bitset masks for base representation.
    std::bitset<bitset_size> mask64;      //!< @brief Bitset mask with 64 bits set to 1.

    /**
     * @brief Constructor for the reorder_global structure.
     * @param max_readlen_param Maximum read length.
     */
    explicit reorder_global(int max_readlen_param);

    /**
     * @brief Destructor for the reorder_global structure.
     */
    ~reorder_global();
};

/**
 * @brief Converts a bitset to a string representation.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param b Bitset to be converted.
 * @param s Output string.
 * @param readlen Length of the read.
 * @param rg Global reorder settings.
 */
template <size_t bitset_size>
void bitsettostring(std::bitset<bitset_size> b, char *s, uint16_t readlen, const reorder_global<bitset_size> &rg);

/**
 * @brief Sets up global arrays for the reordering process.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param rg Global reorder settings.
 */
template <size_t bitset_size>
void setglobalarrays(reorder_global<bitset_size> &rg);

/**
 * @brief Updates the reference count based on the current read.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param cur Current read bitset.
 * @param ref Reference bitset.
 * @param revref Reverse reference bitset.
 * @param count Array for storing counts.
 * @param resetcount Flag to indicate if the count should be reset.
 * @param rev Flag to indicate if the read is reverse complemented.
 * @param shift Amount to shift.
 * @param cur_readlen Current read length.
 * @param ref_len Reference length.
 * @param rg Global reorder settings.
 */
template <size_t bitset_size>
void updaterefcount(std::bitset<bitset_size> &cur, std::bitset<bitset_size> &ref, std::bitset<bitset_size> &revref,
                    int **count, bool resetcount, bool rev, int shift, uint16_t cur_readlen, int &ref_len,
                    const reorder_global<bitset_size> &rg);

/**
 * @brief Reads DNA sequences from a file and stores them in bitset format.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param read Array of bitsets to store the reads.
 * @param read_lengths Array of read lengths.
 * @param rg Global reorder settings.
 */
template <size_t bitset_size>
void readDnaFile(std::bitset<bitset_size> *read, uint16_t *read_lengths, const reorder_global<bitset_size> &rg);

/**
 * @brief Searches for a match between the current read and the reference.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param ref Reference bitset.
 * @param mask1 Mask for the read.
 * @param dict Dictionary of reads.
 * @param k Current read index.
 * @param rev Flag to indicate if the read is reverse complemented.
 * @param shift Amount to shift.
 * @param ref_len Reference length.
 * @param rg Global reorder settings.
 * @return True if a match is found, false otherwise.
 */
template <size_t bitset_size>
bool search_match(const std::bitset<bitset_size> &ref, std::bitset<bitset_size> *mask1,
#ifdef GENIE_USE_OPENMP
                  omp_lock *dict_lock, omp_lock *read_lock,
#endif
                  std::bitset<bitset_size> **mask, uint16_t *read_lengths, bool *remainingreads,
                  std::bitset<bitset_size> *read, bbhashdict *dict, uint32_t &k, bool rev, int shift,
                  const int &ref_len, const reorder_global<bitset_size> &rg);

/**
 * @brief Reorders the reads based on the reference sequence.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param read Array of read bitsets.
 * @param dict Dictionary of reads.
 * @param read_lengths Array of read lengths.
 * @param rg Global reorder settings.
 */
template <size_t bitset_size>
void reorder(std::bitset<bitset_size> *read, bbhashdict *dict, uint16_t *read_lengths,
             const reorder_global<bitset_size> &rg);

/**
 * @brief Converts bitsets to string format and writes them to files.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param read Array of read bitsets.
 * @param read_lengths Array of read lengths.
 * @param rg Global reorder settings.
 */
template <size_t bitset_size>
void writetofile(std::bitset<bitset_size> *read, uint16_t *read_lengths, reorder_global<bitset_size> &rg);

/**
 * @brief Main function for the reordering process.
 * @tparam bitset_size Template parameter specifying the bitset size.
 * @param temp_dir Temporary directory for intermediate files.
 * @param cp Compression parameters.
 */
template <size_t bitset_size>
void reorder_main(const std::string &temp_dir, const compression_params &cp);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/read/spring/reorder.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_REORDER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
