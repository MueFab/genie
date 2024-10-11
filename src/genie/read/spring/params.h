/**
 * @file params.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file defining constants and utility functions for the Spring module.
 *
 * This file provides constants and utility functions used across the Spring module in GENIE. It includes
 * parameters for read lengths, dictionary sizes, and other settings relevant to the encoding and reordering
 * processes. Additionally, it defines OpenMP lock handling for thread-safe operations when OpenMP is enabled.
 *
 * This file is part of the Spring module within the GENIE project.
 */

#ifndef SRC_GENIE_READ_SPRING_PARAMS_H_
#define SRC_GENIE_READ_SPRING_PARAMS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

// ---------------------------------------------------------------------------------------------------------------------
// Constants for Spring module configuration

const uint16_t MAX_READ_LEN = 511;  //!< @brief Maximum supported read length.
const uint32_t MAX_READ_LEN_UREADS =
    4294967290;                             //!< @brief Maximum read length for ureads. May not be supported in MPEG-G.
const uint32_t MAX_NUM_READS = 4294967290;  //!< @brief Maximum number of reads supported.
const int NUM_DICT_REORDER = 2;             //!< @brief Number of dictionaries used for reordering.
const int MAX_SEARCH_REORDER = 1000;        //!< @brief Maximum search iterations during reordering.
const int THRESH_REORDER = 4;               //!< @brief Threshold for reordering.
const float STOP_CRITERIA_REORDER = 0.5;    //!< @brief Stop criterion for reordering (fraction of unmatched reads).
const int NUM_DICT_ENCODER = 2;             //!< @brief Number of dictionaries used for encoding.
const int MAX_SEARCH_ENCODER = 1000;        //!< @brief Maximum search iterations during encoding.
const int THRESH_ENCODER = 24;              //!< @brief Threshold for encoding.
const int NUM_READS_PER_BLOCK = 256000;     //!< @brief Number of reads per block during compression.
const int NUM_READS_PER_BLOCK_UREADS = 256000;     //!< @brief Number of reads per block for ureads (longer reads).
const uint32_t MAX_NUM_TOKENS_ID = 1024;           //!< @brief Maximum number of tokens for read IDs.
const uint32_t BIN_SIZE_COMBINE_PAIRS = 30000000;  //!< @brief Size of bins when combining paired reads in memory.

// ---------------------------------------------------------------------------------------------------------------------
// OpenMP-specific configuration and utilities

#ifdef GENIE_USE_OPENMP

const int NUM_LOCKS_REORDER = 0x10000;  //!< @brief Number of locks (must be a power of 2).
const int LOCKS_REORDER_MASK = 0xffff;  //!< @brief Mask used for lock indexing.

/**
 * @brief Computes the lock index for a given hash value.
 * @param hval Hash value.
 * @return Index of the lock to use.
 */
inline uint64_t reorder_lock_idx(uint64_t hval) { return hval & LOCKS_REORDER_MASK; }

/**
 * @class omp_lock
 * @brief A C++ wrapper for OpenMP locks.
 *
 * This class provides a simple wrapper around OpenMP locks for synchronization purposes.
 * It manages lock creation, destruction, setting, and unsetting operations.
 */
class omp_lock {
 private:
    omp_lock_t lck;  //!< @brief The internal OpenMP lock.

 public:
    /**
     * @brief Constructor that initializes the lock.
     */
    omp_lock() { omp_init_lock(&lck); }

    /**
     * @brief Destructor that destroys the lock.
     */
    ~omp_lock() { omp_destroy_lock(&lck); }

    /**
     * @brief Sets the lock.
     */
    void set() { omp_set_lock(&lck); }

    /**
     * @brief Unsets the lock.
     */
    void unset() { omp_unset_lock(&lck); }

    /**
     * @brief Tests if the lock is available.
     * @return Non-zero if the lock is successfully set, otherwise 0.
     */
    int test() { return omp_test_lock(&lck); }
};

#endif /* GENIE_USE_OPENMP */

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_PARAMS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
