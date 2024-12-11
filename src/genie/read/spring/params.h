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
 *
 * This file is part of the Spring module within the GENIE project.
 */

#ifndef SRC_GENIE_READ_SPRING_PARAMS_H_
#define SRC_GENIE_READ_SPRING_PARAMS_H_

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------
// Constants for Spring module configuration

constexpr uint16_t kMaxReadLen =
    511;  //!< Maximum supported read length.
constexpr uint32_t kMaxReadLenUnalignedReads =
    4294967290;  //!< Maximum read length for unaligned reads. May not be
                 //!< supported in MPEG-G.
constexpr uint32_t kMaxNumReads =
    4294967290;  //!< Maximum number of reads supported.
constexpr int kNumDictReorder =
    2;  //!< Number of dictionaries used for reordering.
constexpr int kMaxSearchReorder =
    1000;  //!< Maximum search iterations during reordering.
constexpr int kThreshReorder = 4;  //!< Threshold for reordering.
constexpr float kStopCriteriaReorder =
    0.5;  //!< Stop criterion for reordering (fraction of unmatched
          //!< reads).
constexpr int kNumDictEncoder =
    2;  //!< Number of dictionaries used for encoding.
constexpr int kMaxSearchEncoder =
    1000;  //!< Maximum search iterations during encoding.
constexpr int kThreshEncoder = 24;  //!< Threshold for encoding.
constexpr int kNumReadsPerBlock =
    256000;  //!< Number of reads per block during compression.
constexpr int kNumReadsPerBlockUnalignedReads =
    256000;  //!< Number of reads per block for unaligned reads (longer
             //!< reads).
constexpr uint32_t kMaxNumTokensId =
    1024;  //!< Maximum number of tokens for read IDs.
constexpr uint32_t kBinSizeCombinePairs =
    30000000;  //!< Size of bins when combining paired reads in memory.

// -----------------------------------------------------------------------------

constexpr int kNumLocksReorder =
    0x10000;  //!< Number of locks (must be a power of 2).
constexpr int kLocksReorderMask =
    0xffff;  //!< Mask used for lock indexing.

/**
 * @brief Computes the lock index for a given hash value.
 * @param hash_value Hash value.
 * @return Index of the lock to use.
 */
inline uint64_t ReorderLockIdx(const uint64_t hash_value) {
  return hash_value & kLocksReorderMask;
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_PARAMS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
