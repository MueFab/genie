#ifndef SPRING_PARAMS_H_
#define SPRING_PARAMS_H_

#include <string>

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

namespace spring {

const uint16_t MAX_READ_LEN = 511;
const uint32_t MAX_READ_LEN_UREADS = 4294967290;
// NOT SURE IF THIS LENGTH IS SUPPORTED IN MPEG-G
const uint32_t MAX_NUM_READS = 4294967290;
const int NUM_DICT_REORDER = 2;
const int MAX_SEARCH_REORDER = 1000;
const int THRESH_REORDER = 4;
const float STOP_CRITERIA_REORDER = 0.5;
// fraction of unmatched reads in last 1M for thread to give up on searching
const int NUM_DICT_ENCODER = 2;
const int MAX_SEARCH_ENCODER = 1000;
const int THRESH_ENCODER = 24;
const int NUM_READS_PER_BLOCK = 256000;
const int NUM_READS_PER_BLOCK_UREADS = 256000;
// might want NUM_READS_PER_BLOCK_UREADS to be a bit smaller since it can have
// longer reads
const uint32_t MAX_NUM_TOKENS_ID = 1024;
const uint32_t BIN_SIZE_COMBINE_PAIRS = 10000000;
// BIN_SIZE_COMBINE_PAIRS is number of records put in memory at a time when
// decompressing with combine_pairs on. Higher value uses more memory but is
// slightly faster.
#ifdef GENIE_USE_OPENMP

const int NUM_LOCKS_REORDER = 0x10000;  // # of locks (power of 2)
const int LOCKS_REORDER_MASK = 0xffff;
inline uint64_t reorder_lock_idx(uint64_t hval) { return hval & LOCKS_REORDER_MASK; }

//
// Add a C++ wrapper around the OpenMP locks.
// In the future, we will extend the interface with debugging aids.
//
class omp_lock {
   private:
    omp_lock_t lck;

   public:
    omp_lock() { omp_init_lock(&lck); }
    ~omp_lock() { omp_destroy_lock(&lck); }
    void set(void) { omp_set_lock(&lck); }
    void unset(void) { omp_unset_lock(&lck); }
    int test(void) { return omp_test_lock(&lck); }
};

#endif /* GENIE_USE_OPENMP */

}  // namespace spring

#endif  // SPRING_PARAMS_H_
