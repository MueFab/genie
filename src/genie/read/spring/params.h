/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_PARAMS_H_
#define SRC_GENIE_READ_SPRING_PARAMS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

const uint16_t MAX_READ_LEN = 511;                //!< @brief
const uint32_t MAX_READ_LEN_UREADS = 4294967290;  //!< @brief @note NOT SURE IF THIS LENGTH IS SUPPORTED IN MPEG-G
const uint32_t MAX_NUM_READS = 4294967290;        //!< @brief
const int NUM_DICT_REORDER = 2;                   //!< @brief
const int MAX_SEARCH_REORDER = 1000;              //!< @brief
const int THRESH_REORDER = 4;                     //!< @brief
const float STOP_CRITERIA_REORDER =
    0.5;                         //!< @brief fraction of unmatched reads in last 1M for thread to give up on searching
const int NUM_DICT_ENCODER = 2;  //!< @brief
const int MAX_SEARCH_ENCODER = 1000;     //!< @brief
const int THRESH_ENCODER = 24;           //!< @brief
const int NUM_READS_PER_BLOCK = 256000;  //!< @brief
const int NUM_READS_PER_BLOCK_UREADS =
    256000;  //!< @brief might want NUM_READS_PER_BLOCK_UREADS to be a bit smaller since it can have longer reads
const uint32_t MAX_NUM_TOKENS_ID = 1024;           //!< @brief
const uint32_t BIN_SIZE_COMBINE_PAIRS = 30000000;  //!< @brief number of records put in memory at a time when
                                                   //!< @brief decompressing with combine_pairs on. Higher value
                                                   //!< @brief uses more memory but is slightly faster.

// ---------------------------------------------------------------------------------------------------------------------

#ifdef GENIE_USE_OPENMP

// ---------------------------------------------------------------------------------------------------------------------

const int NUM_LOCKS_REORDER = 0x10000;  //!< @brief # of locks (power of 2)
const int LOCKS_REORDER_MASK = 0xffff;  //!< @brief

/**
 * @brief
 * @param hval
 * @return
 */
inline uint64_t reorder_lock_idx(uint64_t hval) { return hval & LOCKS_REORDER_MASK; }

/**
 *@brief  Add a C++ wrapper around the OpenMP locks.
 * In the future, we will extend the interface with debugging aids.
 */
class omp_lock {
 private:
    omp_lock_t lck;  //!< @brief

 public:
    /**
     * @brief
     */
    omp_lock() { omp_init_lock(&lck); }

    /**
     * @brief
     */
    ~omp_lock() { omp_destroy_lock(&lck); }

    /**
     * @brief
     */
    void set(void) { omp_set_lock(&lck); }

    /**
     * @brief
     */
    void unset(void) { omp_unset_lock(&lck); }

    /**
     * @brief
     * @return
     */
    int test(void) { return omp_test_lock(&lck); }
};

// ---------------------------------------------------------------------------------------------------------------------

#endif /* GENIE_USE_OPENMP */

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_PARAMS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
