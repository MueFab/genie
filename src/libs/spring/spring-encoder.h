#ifndef GENIE_SPRING_ENCODER_H
#define GENIE_SPRING_ENCODER_H

#include <spring/util.h>
#include <util/perf-stats.h>
#include "mpegg-encoder.h"
namespace genie {
namespace spring {
class SpringEncoder : public MpeggEncoder {
   private:
    int num_thr;
    std::string working_dir;
    bool preserve_quality;
    bool preserve_id;
    spring::compression_params cp;
    util::FastqStats *stats;
    std::string temp_dir;
    std::chrono::time_point<std::chrono::steady_clock> compression_start;

    uint32_t max_readlen;
    uint64_t num_reads[2];
    uint64_t num_reads_clean[2];

    bool *read_contains_N_array;
    uint32_t *read_lengths_array;
    uint32_t num_blocks_done;

    int num_threads;

    std::string outfileN[2];
    std::string outfileorderN[2];
    std::ofstream fout_clean[2];
    std::ofstream fout_N[2];
    std::ofstream fout_order_N[2];
    std::ofstream fout_id;
    std::ofstream fout_quality[2];

    std::chrono::time_point<std::chrono::steady_clock> preprocess_start;

   public:
    void preprocessInit();
    void preprocessIteration(backbone::mpegg_rec::MpeggChunk &&t);
    void preprocessClean();
    SpringEncoder(int _num_thr, std::string _working_dir, bool _ureads_flag, bool _preserve_quality, bool _preserve_id,
                  util::FastqStats &_stats);
    void flowIn(backbone::mpegg_rec::MpeggChunk &&t, size_t id) override;
    void dryIn() override;
};
}  // namespace spring
}  // namespace genie
#endif  // GENIE_SPRING_ENCODER_H
