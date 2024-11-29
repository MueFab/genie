/**
 * Copyright 2018-2024 The Genie Authors.
 * @file params.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file defining constants and utility functions for the Spring
 * module.
 *
 * This file provides constants and utility functions used across the Spring
 * module in Genie It includes parameters for read lengths, dictionary sizes,
 * and other settings relevant to the encoding and reordering processes.
 * Additionally, it defines OpenMP lock handling for thread-safe operations when
 * OpenMP is enabled.
 *
 * This file is part of the Spring module within the GENIE project.
 */

#ifndef SRC_GENIE_READ_SPRING_PARAMS_H_
#define SRC_GENIE_READ_SPRING_PARAMS_H_

// -----------------------------------------------------------------------------

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------
// Constants for Spring module configuration

constexpr uint16_t kMax_Read_Len =
    511;  //!< @brief Maximum supported read length.
constexpr uint32_t kMax_Read_Len_Unaligned_Reads =
    4294967290;  //!< @brief Maximum read length for unaligned reads. May not be
                 //!< supported in MPEG-G.
constexpr uint32_t kMax_Num_Reads =
    4294967290;  //!< @brief Maximum number of reads supported.
constexpr int kNum_Dict_Reorder =
    2;  //!< @brief Number of dictionaries used for reordering.
constexpr int kMax_Search_Reorder =
    1000;  //!< @brief Maximum search iterations during reordering.
constexpr int kThresh_Reorder = 4;  //!< @brief Threshold for reordering.
constexpr float kStop_Criteria_Reorder =
    0.5;  //!< @brief Stop criterion for reordering (fraction of unmatched
          //!< reads).
constexpr int kNum_Dict_Encoder =
    2;  //!< @brief Number of dictionaries used for encoding.
constexpr int kMax_Search_Encoder =
    1000;  //!< @brief Maximum search iterations during encoding.
constexpr int kThresh_Encoder = 24;  //!< @brief Threshold for encoding.
constexpr int kNum_Reads_Per_Block =
    256000;  //!< @brief Number of reads per block during compression.
constexpr int kNum_Reads_Per_Block_Unaligned_Reads =
    256000;  //!< @brief Number of reads per block for unaligned reads (longer
             //!< reads).
constexpr uint32_t kMax_Num_Tokens_Id =
    1024;  //!< @brief Maximum number of tokens for read IDs.
constexpr uint32_t kBin_Size_Combine_Pairs =
    30000000;  //!< @brief Size of bins when combining paired reads in memory.

// -----------------------------------------------------------------------------
// OpenMP-specific configuration and utilities

#ifdef GENIE_USE_OPENMP

constexpr int kNum_Locks_Reorder =
    0x10000;  //!< @brief Number of locks (must be a power of 2).
constexpr int kLocks_Reorder_Mask =
    0xffff;  //!< @brief Mask used for lock indexing.

/**
 * @brief Computes the lock index for a given hash value.
 * @param hash_value Hash value.
 * @return Index of the lock to use.
 */
inline uint64_t ReorderLockIdx(const uint64_t hash_value) {
  return hash_value & kLocks_Reorder_Mask;
}

/**
 * @class OmpLock
 * @brief A C++ wrapper for OpenMP locks.
 *
 * This class provides a simple wrapper around OpenMP locks for synchronization
 * purposes. It manages lock creation, destruction, setting, and unsetting
 * operations.
 */
class OmpLock {
  omp_lock_t lck_{};  //!< @brief The internal OpenMP lock.

 public:
  /**
   * @brief Constructor that initializes the lock.
   */
  OmpLock() { omp_init_lock(&lck_); }

  /**
   * @brief Destructor that destroys the lock.
   */
  ~OmpLock() { omp_destroy_lock(&lck_); }

  /**
   * @brief Sets the lock.
   */
  void Set() { omp_set_lock(&lck_); }

  /**
   * @brief Unsets the lock.
   */
  void Unset() { omp_unset_lock(&lck_); }

  /**
   * @brief Tests if the lock is available.
   * @return Non-zero if the lock is successfully set, otherwise 0.
   */
  int Test() { return omp_test_lock(&lck_); }
};

#endif /* GENIE_USE_OPENMP */

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_PARAMS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
