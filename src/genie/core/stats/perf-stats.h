/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef PERF_STATS_H_
#define PERF_STATS_H_

// ---------------------------------------------------------------------------------------------------------------------

#ifdef ATOMIC_SZ_CTRS
#include <atomic>
#endif
#include <chrono>
#include <cstdio>
#include <vector>

#include <genie/format/mgb/block.h>
#include <genie/util/data-block.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace stats {

// ---------------------------------------------------------------------------------------------------------------------

//
// Right now, both the collection of the original stream sizes and the
// compressed stream sizes are done in ordered sections.  If this changes,
// use an atomic type to increment the count to avoid data races.
//
#ifdef ATOMIC_SZ_CTRS
typedef std::atomic<off_t> stream_sz_t;
#else
typedef off_t stream_sz_t;
#endif

// ---------------------------------------------------------------------------------------------------------------------

class PerfStats {
   public:
    bool decompression;
    size_t num_recs;
    std::chrono::steady_clock::duration total_t;
    std::chrono::steady_clock::duration preprocess_t;
    std::chrono::steady_clock::duration reorder_t;
    std::chrono::steady_clock::duration encode_t;
    std::chrono::steady_clock::duration generation_t;
    std::chrono::steady_clock::duration qual_score_t;
    std::chrono::steady_clock::duration combine_t;
    stream_sz_t orig_total_sz;
    stream_sz_t cmprs_total_sz;

    PerfStats(bool _decompression);
    virtual ~PerfStats(void) = 0;

    virtual void recordMgbStreams(const std::vector<format::mgb::Block> &blocks) = 0;
    virtual void printStats(void) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

class FastqStats : virtual public PerfStats {
   public:
    stream_sz_t orig_id_sz;
    stream_sz_t cmprs_id_sz;
    stream_sz_t orig_seq_sz;
    stream_sz_t cmprs_seq_sz;
    stream_sz_t orig_qual_sz;
    stream_sz_t cmprs_qual_sz;

    FastqStats(bool _decompression);
    ~FastqStats(void);

    void recordMgbStreams(const std::vector<format::mgb::Block> &blocks);
    void printStats(void);
};

// ---------------------------------------------------------------------------------------------------------------------

class SamStats : virtual public PerfStats {
   public:
    stream_sz_t orig_qname_sz;
    stream_sz_t cmprs_qname_sz;
    stream_sz_t orig_flag_sz;
    stream_sz_t cmprs_flag_sz;
    stream_sz_t orig_rname_sz;
    stream_sz_t cmprs_rname_sz;
    stream_sz_t orig_pos_sz;
    stream_sz_t cmprs_pos_sz;
    stream_sz_t orig_mapq_sz;
    stream_sz_t cmprs_mapq_sz;
    stream_sz_t orig_cigar_sz;
    stream_sz_t cmprs_cigar_sz;
    stream_sz_t orig_rnext_sz;
    stream_sz_t cmprs_rnext_sz;
    stream_sz_t orig_pnext_sz;
    stream_sz_t cmprs_pnext_sz;
    stream_sz_t orig_tlen_sz;
    stream_sz_t cmprs_tlen_sz;
    stream_sz_t orig_seq_sz;
    stream_sz_t cmprs_seq_sz;
    stream_sz_t orig_qual_sz;
    stream_sz_t cmprs_qual_sz;

    SamStats(bool _decompression);
    ~SamStats(void);

    void recordMgbStreams(const std::vector<format::mgb::Block> &blocks);
    void printStats(void);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace stats
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // PERF_STATS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
