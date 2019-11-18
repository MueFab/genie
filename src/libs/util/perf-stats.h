
#ifndef PERF_STATS_H_
#define PERF_STATS_H_

#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <string>

namespace util {

class PerfStats {
   public:
    bool enabled;
    size_t num_recs;
    std::chrono::steady_clock::duration total_t;
    std::chrono::steady_clock::duration preprocess_t;
    std::chrono::steady_clock::duration reorder_t;
    std::chrono::steady_clock::duration encode_t;
    std::chrono::steady_clock::duration generation_t;
    std::chrono::steady_clock::duration qual_score_t;

    PerfStats() {
        enabled = false;
        num_recs = 0;
        preprocess_t = std::chrono::steady_clock::duration::zero();
        reorder_t = std::chrono::steady_clock::duration::zero();
        encode_t = std::chrono::steady_clock::duration::zero();
        generation_t = std::chrono::steady_clock::duration::zero();
        qual_score_t = std::chrono::steady_clock::duration::zero();
        total_t = std::chrono::steady_clock::duration::zero();
    }

    virtual ~PerfStats() = 0;

    virtual void printCompressionStats(void) = 0;
    virtual void printDecompressionStats(void) = 0;
};

class FastqStats : public PerfStats {
   public:
    off_t orig_id_sz;
    off_t cmprs_id_sz;
    off_t orig_seq_sz;
    off_t cmprs_seq_sz;
    off_t orig_qual_sz;
    off_t cmprs_qual_sz;
    off_t orig_total_sz;
    off_t cmprs_total_sz;

    FastqStats() {
        orig_total_sz = 0;
        cmprs_total_sz = 0;
        orig_id_sz = 0;
        cmprs_id_sz = 0;
        orig_seq_sz = 0;
        cmprs_seq_sz = 0;
        orig_qual_sz = 0;
        cmprs_qual_sz = 0;
    }

    ~FastqStats(){};

    void printCompressionStats(void);
    void printDecompressionStats(void);
};

}  // namespace util

#endif  // PERF_STATS_H_
